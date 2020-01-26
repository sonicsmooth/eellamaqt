
#include "uimanager.h"
#include "closingdockwidget.h"
#include "libwindow.h"
#include "connable.h"
#include "mymodel.h"
#include <QObject>
#include <QString>
#include <QDockWidget>
#include <QTableView>
#include <QTableWidget>
#include <QTreeView>
#include <QTreeWidget>
#include <QAbstractItemView>
#include <QAbstractItemModel>
#include <typeinfo>
#include <iterator>
#include <list>
#include <vector>
#include <any>
#include <exception>

UIManager::UIManager(QObject *parent) : QObject(parent)
{
    m_defaultUITypes.push_back(UIType::LIBTREEVIEW);
    m_defaultUITypes.push_back(UIType::LIBTABLEVIEW);
}
void UIManager::setParentMW(QMainWindow *p) {
    m_parentMW = p;
}
QMainWindow *UIManager::parentMW() const {
    return m_parentMW;
}

ClosingDockWidget *UIManager::makeLibView(QAbstractItemView *qaiv, QString title) {
    assert(m_pCore);
    LibViewWidget* libViewWidget = new LibViewWidget(qaiv, m_parentMW);
    libViewWidget->setCore(m_pCore);
    libViewWidget->setLogger(m_pLogger);
    libViewWidget->setDbConn(title.toStdString());
    libViewWidget->setMaximumWidth(1000);
    libViewWidget->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding));
    m_openConnWidgets[title.toStdString()].push_back(libViewWidget);

    ClosingDockWidget* libDockWidget = new ClosingDockWidget(m_parentMW);
    libViewWidget->setFocusProxy(libDockWidget); // doesn't seem to do anything
    libDockWidget->setFocusPolicy(Qt::StrongFocus);
    libDockWidget->setWidget(libViewWidget);
    libDockWidget->setWindowTitle(title);
    m_openLibWidgets[title.toStdString()].push_back(libDockWidget);

    return libDockWidget;
}

void UIManager::dockLibView(ClosingDockWidget *libDockWidget, Qt::DockWidgetArea area) {
    // Stick argument to right or left (or wherever), tabifying as necessary
    m_parentMW->addDockWidget(area, libDockWidget, Qt::Orientation::Horizontal);

    // Make this dockwidget stick to right and tabify if needed
    // Get all ClosingDockWidgets and ignore those that are either floating or not in the
    // right dock area, then tabify on top of the first one remaining
    QList<ClosingDockWidget *> cdws0(m_parentMW->findChildren<ClosingDockWidget *>());
    QList<ClosingDockWidget *> cdws; // working copy
    for (auto const & cdw : cdws0) {
        if (m_parentMW->dockWidgetArea(cdw) == area && !cdw->isFloating()) {
            cdws.push_back(cdw);
        }
    }

    // Add to existing right ClosingDockWidget and raise
    if (cdws.length() > 1) {
        m_parentMW->tabifyDockWidget(cdws.first(), libDockWidget);
        m_parentMW->blockSignals(true);
        libDockWidget->setVisible(true);
        libDockWidget->setFocus();
        libDockWidget->raise();
        m_parentMW->blockSignals(false);
    }

    // Resize to fixed width; start with needed lists
    QList<QDockWidget *> qdws; // a base-classier cast-down version of ClosingDockWidget
    QList<int> qdww; // the widths for QDockWidget
    for (auto const & cdw : cdws) {
        qdww.push_back(50);
        qdws.push_back(static_cast<QDockWidget *>(cdw));
    }
    m_parentMW->resizeDocks(qdws, qdww, Qt::Orientation::Horizontal);

    // Send close signal somewhere. When all dockwindows with the same
    // string are closed, UIManager calls core to close library.
    // For some reason these connect calls are active *before* the raise() call above,
    // so when raise is called, the onDockWidgetActive gets called.  The callback for this
    // refers to m_openLibTreeWidgets which has not yet been updated with the libDockWidget, so
    // the assert fails.  Therefore the parentMW signals are blocked above so this
    // callback doesn't get called.`
    QObject::connect(m_parentMW, &QMainWindow::tabifiedDockWidgetActivated, this, &UIManager::onDockWidgetActivate);
    QObject::connect(libDockWidget, &ClosingDockWidget::closing, this, &UIManager::onDockWidgetClose);
}

template <class T>
ClosingDockWidget *findByType(std::list<ClosingDockWidget *> wlst) {
    // wlst is list of QDockWidgets, eg ClosingDockWidgets
    // findByType applies ->widget()->view() to each element in wlst
    // and returns the first one which can be cast to T

    // QDockWidget (in wlst)
    // --- LibViewWidget
    // ------ T
    for (ClosingDockWidget *qdw : wlst) {
        LibViewWidget *lvw(dynamic_cast<LibViewWidget *>(qdw->widget()));
        if (lvw) {
            T *innerThing = dynamic_cast<T*>(lvw->view());
            if (innerThing)
                return qdw;
        }
    }
    return nullptr;
}

void *getWidget(ClosingDockWidget *p) {
    return p->widget();
}

void UIManager::openUI(std::string title) {
    // Called from core typically
    for (auto uit : m_defaultUITypes)
        openUI(title, uit);
}

