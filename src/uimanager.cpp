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

LibTreeView *UIManager::makeLibTreeView(IDbIf *dbif, std::string fullpath) {
    //QAbstractItemModel *model = m_pSQLTreeModel;
    QSqlDatabase db(dynamic_cast<QSQDbIf *>(dbif)->database(fullpath));
    QStandardItemModel *model(new QSqlTreeModel(m_parentMW, db));
    LibTreeView *view(new LibTreeView(m_parentMW, model, m_pCore, m_pLogger, fullpath));
    m_libViews[model].push_back(view); // store this view keyed to model
    return view;
}
LibTableView *UIManager::makeLibTableView(IDbIf *dbif, std::string fullpath) {
    QSqlDatabase db(dynamic_cast<QSQDbIf *>(dbif)->database(fullpath));
    QSqlTableModel *model(new QSqlTableModel(this, db));
    LibTableView *view(new LibTableView(m_parentMW, model, m_pCore, m_pLogger, fullpath));
    model->setTable("firsttable");
    model->setEditStrategy(QSqlTableModel::OnRowChange);
    model->select();
    m_libViews[model].push_back(view);
    return view;
}
ClosingDockWidget *UIManager::makeCDWLibWidget(QAbstractItemView *view, QString title) {
    view->setMaximumWidth(1000);
    view->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding));
    ClosingDockWidget* libDockWidget(new ClosingDockWidget(m_parentMW));
    view->setFocusProxy(libDockWidget); // doesn't seem to do anything
    libDockWidget->setFocusPolicy(Qt::StrongFocus); // accepts focus by tabbing or clicking
    libDockWidget->setWidget(view);
    libDockWidget->setWindowTitle(title);
    return libDockWidget;
}
ClosingMDIWidget *UIManager::makeMDILibWidget(QAbstractItemView *view, QString title) {
    view->setMaximumWidth(1000);
    view->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding));
    ClosingMDIWidget* libMDIWidget(new ClosingMDIWidget(m_parentMW));
    libMDIWidget->setWidget(view);
    libMDIWidget->setWindowTitle(title);
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

template <class T>
QAbstractItemModel *findDbModelKeyByPath(std::map<QAbstractItemModel *, std::list<QAbstractItemView *>> libViews,
                                          std::string fullpath) {
    // Searches through keys of libViews
    // Returns first T=QSqlTableModel or T=QSqlTreeModel in libViews which points to path.
    // libViews must actually use T as key
    QString qdbpath(QString::fromStdString(fullpath));
    for (auto entry : libViews) {
        if (auto *m = dynamic_cast<T *>(entry.first)) {
            if (m->database().connectionName() == qdbpath)
                return m;
        }
        else
            continue;
    }
    return nullptr;
}

void UIManager::notifyDbOpen(IDbIf *dbif, std::string fullpath) {
    log("Notify open %s", fullpath.c_str());
    for (auto uit : m_defaultViewTypes) {
        openUI(dbif, fullpath, uit);
    }
}

 void UIManager::openUI(IDbIf *dbif, std::string fullpath, ViewType uit) {
    // Ensures only one of any type of view/window is opened for any given title
    // Returns pointer if one already exists, otherwise creates new one
    assert(m_pCore);
    std::string title(std::filesystem::path(fullpath).filename().string());
    QString qtitle(QString::fromStdString(title));
    if(uit == ViewType::LIBTREEVIEW) {
        // Look for db models in keys that have given connection
        // Then check if model has not been used for any views, or ...
        // model has been used already, but not for labtableview
        QAbstractItemModel *model(findDbModelKeyByPath<QSqlTreeModel>(m_libViews, fullpath));
        //QAbstractItemModel *model(&m_siModel);
        if (!model || !findViewBySubType<LibTreeView>(m_libViews[model])) {
            LibTreeView *view(makeLibTreeView(dbif, fullpath));
            ClosingDockWidget *widget(makeCDWLibWidget(view, qtitle));
            dockLibView(widget, Qt::DockWidgetArea::RightDockWidgetArea);
         }
     } else 
     if (uit == ViewType::LIBTABLEVIEW) {
         QAbstractItemModel *model(findDbModelKeyByPath<QSqlTableModel>(m_libViews, fullpath));
         if (!model || !findViewBySubType<LibTableView>(m_libViews[model])) { 
             LibTableView *view(makeLibTableView(dbif, fullpath));
             ClosingDockWidget *widget(makeCDWLibWidget(view, qtitle));
             dockLibView(widget, Qt::DockWidgetArea::LeftDockWidgetArea);
         }
 //    } else if (uit == UIType::LIBSYMBOLEDITOR) {

     }
 //    return nullptr;
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
    log("Notify close %s", fullpath.c_str());
    // Close all widgets that ultimately connect to fullpath

    
}

// void UIManager::closeUI(std::string connpath) {
//     // Intended to be called from Core, which is is not aware of UIs very much.
//     // When called with a given string, this fn will close all QAbstractItemViews
//     // which have a model associated with that string and remove string from map
//     QAbstractItemModel *model = findSqlModelKeyByPath(m_libViews, QString::fromStdString(connpath));
//     // ok if null, as this was maybe called indirectly from onDockWidgetClose
//     if (!model)
//         return;

//     // Can't use for loop because body modifies m_libViews
//     while (!m_libViews[model].empty()) {
//         QAbstractItemView *view(m_libViews[model].back());
//         view->parentWidget()->close();  // triggers onDockWidgetClose
//     }
// }

void UIManager::notifyDbRename(IDbIf *dbif, std::string oldpath, std::string newpath) {
    log("Notify rename %s to %s", oldpath.c_str(), newpath.c_str());
    
}


void UIManager::onDockWidgetClose(QWidget *pw) {
    // Remove the widget from the open widgets map
    // Remove the viewwidget from the conn widgets map
    // They should track either, but be ignorant of each other
    // So we modify them separately either here or in retargetUI
    // If no more open widgets, then ask core to close lib
    // And ask parent to update menu enables
    assert(m_pCore);
    ClosingDockWidget *cdw(static_cast<ClosingDockWidget *>(pw));
    QAbstractItemView *libview(static_cast<QAbstractItemView *>(cdw->widget()));
    QAbstractItemModel *model = libview->model();
    m_libViews[model].remove(libview);
    if (m_libViews[model].empty()) {
        m_libViews.erase(model); // Delete key/value entry entirely;
        // could be that model is not a sql model, so dynamic cast and check
        if (auto *sqlmodel = dynamic_cast<QSqlTableModel *>(model)) {
            std::string conn(sqlmodel->database().databaseName().toStdString());
//            if (m_pCore->activeDb(conn)) {
//                m_pCore->closeLib(conn);
//                static_cast<LibWindow *>(m_parentMW)->updateActions();
//            }
        }
    }
}
void UIManager::onDockWidgetActivate(QWidget *pw) {
    // For some reason this gets called twice when each tab is click
    assert(m_pCore);
    auto *qdw(dynamic_cast<QDockWidget *>(pw));
    assert(qdw);
    std::string title(qdw->windowTitle().toStdString());
    log("UIManager::OnDockWidgetActivate: activated %s", title.c_str());
//    m_pCore->pushActiveDb(title);
}


// void UIManager::setModelManager(IModelManager *pmm) {
//     m_pModelManager = pmm;
// }
// IModelManager *UIManager::modelManager() const {
//     return m_pModelManager;
// }
// void UIManager::setViewManager(IViewManager *pvm) {
//     m_pViewManager = pvm;
// }
// IViewManager *UIManager::viewManager() const {
//     return m_pViewManager;
// }
