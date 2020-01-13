
#include "uimanager.h"
#include "closingdockwidget.h"
#include <QObject>
#include <QString>
#include <typeinfo>
#include <iterator>
#include <list>
#include <vector>

UIManager::UIManager(QObject *parent) : QObject(parent)
{

}
void UIManager::setParentMW(QMainWindow *p) {
    m_parentMW = p;
}
QMainWindow *UIManager::parentMW() const {
    return m_parentMW;
}

ClosingDockWidget *UIManager::openLibTreeView(QString title, QString tooltip) {
    assert(m_pCore);
    LibTreeWidget* libTreeWidget = new LibTreeWidget(parentMW());
    libTreeWidget->setCore(m_pCore);
    libTreeWidget->setLogger(m_pLogger);
    libTreeWidget->setDbConn(title.toStdString());
    libTreeWidget->setMaximumWidth(1000);
    log("Size hint: %d, %d", libTreeWidget->sizeHint().width(), libTreeWidget->sizeHint().height());
    libTreeWidget->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding));
    ClosingDockWidget* libDockWidget = new ClosingDockWidget(parentMW());
    libTreeWidget->setFocusProxy(libDockWidget); // doesn't seem to do anything
    libDockWidget->setFocusPolicy(Qt::StrongFocus);
    libDockWidget->setWidget(libTreeWidget);
    libDockWidget->setWindowTitle(title);

    // Make this dockwidget attach and show
    // TODO:: Filter for which ones are currently tabbed; remove from list those that are floating
    parentMW()->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, libDockWidget, Qt::Orientation::Vertical);
    QList<ClosingDockWidget *> cdws0(parentMW()->findChildren<ClosingDockWidget *>());
    QList<ClosingDockWidget *> cdws(cdws0); // working copy
    for (auto const & cdw : cdws0) {
        if (parentMW()->dockWidgetArea(cdw) != Qt::RightDockWidgetArea ||
            cdw->isFloating()) {
            cdws.removeOne(cdw);
        }
    }
    if (cdws.length() > 1) {
        log("Docking '%s' to '%s'", libDockWidget->windowTitle().toStdString().c_str(),
                                    cdws.first()->windowTitle().toStdString().c_str());
        parentMW()->tabifyDockWidget(cdws.first(), libDockWidget);
    }

    parentMW()->blockSignals(true);
    libDockWidget->setVisible(true);
    libDockWidget->setFocus();
    libDockWidget->raise();
    parentMW()->blockSignals(false);

    // Resize to fixed width; start with needed lists
    QList<QDockWidget *> qdws; // a base-classier cast-down version of ClosingDockWidget
    QList<int> qdww; // the widths for QDockWidget
    for (auto const & cdw : cdws) {
        qdww.push_back(50);
        qdws.push_back(static_cast<QDockWidget *>(cdw));
    }
    parentMW()->resizeDocks(qdws, qdww, Qt::Orientation::Horizontal);

    // Send close signal somewhere. When all dockwindows with the same
    // string are closed, UIManager calls core to close library.
    // For some reason these connect calls are active *before* the raise() call above,
    // so when raise is called, the onDockWidgetActive gets called.  The callback for this
    // refers to m_openWidgets which has not yet been updated with the libDockWidget, so
    // the assert fails.  Therefore the parentMW signals are blocked above so this
    // callback doesn't get called.
    QObject::connect(libDockWidget, &ClosingDockWidget::closing, this, &UIManager::onDockWidgetClose);
    QObject::connect(parentMW(), &QMainWindow::tabifiedDockWidgetActivated, this, &UIManager::onDockWidgetActivate);

    return libDockWidget;
}
template<typename TMKEY, typename TMVAL, typename TVAL>
QWidget *findMapValue(std::map<TMKEY, TMVAL> m, TVAL value) {
    // Finds first key/val pair where val == value and returns key
    for (auto const & [k,v] : m)
        if (v == value)
            return k;
    return nullptr;
}

template<typename TKEY, typename TVAL, typename TCMP>
std::vector<QWidget *> findMapValues(std::map<TKEY, TVAL> m, TCMP value) {
    // Finds all key/val pairs where val == value and returns keys
    std::vector<TKEY> ret;
    for (auto const & [k, v]: m) {
        if (v == value)
            ret.push_back(k);
    }
    return ret;
}

std::any UIManager::openUI(UIType uit, std::string title) {
    // Ensures only one of any type of view/window is opened
    // Returns pointer if one already exists, otherwise creates new one
    assert(m_pCore);
    if(uit == UIType::LIBTREEVIEW) {
        QWidget *pw = findMapValue(m_openWidgets, title);
        if (!pw) {
            pw = openLibTreeView(QString::fromStdString(title), "sometooltip");
            m_openWidgets[pw] = title; // needs to be added before openLibTreeView, which triggers onDockWidgetActivate
            log("UIManager::OpenUI: Opened LibView %s ", m_openWidgets[pw].c_str());
        } else {
            log("UIManager::OpenUI: LibView %s already open", m_openWidgets[pw].c_str());
        }
        return pw;
    } else
        return nullptr;
}
void UIManager::retargetUI(std::string oldpath, std::string newpath) {
    // Rename all open UIs which currently have oldpath to newpath
    // Update each entry in m_openWidgets map.
    // TODO: figure out how data model works and retarget this UI to new or renamed model
    assert(m_pCore);
    log("UIManager::retargetUI: retarget from %s to %s",
        oldpath.c_str(), newpath.c_str());
    for (auto pw : findMapValues(m_openWidgets, oldpath)) {
        pw->setWindowTitle(QString::fromStdString(newpath));
        m_openWidgets[pw] = newpath;
    }
}

void UIManager::closeUI(std::string title) {
    // Intended to be called from Core, which is is not aware of UIs very much.
    // When called with a given string, this fn will close all UI windows
    // associated with that string
    assert(m_pCore);
    int n = 0;

    // Can't use for loop because body modifies m_openWidgets
    QWidget *pw(nullptr);
    while ((pw = findMapValue(m_openWidgets, title))) {
        log("UIManager::CloseUI: closing window %s @ 0x%x", m_openWidgets[pw].c_str(), pw);
        pw->close();
        n++;
    }
    log("UIManager::CloseUI: closed %d windows", n);
}

void UIManager::onDockWidgetClose(QWidget *pw) {
    // Remove the widget from the open widgets map
    // If no more open widgets, then ask core to close lib
    assert(m_pCore);
    assert(m_openWidgets.count(pw));
    std::string title(m_openWidgets[pw]);
    m_openWidgets.erase(pw);
    size_t n = m_openWidgets.count(pw);
    if (n == 0) {
        log("UIManager::OnDockWidgetClose: Last reference to %s closed", title.c_str());
        if (m_pCore->activeDb(title))
            m_pCore->closeLib(title);
    } else {
        log("UIManager::OnDockWidgetClose: %d references to %s", n, title.c_str());
    }
}
void UIManager::onDockWidgetActivate(QWidget *pw) {
    // Remove the widget from the open widgets map
    // If no more open widgets, then ask core to close lib
    // For some reason this gets called twice with each tab is click
    assert(m_pCore);
    assert(m_openWidgets.count(pw)); // make sure pw is there
    std::string title(m_openWidgets[pw]);
    log("UIManager::OnDockWidgetActivate: activated %s", title.c_str());
    m_pCore->pushActiveDb(title);
}
