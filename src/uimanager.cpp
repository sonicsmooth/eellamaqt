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
#include <QItemSelectionModel>
#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QItemSelection>

#include <filesystem>
#include <iterator>
#include <list>
#include <set>
#include <exception>
#include <optional>
#include <functional>


//////////
// LOCAL
//////////
template <typename T_WIDGET>
auto getModelViewFromWidget(T_WIDGET *pw) {
    T_WIDGET *qdw(static_cast<T_WIDGET *>(pw));
    QAbstractItemView *view(static_cast<QAbstractItemView *>(qdw->widget()));
    QAbstractItemModel *model(view->model());
    ViewType vt(dynamic_cast<LibSymbolView *>(view) ? ViewType::LIBSYMBOLVIEW :
                dynamic_cast<LibTreeView *>(view) ? ViewType::LIBTREEVIEW :
                dynamic_cast<LibTableView *>(view) ? ViewType::LIBTABLEVIEW :
                throw("Must be symbolview, treeview, or tableview"));
    struct retVals {
        QAbstractItemModel *model;
        QAbstractItemView *view;
        ViewType vt;
    };
    return retVals {model, view, vt};
}
auto getFullpathFromModel(QAbstractItemModel *model) {
    // Attempt to cast model to different things to determine vt
    QSqlTreeModel *treeModel(dynamic_cast<QSqlTreeModel *>(model));
    QSqlTableModel *tableModel(dynamic_cast<QSqlTableModel *>(model));
    std::string fullpath(treeModel  ? treeModel->database().connectionName().toStdString():
                         tableModel ? tableModel->database().connectionName().toStdString() :
                         throw("Neither treeModel nor tableModel"));
    struct retVals {
        std::string fullpath;
        QSqlTreeModel *treeModel;
        QSqlTableModel *tableModel;
    };
    return retVals {fullpath, treeModel, tableModel};
}


