#include "uimanager.h"
#include "closingdockwidget.h"
#include "closingmdiwidget.h"
#include "libwindow.h"
#include "connable.h"
#include "mymodel.h"
#include "libsymbolview.h"
#include "libtableview.h"
#include "libtreeview.h"
#include "qsqltreemodel.h"
#include "qdbif.h"
#include "mvtypes.h"

#include <QApplication>
#include <QObject>
#include <QString>
#include <QDockWidget>
#include <QTableView>
#include <QTableWidget>
#include <QTreeView>
#include <QTreeWidget>
#include <QAbstractItemView>
//#include <QStandardItem>
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
#include <optional>


//////////
// LOCAL
//////////
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


//////////
// PRIVATE
//////////
std::optional<ConnView> UIManager::selectWhere(std::string conn, ViewType vt) {
    // Go through each item in cvs and return first
    // entry where conn and viewtype match.
    // Returns std::optional::nullopt when nothing found
    for (auto cv : m_connViews) {
        if (cv.conn == conn && cv.vt == vt)
            return cv;
    }
    return std::nullopt;
}
std::optional<ConnView> UIManager::selectWhere(std::string conn, ViewType vt, QMainWindow *mw) {
    // Go through each item in cvs and return first
    // entry where conn, viewtype, and mainwindow match.
    // Returns std::optional::nullopt when nothing found
    for (auto cv : m_connViews) {
        if (cv.conn == conn && cv.vt == vt && cv.mainwindow==mw)
            return cv;
    }
    return std::nullopt;
}
ConnViews UIManager::selectWheres(std::string conn, ViewType vt) {
    // Go through each item in cvs and return list of matching entries
    // List is empty if nothing is found
    ConnViews retval;
    for (auto cv: m_connViews) {
        if(cv.conn == conn && cv.vt == vt)
            retval.push_back(cv);
    }
    return retval;
}

void UIManager::newMainWindow(IDbIf *dbif, std::string fullpath) {
    // // Create new LibWindow
    // auto modelopt = selectWhere(fullpath, ViewType::LIBFOOTPRINTVIEW);
    // QAbstractItemModel *model(nullptr);
    // if (modelopt) {
    //     model = (*modelopt).model;
    // } else {
    //     QSqlDatabase db(dynamic_cast<QSQDbIf *>(dbif)->database(fullpath));
    //     QSqlTableModel *model(new QSqlTableModel(this, db));
    //     model->setTable("firsttable");
    //     model->setEditStrategy(QSqlTableModel::OnRowChange);
    //     model->select();
    // }

    // QAbstractItemView *view(new LibSymbolView(m_parentMW, model, m_pCore, m_pLogger, fullpath));
    // view->setMaximumWidth(1000);
    // view->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding));
    // ClosingMDIWidget* libMDIWidget(new ClosingMDIWidget(m_parentMW));
    // libMDIWidget->setWidget(view);
    // std::string title(std::filesystem::path(fullpath).filename().string());
    // libMDIWidget->setWindowTitle(QString::fromStdString(title));

    // m_parentMW = new LibWindow();
    // static_cast<LibWindow *>(m_parentMW)->mdiArea()->setTabsClosable(true);
    // static_cast<LibWindow *>(m_parentMW)->mdiArea()->addSubWindow(libMDIWidget);

    // m_connViews.push_back({fullpath, ViewType::LIBFOOTPRINTVIEW, model, view, true, m_parentMW});
    // m_parentMW->show();
}

QAbstractItemModel *UIManager::makeLibSymbolModel(IDbIf *dbif, std::string fullpath) {
    QSqlDatabase db(dynamic_cast<QSQDbIf *>(dbif)->database(fullpath));
    return new QSqlTableModel(this, db);
}
QAbstractItemModel *UIManager::makeLibTreeModel(IDbIf *dbif, std::string fullpath) {
    QSqlDatabase db(dynamic_cast<QSQDbIf *>(dbif)->database(fullpath));
    return new QSqlTreeModel(m_parentMW, db);
}
QAbstractItemModel *UIManager::makeLibTableModel(IDbIf *dbif, std::string fullpath) {
    QSqlDatabase db(dynamic_cast<QSQDbIf *>(dbif)->database(fullpath));
    return new QSqlTableModel(this, db);
}