std::any UIManager::openUI(std::string title, UIType uit) {
    // Ensures only one of any type of view/window is opened for any given title
    // Returns pointer if one already exists, otherwise creates new one
    assert(m_pCore);
    QString qtitle(QString::fromStdString(title));
    ClosingDockWidget *cdw(nullptr);
    if(uit == UIType::LIBTREEVIEW) {
        // usually findByType will return nullptr, as any caller will probably check beforehand
        if (!(cdw = findByType<QTreeView>(m_openLibWidgets[title]))) {
            cdw = makeLibView(new QTreeView, qtitle);
            dockLibView(cdw, Qt::DockWidgetArea::RightDockWidgetArea);
        }
    } else if (uit == UIType::LIBTABLEVIEW) {
        if (!(cdw = findByType<QTableWidget>(m_openLibWidgets[title]))) {
            //QTableWidget *qtw = new QTableWidget(m_parentMW);
            QTableView *qtv = new QTableView(m_parentMW);
            //qtw->setColumnCount(3);
            //qtw->setRowCount(10);
            qtv->setModel(new MyModel);
            cdw = makeLibView(qtv, qtitle);
            dockLibView(cdw, Qt::DockWidgetArea::LeftDockWidgetArea);
        }
    }
    return cdw;
}

void UIManager::retargetUI(std::string oldpath, std::string newpath) {
    // Rename key which points to all open UIs which currently have oldpath, to newpath
    // Update each entry in m_openLibWidgets map.
    // TODO: figure out how data model works and retarget this UI to new or renamed model
    assert(m_pCore);
    assert(m_openLibWidgets.find(oldpath) != m_openLibWidgets.end());

    // Change the title in the map, aka the key
    // https://en.cppreference.com/w/cpp/container/map/extract
    auto t1 = m_openLibWidgets.extract(oldpath);
    t1.key() = newpath;
    m_openLibWidgets.insert(std::move(t1));

    auto t2 = m_openConnWidgets.extract(oldpath);
    t2.key() = newpath;
    m_openConnWidgets.insert(std::move(t2));

    // Set title in each ClosingDockWidget
    int counta = 0;
    int countb = 0;
    for (QDockWidget *pw : m_openLibWidgets[newpath]) {
        pw->setWindowTitle(QString::fromStdString(newpath));
        counta++;
    }

    // Set the connection in each Connection-holding widget
    for (Connable *cw : m_openConnWidgets[newpath]) {
        cw->setDbConn(newpath);
        countb++;
    }
    log("UIManager::retargetui: from %s to %s", oldpath.c_str(), newpath.c_str());
    log("UIManager::retargetUI: retargeted %d dockwidgets", counta);
    log("UIManager::retargetUI: retargeted %d connwidgets", countb);
}

void UIManager::closeUI(std::string title) {
    // Intended to be called from Core, which is is not aware of UIs very much.
    // When called with a given string, this fn will close all UI windows
    // associated with that string and remove string from map
    assert(m_pCore);
    assert(m_openLibWidgets.find(title) != m_openLibWidgets.end());
    assert(m_openConnWidgets.find(title) != m_openConnWidgets.end());
    int n = 0;

    // Can't use for loop because body modifies m_openLibTreeWidgets
    QWidget *pw(nullptr);
    while (!m_openLibWidgets[title].empty()) {
        pw = m_openLibWidgets[title].back();
        //log("UIManager::CloseUI: closing window %s @ 0x%x", title.c_str(), pw);
        pw->close();  // triggers onDockWidgetClose
        n++;
    }
}

void UIManager::onDockWidgetClose(QWidget *pw) {
    // Remove the widget from the open widgets map
    // Remove the viewwidget from the conn widgets map
    // They should track either, but be ignorant of each other
    // So we modify them separately either here or in retargetUI
    // If no more open widgets, then ask core to close lib
    // And ask parent to update menu enables
    assert(m_pCore);
    ClosingDockWidget *cdw(dynamic_cast<ClosingDockWidget *>(pw));
    assert(cdw);
    std::string title(pw->windowTitle().toStdString());
    assert(m_openLibWidgets.find(title) != m_openLibWidgets.end());
    assert(m_openConnWidgets.find(title) != m_openConnWidgets.end());
    std::list<ClosingDockWidget *> & olwlst(m_openLibWidgets[title]);
    std::list<Connable *>          & ocwlst(m_openConnWidgets[title]);
    olwlst.remove(cdw);
    ocwlst.remove(dynamic_cast<Connable *>(cdw->widget()));
    m_parentMW->removeDockWidget(cdw);
    if (olwlst.empty()) {
        assert(ocwlst.empty());
        if (m_pCore->activeDb(title)) { // this could legitimately be false
            m_pCore->closeLib(title);
            dynamic_cast<LibWindow *>(m_parentMW)->updateActions();
        }
    }
}
void UIManager::onDockWidgetActivate(QWidget *pw) {
    // For some reason this gets called twice when each tab is click
    assert(m_pCore);
    QDockWidget *qdw(dynamic_cast<QDockWidget *>(pw));
    assert(qdw);
    std::string title(qdw->windowTitle().toStdString());
    log("UIManager::OnDockWidgetActivate: activated %s", title.c_str());
    m_pCore->pushActiveDb(title);
}
