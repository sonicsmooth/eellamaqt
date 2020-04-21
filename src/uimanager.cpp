#include "uimanager.h"
#include "closingdockwidget.h"
#include "closingmdiwidget.h"
#include "libwindow.h"
#include "connable.h"
#include "mymodel.h"
#include "libtableview.h"
#include "libtreeview.h"
#include "qsqltreemodel.h"
#include "qdbif.h"
#include "mvtypes.h"

#include <QObject>
#include <QString>
#include <QDockWidget>
#include <QTableView>
#include <QTableWidget>
#include <QTreeView>
#include <QTreeWidget>
#include <QAbstractItemView>
#include <QStandardItem>
#include <QItemSelectionModel>
#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QItemSelection>

#include <filesystem>
#include <typeinfo>
#include <iterator>
#include <list>
#include <vector>
#include <exception>

UIManager::UIManager(QObject *parent) :
    QObject(parent)
{
    m_defaultViewTypes.push_back(ViewType::LIBTREEVIEW);
    m_defaultViewTypes.push_back(ViewType::LIBTABLEVIEW);
}
void UIManager::setParentMW(QMainWindow *p) {
    m_parentMW = p;
}
QMainWindow *UIManager::parentMW() const {
    return m_parentMW;
}

QAbstractItemView *UIManager::makeLibTreeView(IDbIf *dbif, std::string fullpath) {
    //QAbstractItemModel *model = m_pSQLTreeModel;
    QSqlDatabase db(dynamic_cast<QSQDbIf *>(dbif)->database(fullpath));
    QSqlTreeModel *model(new QSqlTreeModel(m_parentMW, db));
    LibTreeView *view(new LibTreeView(m_parentMW, model, m_pCore, m_pLogger, fullpath));
    m_connViews[fullpath][ViewType::LIBTREEVIEW].push_back(view);
    return view;
}
QAbstractItemView *UIManager::makeLibTableView(IDbIf *dbif, std::string fullpath) {
    QSqlDatabase db(dynamic_cast<QSQDbIf *>(dbif)->database(fullpath));
    QSqlTableModel *model(new QSqlTableModel(this, db));
    LibTableView *view(new LibTableView(m_parentMW, model, m_pCore, m_pLogger, fullpath));
    model->setTable("firsttable");
    model->setEditStrategy(QSqlTableModel::OnRowChange);
    model->select();
    m_connViews[fullpath][ViewType::LIBTABLEVIEW].push_back(view);
    return view;
}
ClosingDockWidget *UIManager::makeCDWLibWidget(QAbstractItemView *view, std::string title) {
    view->setMaximumWidth(1000);
    view->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding));
    ClosingDockWidget* libDockWidget(new ClosingDockWidget(m_parentMW));
    view->setFocusProxy(libDockWidget); // doesn't seem to do anything
    libDockWidget->setFocusPolicy(Qt::StrongFocus); // accepts focus by tabbing or clicking
    libDockWidget->setWidget(view);
    libDockWidget->setWindowTitle(QString::fromStdString(title));
    return libDockWidget;
}
ClosingMDIWidget *UIManager::makeMDILibWidget(QAbstractItemView *view, std::string title) {
    view->setMaximumWidth(1000);
    view->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding));
    ClosingMDIWidget* libMDIWidget(new ClosingMDIWidget(m_parentMW));
    libMDIWidget->setWidget(view);
    libMDIWidget->setWindowTitle(QString::fromStdString(title));
    return libMDIWidget;
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
T *findViewBySubType(std::list<QAbstractItemView *> lst) {
    // Return first element that can be cast to T*
    // T is typically LibTreeView, LibTableView, etc.
    for (QAbstractItemView *v : lst)
        if (T *t = dynamic_cast<T *>(v))
            return t;
    return nullptr;
}

void UIManager::notifyDbOpen(IDbIf *dbif, std::string fullpath) {
    log("Notify open %s", fullpath.c_str());
    for (auto uit : m_defaultViewTypes) {
        openUI(dbif, fullpath, uit);
    }
}

void UIManager::openUI(IDbIf *dbif, std::string fullpath, ViewType vt) {
    // Ensures only one of any type of view is opened for any given title
    // Returns pointer if one already exists, otherwise creates new one

    std::map<ViewType, Qt::DockWidgetArea> dwam = 
     {{ViewType::LIBTREEVIEW, Qt::DockWidgetArea::RightDockWidgetArea},
      {ViewType::LIBTABLEVIEW, Qt::DockWidgetArea::LeftDockWidgetArea}};
    
    typedef QAbstractItemView * (UIManager::*abstractViewFn)(IDbIf *, std::string);
    std::map<ViewType, abstractViewFn> dwfm = 
     {{ViewType::LIBTREEVIEW, &UIManager::makeLibTreeView},
      {ViewType::LIBTABLEVIEW, &UIManager::makeLibTableView}};

    if (m_connViews[fullpath].size() && m_connViews[fullpath][vt].size())
        // For now just returning thexjdy.bjhhnecbtccckyytpxixky.k.i. first view
        return;
    else { // create new view
        QAbstractItemView *view = (this->*dwfm[vt])(dbif, fullpath); // from https://stackoverflow.com/questions/14814158/c-call-pointer-to-member-function
        std::string title(std::filesystem::path(fullpath).filename().string());
        ClosingDockWidget *widget(makeCDWLibWidget(view, title));
        dockLibView(widget, dwam[vt]);
    } 
 }

 //            connect(qtv->selectionModel(), &QItemSelectionModel::selectionChanged,
 //                [=](const QItemSelection & /*newSelection*/, const QItemSelection & /*oldSelection*/) {
 //                    const QModelIndex index = qtv->selectionModel()->currentIndex();
 //                    QString selectedText = index.data(Qt::DisplayRole).toString();
 //                    log("%s, level %d", selectedText.toStdString().c_str());
 //                });