QAbstractItemView *UIManager::makeLibSymbolView(QAbstractItemModel *model/*, std::string fullpath*/) {
    // Make sure we're getting the right type of model
    QSqlTableModel *sqlModel(dynamic_cast<QSqlTableModel *>(model));
    if (!sqlModel)
        throw("Not a QSqlTableModel");
    QAbstractItemView *view(new LibSymbolView(m_parentMW, model, m_pCore, m_pLogger/*, fullpath)*/));
    sqlModel->setTable("firsttable");
    sqlModel->setEditStrategy(QSqlTableModel::OnRowChange);
    sqlModel->select();
    return view;
}
QAbstractItemView *UIManager::makeLibTreeView(QAbstractItemModel *model/*, std::string fullpath*/) {
    // Make sure we're getting the right type of model
    QSqlTreeModel *sqlModel(dynamic_cast<QSqlTreeModel *>(model));
    if (!sqlModel)
        throw("Not a QSqlTreeModel");
    QAbstractItemView *view(new LibTreeView(m_parentMW, model, m_pCore, m_pLogger/*, fullpath*/));
    // model->setTable("firsttable");
    // model->setEditStrategy(QSqlTableModel::OnRowChange);
    // model->select();
    //m_connViews[fullpath][ViewType::LIBTREEVIEW].push_back(view);
    return view;
}
QAbstractItemView *UIManager::makeLibTableView(QAbstractItemModel *model/*, std::string fullpath*/) {
    // Make sure we're getting the right type of model
    QSqlTableModel *sqlModel(dynamic_cast<QSqlTableModel *>(model));
    if (!sqlModel)
        throw("Not a QSqlTableModel");
    QAbstractItemView *view(new LibTableView(m_parentMW, model, m_pCore, m_pLogger/*, fullpath*/));
    sqlModel->setTable("firsttable");
    sqlModel->setEditStrategy(QSqlTableModel::OnRowChange);
    sqlModel->select();
    //m_connViews[fullpath][ViewType::LIBTABLEVIEW].push_back(view);
    return view;
}
ClosingDockWidget *UIManager::makeCDWLibWidget(QWidget *parent, QAbstractItemView *view, std::string title) {
    view->setMaximumWidth(1000);
    view->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding));
    ClosingDockWidget* libDockWidget(new ClosingDockWidget(parent));
    view->setFocusProxy(libDockWidget); // doesn't seem to do anything
    libDockWidget->setFocusPolicy(Qt::StrongFocus); // accepts focus by tabbing or clicking
    libDockWidget->setWidget(view);
    libDockWidget->setWindowTitle(QString::fromStdString(title));
    return libDockWidget;
}
ClosingMDIWidget *UIManager::makeMDILibWidget(QWidget *parent, QAbstractItemView *view, std::string title) {
    view->setMaximumWidth(1000);
    view->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding));
    ClosingMDIWidget* widget(new ClosingMDIWidget(parent));
    widget->setWidget(view);
    widget->setWindowTitle(QString::fromStdString(title));
    widget->setAttribute(Qt::WA_DeleteOnClose);
    return widget;
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
        qdww.push_back(100);
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
void UIManager::openUI(IDbIf *dbif, std::string fullpath, ViewType vt) {
    // For LibSymbolView, allows any number of instances in any number of mainwindows
    // For auxilliary views, only one type of view is allowed per mainwindow

    if (vt == ViewType::LIBSYMBOLVIEW) {
        QAbstractItemModel *model(nullptr);
        QAbstractItemView *view(nullptr);
        QWidget *w(QApplication::activeWindow());
        LibWindow *mw = static_cast<LibWindow *>(w ? w : m_parentMW);
        auto modelopt(selectWhere(fullpath, vt, mw));
        model = modelopt ? (*modelopt).model : (this->*makeModelfm[vt])(dbif, fullpath);
        view = (this->*makeViewfm[vt])(model);
        std::string title(std::filesystem::path(fullpath).filename().string());
        ClosingMDIWidget *widget(makeMDILibWidget(nullptr, view, title));
        mw->mdiArea()->addSubWindow(widget);
        widget->show();
        
    } else {
        
    }

    // if libview
    //    look for model, use that if found, else new model
    //    create new view, new mdiwidget
    //    add mdiwidget to current mainwindow 
    // else if treeview
    //    look for tree model, use that if found, else new tree model
    //    look for tree view for this main window and get its parentwidget
    //    confirm parent widget in table, error if other
    //    If no view, create new view, new dock widget
    //    dock


    // if (m_connViews[fullpath].size() && m_connViews[fullpath][vt].size()) {
    //     // For now just returning the first view
    //     updateTitle();
    //     return;
    // }
    // else { // create new view
    //     QAbstractItemView *view = (this->*dwfm[vt])(dbif, fullpath); // from https://stackoverflow.com/questions/14814158/c-call-pointer-to-member-function
    //     std::string title(std::filesystem::path(fullpath).filename().string());
    //     if (dwam[vt] > Qt::DockWidgetArea::NoDockWidgetArea) {
    //         ClosingDockWidget *widget(makeCDWLibWidget(view, title));
    //         dockLibView(widget, dwam[vt]);
    //     }
    //     else {
    //         assert(m_parentMW);
    //         ClosingMDIWidget *widget(makeMDILibWidget(view, title));
    //         static_cast<LibWindow *>(m_parentMW)->mdiArea()->setTabsClosable(true);
    //         static_cast<LibWindow *>(m_parentMW)->mdiArea()->addSubWindow(widget);
    //     }
    //     updateTitle();
    // } 
 }