//////////
// PRIVATE
//////////
std::optional<ConnView> UIManager::selectWhere(QDockWidget *w) {
    // Return first entry where widget matches, else nullopt
    for (auto cv : m_connViews) {
        if (cv.widget == w)
            return std::move(cv);
    }
    return std::nullopt;
}
std::optional<ConnView> UIManager::selectWhere(QMdiSubWindow *w) {
    // Return first entry where widget matches, else nullopt
    for (auto cv : m_connViews) {
        if (cv.widget == w)
            return std::move(cv);
    }
    return std::nullopt;
}
std::optional<ConnView> UIManager::selectWhere(QMainWindow *mw) {
    // Returns first item in m_connViews where
    // mainWindow matches
    // Returns std::optional::nullopt when nothing found
    for (auto cv : m_connViews) {
        if (cv.mainwindow == mw)
            return std::move(cv);
    }
    return std::nullopt;
}
std::optional<ConnView> UIManager::selectWhere(std::string conn, ViewType vt) {
    // Returns first item in m_connViews where
    // conn and viewtype match.
    // Returns std::optional::nullopt when nothing found
    for (auto cv : m_connViews) {
        if (cv.conn == conn && cv.vt == vt)
            return std::move(cv);
    }
    return std::nullopt;
}
std::optional<ConnView> UIManager::selectWhere(std::string conn, ViewType vt, QMainWindow *mw) {
    // Returns first item in m_connViews where
    // conn, viewtype, and mainwindow match.
    // Returns std::optional::nullopt when nothing found
    for (auto cv : m_connViews) {
        if (cv.conn == conn && cv.vt == vt && cv.mainwindow==mw)
            return std::move(cv);
    }
    return std::nullopt;
}
template<typename F>
std::optional<ConnView> UIManager::selectWhere(F&& fn) {
    // Returns first item in m_connViews where
    // pred(item) returns true
    for (auto cv : m_connViews) {
        if (fn(cv))
            return std::move(cv);
    }
    return std::nullopt;
}
ConnViews UIManager::selectWheres(std::string conn) {
    // Return list of matching entries, else empty list
    ConnViews retval;
    for (auto cv: m_connViews) {
        if(cv.conn == conn)
            retval.push_back(cv);
    }
    return retval;
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


QAbstractItemModel *UIManager::makeLibSymbolModel(IDbIf *dbif, std::string fullpath) {
    QSqlDatabase db(dynamic_cast<QSQDbIf *>(dbif)->database(fullpath));
    return new QSqlTableModel(this, db);
}
QAbstractItemModel *UIManager::makeLibTreeModel(IDbIf *dbif, std::string fullpath) {
    QSqlDatabase db(dynamic_cast<QSQDbIf *>(dbif)->database(fullpath));
    return new QSqlTreeModel(QApplication::activeWindow(), db);
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
    QAbstractItemView *view(new LibSymbolView(QApplication::activeWindow(), model, m_pCore, m_pLogger/*, fullpath)*/));
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
    QAbstractItemView *view(new LibTreeView(QApplication::activeWindow(), model, m_pCore, m_pLogger/*, fullpath*/));
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
    QAbstractItemView *view(new LibTableView(QApplication::activeWindow(), model, m_pCore, m_pLogger/*, fullpath*/));
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
    QMainWindow *mw(static_cast<QMainWindow *>(QApplication::activeWindow()));
    mw->addDockWidget(area, libDockWidget, Qt::Orientation::Horizontal);

    // Make this dockwidget stick to right and tabify if needed
    // Get all ClosingDockWidgets and ignore those that are either floating or not in the
    // right dock area, then tabify on top of the first one remaining
    QList<ClosingDockWidget *> cdws0(mw->findChildren<ClosingDockWidget *>());
    QList<ClosingDockWidget *> cdws; // working copy
    for (auto const & cdw : cdws0) {
        if (mw->dockWidgetArea(cdw) == area && !cdw->isFloating()) {
            cdws.push_back(cdw);
        }
    }

    // Add to existing right ClosingDockWidget and raise
    if (cdws.length() > 1) {
        mw->tabifyDockWidget(cdws.first(), libDockWidget);
        mw->blockSignals(true);
        libDockWidget->setVisible(true);
        libDockWidget->setFocus();
        libDockWidget->raise();
        mw->blockSignals(false);
    }

    // Resize to fixed width; start with needed lists
    QList<QDockWidget *> qdws; // a base-classier cast-down version of ClosingDockWidget
    QList<int> qdww; // the widths for QDockWidget
    for (auto const & cdw : cdws) {
        qdww.push_back(100);
        qdws.push_back(static_cast<QDockWidget *>(cdw));
    }
    mw->resizeDocks(qdws, qdww, Qt::Orientation::Horizontal);

    // Send close signal somewhere. When all dockwindows with the same
    // string are closed, UIManager calls core to close library.
    // For some reason these connect calls are active *before* the raise() call above,
    // so when raise is called, the onDockWidgetActive gets called.  The callback for this
    // refers to m_openLibTreeWidgets which has not yet been updated with the libDockWidget, so
    // the assert fails.  Therefore the parentMW signals are blocked above so this
    // callback doesn't get called.`
    QObject::connect(mw, &QMainWindow::tabifiedDockWidgetActivated, this, &UIManager::onDockWidgetActivate);
    QObject::connect(libDockWidget, &ClosingDockWidget::closing, this, &UIManager::onDockWidgetClose);
}
void UIManager::openUI(IDbIf *dbif, std::string fullpath, ViewType vt) {
    // For LibSymbolView, allows any number of instances in any number of mainwindows
    // For auxilliary views, only one type of view is allowed per mainwindow

    if (vt == ViewType::LIBSYMBOLVIEW) {
        QAbstractItemModel *model(nullptr);
        QAbstractItemView *view(nullptr);
        LibWindow *mw = static_cast<LibWindow *>(QApplication::activeWindow());
        assert(mw);
        auto selectopt(selectWhere(fullpath, vt)); // selects the model in any window
        model = selectopt ? (*selectopt).model : (this->*makeModelfm[vt])(dbif, fullpath);
        view = (this->*makeViewfm[vt])(model);
        assert(view->model() == model);
        std::string title(std::filesystem::path(fullpath).filename().string());
        ClosingMDIWidget *widget(makeMDILibWidget(nullptr, view, title));
        QObject::connect(widget, &ClosingMDIWidget::closing, this, &UIManager::onMainWidgetClose);
        mw->mdiArea()->addSubWindow(widget);
        widget->show();
        m_connViews.push_back({fullpath, vt, model, view, true, widget, mw});

    } else {

    }
 }
void UIManager::onDockWidgetClose(QWidget *pw) {
    // Remove the view from list and allow library to be closed
    // This happens when user clicks the close button
    //removeView(pw);
    LibWindow *lw(dynamic_cast<LibWindow *>(QApplication::activeWindow()));
    lw->updateActions(m_pCore->DbIf()->isDatabaseOpen() && 
                      selectWhere([lw](ConnView cv) -> bool {return cv.view && cv.mainwindow == lw;}));
}
void UIManager::onMainWidgetClose(QWidget *pw) {
    log("OnMainWidgetClose");
    auto [model, _v, vt] = getModelViewFromWidget(static_cast<QMdiSubWindow *>(pw));
    auto [fullpath, _x, _y] = getFullpathFromModel(model);
    assert(vt == ViewType::LIBSYMBOLVIEW);
    QMainWindow *mw(static_cast<QMainWindow *>(QApplication::activeWindow()));

    // Count total entries with this conn and vt.
    // If == 0, error
    // if == 1, close all entries with fullpath
    // If > 1, then remove just the one with this conn, vt, and mainwindow
    ConnViews mainViews(selectWheres(fullpath, vt));
    if (mainViews.size() == 0)
        throw("Error in mainViews count");
    else if (mainViews.size() == 1) {
        // nothing else found, so close all (aux) views and close library
        for (auto cv : selectWheres(fullpath)) {
            cv.widget->blockSignals(true);
            cv.widget->close();
            m_connViews.remove(cv);
        }
        delete model; // hopefully no views still point here
        log("UIManager::onMainWidgetClose Closing lib %s", fullpath.c_str());
        m_pCore->closeLibNoGui(fullpath);
    } else {
        // Other mainviews found, so just delete this entry
        // May still need to shuffle windows
        m_connViews.remove(*selectWhere(fullpath, vt, mw));
    }
    LibWindow *lw(static_cast<LibWindow *>(mw));
    lw->mdiArea()->activateNextSubWindow();
    lw->updateActions(m_pCore->DbIf()->isDatabaseOpen() && 
                      selectWhere([lw](ConnView cv) -> bool {return cv.view && cv.mainwindow == lw;}));

}
void UIManager::onDockWidgetActivate(QWidget *pw) {
    // For some reason this gets called twice when each tab is click
    assert(m_pCore);
    auto [model, _v, vt] = getModelViewFromWidget(static_cast<QMdiSubWindow *>(pw));
    auto [fullpath, _x, _y] = getFullpathFromModel(model);
    log("UIManager::OnDockWidgetActivate: activated %s", fullpath.c_str());
    m_pCore->activateLib(fullpath);
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
}
// void UIManager::setParentMW(QMainWindow *p) {
//     m_parentMW = p;
//     m_connViews.front().mainwindow = p;
// }
// QMainWindow *UIManager::parentMW() const {
//     return m_parentMW;
// }
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

void UIManager::newWindow()  {
    newWindow(core(), logger());
}; // Creates new top level window

void UIManager::newWindow(LibCore *core, ILogger *lgr)  {
    LibWindow *w(new LibWindow());
    w->setCore(core);
    w->setLogger(lgr);
    w->setAttribute(Qt::WA_DeleteOnClose);
    m_connViews.push_back({"", ViewType::INVALID, nullptr, nullptr, false, nullptr, w});
}; // Creates new top level window

void UIManager::closeWindow() {
    // Closes current top level window
    // Not sure what happens to log window and textedit pointers
    // when first mainwindow is closed.  They appear to stick around,
    // but don't cause a seg fault when invoked by remaining windows.
    // TODO: send signal from either logger or text edit to clients
    // telling them to clear pointer.
    QApplication::activeWindow()->close();
};

std::list<QMainWindow *> UIManager::mainWindows() {
    std::set<QMainWindow *> set;
    for (auto cv : m_connViews)
        set.insert(cv.mainwindow);
    return {set.begin(), set.end()};

}

void UIManager::duplicateSymbolView() {
    // Create another widget for Lib Symbol view without
    // creating a new model or main window
    assert(m_pCore);
    LibWindow *lw(static_cast<LibWindow *>(QApplication::activeWindow()));
    QMdiSubWindow *mdiWidget(lw->mdiArea()->activeSubWindow());
    assert(mdiWidget);
    auto [model, view, vt] = getModelViewFromWidget(mdiWidget);
    auto [fullpath, _x, _y] = getFullpathFromModel(model);
    openUI(m_pCore->DbIf(), fullpath, ViewType::LIBSYMBOLVIEW);
}