// void UIManager::retargetUI(std::string oldpath, std::string newpath) {
//     // Rename key which points to all open UIs which currently have oldpath, to newpath
//     // Update each entry in m_libViews map.
    
//     QAbstractItemModel *newmodel(findSqlModelKeyByPath(m_libViews, QString::fromStdString(newpath)));
//     QAbstractItemModel *oldmodel(findSqlModelKeyByPath(m_libViews, QString::fromStdString(oldpath)));
//     assert(!newmodel);
//     assert(oldmodel);

//     // Change the title in the map, aka the key
//     // https://en.cppreference.com/w/cpp/container/map/extract
//     auto t1 = m_libViews.extract(oldmodel);
//     QSqlDatabase db = std::any_cast<QSqlDatabase>(m_pDbIf->database(newpath));
//     newmodel = new QSqlTableModel(this, db);
//     assert(newmodel);
//     t1.key() = newmodel;
//     m_libViews.insert(std::move(t1));
//     // TODO: verify we don't need to close the old database from here
//     // I think core does that

//     // Retarget model in each libview
//     for (QAbstractItemView *view : m_libViews[newmodel]) {
//         view->setModel(newmodel);
//         view->parentWidget()->setWindowTitle("retargeted title");
//     }
    
//     delete oldmodel;
// }

void UIManager::notifyDbClose(IDbIf *dbif, std::string fullpath) {
    (void) dbif;
    // Close all widgets that are downstream from fullpath
    // Suspend event before closing
    log("Notify close %s", fullpath.c_str());
    while (!m_connViews[fullpath].empty() &&
           !m_connViews[fullpath].begin()->second.empty()) {
        auto view(m_connViews[fullpath].begin()->second.front());
        auto *cdw(static_cast<ClosingDockWidget *>(view->parentWidget()));
        cdw->blockSignals(true);
        cdw->close();
        removeView(cdw);
    }
}

void UIManager::notifyDbRename(IDbIf *dbif, std::string oldpath, std::string newpath) {
    (void) dbif;
    log("Notify rename %s to %s", oldpath.c_str(), newpath.c_str());
}

auto getModelViewFromWidget(QWidget *pw) {
    QDockWidget *qdw(static_cast<QDockWidget *>(pw));
    QAbstractItemView *view(static_cast<QAbstractItemView *>(qdw->widget()));
    QAbstractItemModel *model(view->model());
    struct retVals {
        QAbstractItemModel *model;
        QAbstractItemView *view;
    };
    return retVals {model, view};
}

auto getFullpathFromModel(QAbstractItemModel *model) {
    QSqlTreeModel *treeModel(dynamic_cast<QSqlTreeModel *>(model));
    QSqlTableModel *tableModel(dynamic_cast<QSqlTableModel *>(model));
    std::string fullpath(treeModel  ? treeModel->database().connectionName().toStdString():
                         tableModel ? tableModel->database().connectionName().toStdString() :
                         throw("Neither treeModel nor tableModel"));
    ViewType vt(treeModel  ? ViewType::LIBTREEVIEW :
                tableModel ? ViewType::LIBTABLEVIEW :
                throw("Neither treeModel nor tableModel"));
    struct retVals {
        std::string fullpath;
        QSqlTreeModel *treeModel;
        QSqlTableModel *tableModel;
        ViewType vt;
    };
    return retVals {fullpath, treeModel, tableModel, vt};
}

void UIManager::removeView(QWidget *pw) {
    // Finds view and removes it from list.
    // if doCloseLib is true, then when view is the last from
    // the list, calls closeLib.
    // Returns true if all views from fullpath are closed
    auto [model, view] = getModelViewFromWidget(pw);
    auto [fullpath, treeModel, tableModel, vt] = getFullpathFromModel(model);
    
    // Remove view from list; if list is empty, delete 
    // <viewtype, list> from map keyed by fullpath
    // if m_connViews[fullpath] is empty, then close lib
    m_connViews[fullpath][vt].remove(view);
    if (m_connViews[fullpath][vt].empty()) {
        m_connViews[fullpath].erase(vt);
        // This model is done, so delete which causes db connection to close
        delete model;
        if (m_connViews[fullpath].empty()) {
            m_connViews.erase(fullpath);
            m_pCore->closeLibNoGui(fullpath);
        } 
    }
}

void UIManager::onDockWidgetClose(QWidget *pw) {
    // Remove the view from list and allow library to be closed
    // This happens when user clicks the close button
    removeView(pw);
    dynamic_cast<LibWindow *>(m_parentMW)->updateActions(m_pCore->DbIf()->isDatabaseOpen());
}

void UIManager::onDockWidgetActivate(QWidget *pw) {
    // For some reason this gets called twice when each tab is click
    assert(m_pCore);
    auto [model, _v] = getModelViewFromWidget(pw);
    auto [fullpath, _x, _y, _z] = getFullpathFromModel(model);
    log("UIManager::OnDockWidgetActivate: activated %s", fullpath.c_str());
    m_pCore->activateLib(fullpath);
}