void UIManager::updateTitle() {
    // auto db(m_pCore->DbIf()->activeDatabase());
    // if (db) {
    //     static_cast<LibWindow *>(m_parentMW)->updateTitle(db.value());
    //     std::map<ViewType, std::list<QAbstractItemView *>> mp = m_connViews[db.value()];
    //     QAbstractItemView *view = mp.begin()->second.front();
    //     ClosingDockWidget *libDockWidget = static_cast<ClosingDockWidget *>(view->parentWidget());

    //     m_parentMW->blockSignals(true);
    //     libDockWidget->setVisible(true);
    //     libDockWidget->setFocus();
    //     libDockWidget->raise();
    //     m_parentMW->blockSignals(false);

    // }
    // else
    //     static_cast<LibWindow *>(m_parentMW)->updateTitle();
}
void UIManager::onDockWidgetClose(QWidget *pw) {
    // Remove the view from list and allow library to be closed
    // This happens when user clicks the close button
    //removeView(pw);
    dynamic_cast<LibWindow *>(m_parentMW)->updateActions(m_pCore->DbIf()->isDatabaseOpen());
    // updateTitle();
}
void UIManager::onDockWidgetActivate(QWidget *pw) {
    // For some reason this gets called twice when each tab is click
    assert(m_pCore);
    auto [model, _v] = getModelViewFromWidget(pw);
    auto [fullpath, _x, _y, _z] = getFullpathFromModel(model);
    log("UIManager::OnDockWidgetActivate: activated %s", fullpath.c_str());
    m_pCore->activateLib(fullpath);
    updateTitle();
}


//////////
// PUBLIC 
//////////
UIManager::UIManager(QObject *parent) :
    QObject(parent)
{
    m_defaultViewTypes.push_back(ViewType::LIBSYMBOLVIEW);
    m_defaultViewTypes.push_back(ViewType::LIBTREEVIEW);
    m_defaultViewTypes.push_back(ViewType::LIBTABLEVIEW);

    // Create entry for default main window
    m_connViews.push_back({"",ViewType::INVALID, nullptr, nullptr, false, nullptr});
}
void UIManager::setParentMW(QMainWindow *p) {
    m_parentMW = p;
    m_connViews.front().mainwindow = p;
}
QMainWindow *UIManager::parentMW() const {
    return m_parentMW;
}
void UIManager::notifyDbOpen(IDbIf *dbif, std::string fullpath) {
    log("Notify open %s", fullpath.c_str());
    for (auto uit : m_defaultViewTypes) {
        openUI(dbif, fullpath, uit);
    }
}
void UIManager::notifyDbClose(IDbIf *dbif, std::string fullpath) {
    // (void) dbif;
    // // Close all widgets that are downstream from fullpath
    // // Suspend event before closing
    // log("Notify close %s", fullpath.c_str());
    // while (!m_connViews[fullpath].empty() &&
    //        !m_connViews[fullpath].begin()->second.empty()) {
    //     auto view(m_connViews[fullpath].begin()->second.front());
    //     auto *cdw(static_cast<ClosingDockWidget *>(view->parentWidget()));
    //     cdw->blockSignals(true);
    //     cdw->close();
    //     removeView(cdw);
    // }
}
void UIManager::notifyDbRename(IDbIf *dbif, std::string oldpath, std::string newpath) {
    (void) dbif;
    log("Notify rename %s to %s", oldpath.c_str(), newpath.c_str());
}
