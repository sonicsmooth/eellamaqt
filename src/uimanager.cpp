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
#include <QPoint>

#include <filesystem>
#include <iterator>
#include <list>
#include <set>
#include <exception>
#include <optional>
#include <functional>


void ConnView::log(ILogger *lgr) {
    std::string vts(viewType == ViewType::INVALID          ? "INVALID"          :
                    viewType == ViewType::LIBSYMBOLVIEW    ? "LibSymbolView"    :
                    viewType == ViewType::LIBTREEVIEW      ? "LibTreeView"      :
                    viewType == ViewType::LIBTABLEVIEW     ? "LibTableView"     :
                    viewType == ViewType::LIBFOOTPRINTVIEW ? "LibFootprintView" : "really invalid");
    std::string title(std::filesystem::path(fullpath).filename().string());
    lgr->log("%-19s  %-13s  0x%08x  0x%08x  0x%08x  0x%08x", 
    title.c_str(), vts.c_str(), model, view, subWidget, mainWindow);
}

void cvlog(ConnViews cvs, ILogger *lgr) {
    lgr->log("");
    lgr->log("%-19s  %-13s  %-10s  %-10s  %-10s  %-10s", 
        "Fullpath","ViewType","Model","View","SubWidget","Mainwindow");
    for (auto cv : cvs)
        cv.log(lgr);
}


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
    // Return first entry where fields match, else nullopt
    for (auto cv : m_connViews)
        if (cv.subWidget == w)
            return std::move(cv);
    return std::nullopt;
}
std::optional<ConnView> UIManager::selectWhere(QMdiSubWindow *w) {
    // Return first entry where fields match, else nullopt
    for (auto cv : m_connViews)
        if (cv.subWidget == w)
            return std::move(cv);
    return std::nullopt;
}
std::optional<ConnView> UIManager::selectWhere(QMainWindow *mw) {
    // Return first entry where fields match, else nullopt
    for (auto cv : m_connViews)
        if (cv.mainWindow == mw)
            return std::move(cv);
    return std::nullopt;
}
std::optional<ConnView> UIManager::selectWhere(std::string conn, ViewType vt) {
    // Return first entry where fields match, else nullopt
    for (auto cv : m_connViews)
        if (cv.fullpath == conn && cv.viewType == vt)
            return std::move(cv);
    return std::nullopt;
}
std::optional<ConnView> UIManager::selectWhere(std::string conn, ViewType vt, QMainWindow *mw) {
    // Return first entry where fields match, else nullopt
    for (auto cv : m_connViews)
        if (cv.fullpath == conn && cv.viewType == vt && cv.mainWindow==mw)
            return std::move(cv);
    return std::nullopt;
}
template <typename F>
std::optional<ConnView> UIManager::selectWhere(F&& fn) {
    // Return first entry where predicate is true, else nullopt
    for (auto cv : m_connViews)
        if (fn(cv))
            return std::move(cv);
    return std::nullopt;
}
ConnViews UIManager::selectWheres(std::string conn) {
    // Return list of matching entries, else empty list
    ConnViews retval;
    for (auto cv: m_connViews)
        if(cv.fullpath == conn)
            retval.push_back(cv);
    return retval;
}
ConnViews UIManager::selectWheres(ViewType vt) {
    // Go through each item in cvs and return list of matching entries
    // List is empty if nothing is found
    ConnViews retval;
    for (auto cv: m_connViews)
        if(cv.viewType == vt)
            retval.push_back(cv);
    return retval;
}
ConnViews UIManager::selectWheres(QMainWindow *mw) {
    // Return list of matching entries, else empty list
    ConnViews retval;
    for (auto cv: m_connViews)
        if(cv.mainWindow == mw)
            retval.push_back(cv);
    return retval;
}
ConnViews UIManager::selectWheres(std::string conn, ViewType vt) {
    // Go through each item in cvs and return list of matching entries
    // List is empty if nothing is found
    ConnViews retval;
    for (auto cv: m_connViews)
        if(cv.fullpath == conn && cv.viewType == vt)
            retval.push_back(cv);
    return retval;
}
// template <typename F>
// ConnViews UIManager::selectWheres(F&& fn) {
//     // Go through each item in cvs and return list where fn(cv) is true
//     // List is empty if nothing is found
//     ConnViews retval;
//     for (auto cv: m_connViews)
//         if(fn(cv))
//             retval.push_back(cv);
//     return retval;
// }

LibWindow *UIManager::activeLibWindow() {
    return static_cast<LibWindow *>(QApplication::activeWindow());
}
QMainWindow *UIManager::activeMainWindow() {
    return static_cast<QMainWindow *>(QApplication::activeWindow());
}
ClosingMDIWidget *UIManager::activeLibWidget() {
    return static_cast<ClosingMDIWidget *>(activeLibWindow()->mdiArea()->activeSubWindow());
}
QMdiSubWindow *UIManager::activeMdiSubWindow() {
    return activeLibWindow()->mdiArea()->activeSubWindow();
}

QAbstractItemModel *UIManager::makeLibSymbolModel(IDbIf *dbif, std::string fullpath) {
    QSqlDatabase db(dynamic_cast<QSQDbIf *>(dbif)->database(fullpath));
    return new QSqlTableModel(this, db);
}
QAbstractItemModel *UIManager::makeLibTreeModel(IDbIf *dbif, std::string fullpath) {
    QSqlDatabase db(dynamic_cast<QSQDbIf *>(dbif)->database(fullpath));
    return new QSqlTreeModel(activeMainWindow(), db);
}
QAbstractItemModel *UIManager::makeLibTableModel(IDbIf *dbif, std::string fullpath) {
    QSqlDatabase db(dynamic_cast<QSQDbIf *>(dbif)->database(fullpath));
    return new QSqlTableModel(this, db);
}
QAbstractItemView *UIManager::makeLibSymbolView(QAbstractItemModel *model) {
    // Make sure we're getting the right type of model
    QSqlTableModel *sqlModel(dynamic_cast<QSqlTableModel *>(model));
    if (!sqlModel)
        throw("Not a QSqlTableModel");
    QAbstractItemView *view(new LibSymbolView(activeMainWindow(), model, m_pCore, m_pLogger/*, fullpath)*/));
    sqlModel->setTable("firsttable");
    sqlModel->setEditStrategy(QSqlTableModel::OnRowChange);
    sqlModel->select();
    return view;
}
QAbstractItemView *UIManager::makeLibTreeView(QAbstractItemModel *model) {
    // Make sure we're getting the right type of model
    QSqlTreeModel *sqlModel(dynamic_cast<QSqlTreeModel *>(model));
    if (!sqlModel)
        throw("Not a QSqlTreeModel");
    QAbstractItemView *view(new LibTreeView(activeMainWindow(), model, m_pCore, m_pLogger/*, fullpath*/));
    // model->setTable("firsttable");
    // model->setEditStrategy(QSqlTableModel::OnRowChange);
    // model->select();
    return view;
}
QAbstractItemView *UIManager::makeLibTableView(QAbstractItemModel *model) {
    // Make sure we're getting the right type of model
    QSqlTableModel *sqlModel(dynamic_cast<QSqlTableModel *>(model));
    if (!sqlModel)
        throw("Not a QSqlTableModel");
    QAbstractItemView *view(new LibTableView(activeMainWindow(), model, m_pCore, m_pLogger/*, fullpath*/));
    sqlModel->setTable("firsttable");
    sqlModel->setEditStrategy(QSqlTableModel::OnRowChange);
    sqlModel->select();
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
    QMainWindow *mw(activeMainWindow());
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
QWidget *UIManager::openUI(IDbIf *dbif, std::string fullpath, ViewType vt) {
    // For LibSymbolView, allows any number of instances in any number of mainwindows
    // For auxilliary views, only one type of view is allowed per mainwindow
    // TODO: Don't automatically maximize -- open based on viewmode
    if (vt == ViewType::LIBSYMBOLVIEW) {
        QAbstractItemModel *model(nullptr);
        QAbstractItemView *view(nullptr);
        LibWindow *mw = activeLibWindow();
        assert(mw);
        auto selectopt(selectWhere(fullpath, vt)); // selects the model in any window
        model = selectopt ? (*selectopt).model : (this->*makeModelfm[vt])(dbif, fullpath);
        view = (this->*makeViewfm[vt])(model);
        assert(view->model() == model);
        std::string title(std::filesystem::path(fullpath).filename().string());
        ClosingMDIWidget *widget(makeMDILibWidget(nullptr, view, title));
        QObject::connect(widget, &ClosingMDIWidget::closing, this, &UIManager::onMdiSubWindowClose);
        mw->mdiArea()->addSubWindow(widget);
        if (mw->mdiArea()->viewMode() == QMdiArea::ViewMode::SubWindowView)
            widget->showMaximized();
        else
            widget->show();
        m_connViews.push_back({fullpath, vt, model, view, widget, mw});
        cvlog(m_connViews, m_pLogger);
        return widget;

    } else {

    }
 }
 void UIManager::onDockWidgetActivate(QWidget *pw) {
    // For some reason this gets called twice when each tab is clicked
    assert(m_pCore);
    auto [model, _v, vt] = getModelViewFromWidget(static_cast<QMdiSubWindow *>(pw));
    auto [fullpath, _x, _y] = getFullpathFromModel(model);
    log("UIManager::OnDockWidgetActivate: activated %s", fullpath.c_str());
    m_pCore->activateLib(fullpath);
}
void UIManager::onDockWidgetClose(QWidget *pw) {
    // Remove the view from list and allow library to be closed
    // This happens when user clicks the close button
    (void) pw;
    LibWindow *lw(activeLibWindow());
    lw->updateLibActions(m_pCore->DbIf()->isDatabaseOpen() &&
                      selectWhere([lw](ConnView cv){return cv.view && cv.mainWindow == lw;}));
}
void UIManager::onMdiSubWindowClose(QWidget *w) {
    // Make sure we're actually getting an MDIWidget
    // aka mainWidget informally, which is different from mainWindow informally

    QMdiSubWindow *mdisw(dynamic_cast<QMdiSubWindow *>(w));
    assert(mdisw);
    auto [model, _v, vt] = getModelViewFromWidget(static_cast<QMdiSubWindow *>(w));
    auto [fullpath, _x, _y] = getFullpathFromModel(model);
    assert(vt == ViewType::LIBSYMBOLVIEW);

    // Count total entries with this conn and vt.
    // If == 0, error
    // if == 1, close all entries with fullpath
    // If > 1, then remove just the one with this conn, vt, and mainwindow
    ConnViews mainViews(selectWheres(fullpath, vt));
    assert(mainViews.size());
    if (mainViews.size() == 1) {
        // nothing else found, so close all (aux) views and close library
        for (auto cv : selectWheres(fullpath)) {
            cv.subWidget->blockSignals(true);
            cv.subWidget->close();
            m_connViews.remove(cv);
            cvlog(m_connViews, m_pLogger);
        }
        delete model; // hopefully no views still point here
        log("UIManager::onMainWidgetClose Closing lib %s", fullpath.c_str());
        m_pCore->closeLibNoGui(fullpath);
    } else { // Other mainviews found, so just delete this entry
        m_connViews.remove(*selectWhere(mdisw));
        cvlog(m_connViews, m_pLogger);
    }

    // We know that mainWindows are represented in m_connViews at least once
    // with INVALID ViewType Loop through mainWindows and if the list for these
    // with not INVALID viewtype is empty, then disable that window's actions
    for (auto mwcv : selectWheres(ViewType::INVALID)) {
        if (!selectWhere([mwcv](ConnView cv){return cv.viewType != ViewType::INVALID &&
                                                    cv.mainWindow == mwcv.mainWindow;}))
            static_cast<LibWindow *>(mwcv.mainWindow)->updateLibActions(false);
    }
}
void UIManager::onMainWindowClose(QWidget *w) {
    // Make sure we're actually getting a LibWindow
    // aka mainWindow informally
    LibWindow *lw(dynamic_cast<LibWindow *>(w));
    // Make sure all 'mainWidget' (aka QMdiSubWindow) entries with this mainWindow in m_connViews are gone,
    // thus indicating that all QMdiSubWindows have been properly closed, the closing of each of which
    // removes their entry from m_connViews;
    auto selectopt(selectWheres(lw));
    assert(selectopt.size() == 1);
    m_connViews.remove(selectopt.front());
    cvlog(m_connViews, m_pLogger);
}


// PUBLIC
UIManager::UIManager(QObject *parent) :
    QObject(parent)
{
    m_defaultViewTypes.push_back(ViewType::LIBSYMBOLVIEW);
    //m_defaultViewTypes.push_back(ViewType::LIBTREEVIEW);
    //m_defaultViewTypes.push_back(ViewType::LIBTABLEVIEW);
}
void UIManager::notifyDbOpen(IDbIf *dbif, std::string fullpath) {
    for (auto uit : m_defaultViewTypes) {
        openUI(dbif, fullpath, uit);
    }
}
void UIManager::notifyDbClose(IDbIf *dbif, std::string fullpath) {
    // Close windows after being notified that library has closed
    ConnViews cvs(selectWheres(fullpath, ViewType::LIBSYMBOLVIEW));
    for (auto cv : cvs) {
        cv.subWidget->close();
    }
    
    
}
void UIManager::notifyDbRename(IDbIf *dbif, std::string oldpath, std::string newpath) {
    (void) dbif;
    log("Notify rename %s to %s", oldpath.c_str(), newpath.c_str());
}
void *UIManager::newWindow()  {
    // Creates new top level window using members from this UIManager instance
    return newWindow(core(), logger());
}
void *UIManager::newWindow(LibCore *core, ILogger *lgr)  {
    // Creates new top level window using given members
    LibWindow *w(new LibWindow());
    w->setCore(core);
    w->setLogger(lgr);
    w->setAttribute(Qt::WA_DeleteOnClose);
    //w->show();

    QObject::connect(w, &LibWindow::closing, this, &UIManager::onMainWindowClose);
    m_connViews.push_back({"", ViewType::INVALID, nullptr, nullptr, nullptr, w});
    cvlog(m_connViews, m_pLogger);
    return w;

}
void *UIManager::duplicateWindow() {
    // Duplicate the active window
    return duplicateWindow(activeLibWindow());
}
void *UIManager::duplicateWindow(void *oldw) {
    // Duplicate the given window
    LibWindow *oldlw(static_cast<LibWindow *>(oldw));
    LibWindow *newlw(static_cast<LibWindow *>(newWindow()));
    QMdiArea *oa(oldlw->mdiArea());
    QMdiArea *na(newlw->mdiArea());
    na->setActivationOrder(oa->activationOrder());
    na->setBackground(oa->background());
    na->setDocumentMode(oa->documentMode());
    na->setTabPosition(oa->tabPosition());
    na->setTabShape(oa->tabShape());
    na->setTabsClosable(oa->tabsClosable());
    na->setTabsMovable(oa->tabsMovable());
    na->setViewMode(oa->viewMode());
    newlw->move(oldlw->pos() + QPoint(50,50));
    newlw->resize(oldlw->size());
    return newlw;
}
void UIManager::closeWindow() {
    // Closes current top level window As this and children are closed, they may
    // also be destroyed if their WA_DeletOnClose property is set. If this is
    // true, then it's possible that the textEdit object gets destroyed, leading
    // to the Logger pointing to a ghost.  Main() should connect the destroyed
    // signal from the textedit to the Logger telling Logger to reset its
    // textedit pointer.
    activeMainWindow()->close();
}
void UIManager::closeWindow(void *window) {
    QWidget *widget(static_cast<QWidget *>(window));
    widget->close();
}

std::list<QMainWindow *> UIManager::mainWindows() {
    std::set<QMainWindow *> set;
    for (auto cv : m_connViews)
        set.insert(cv.mainWindow);
    return {set.begin(), set.end()};

}

void UIManager::duplicateMainView() {
    // Take current MDIWidget, extract the model, then extract the fullpath.
    // Create a new UI with fullpath.  This creates a new view and a new MDIWidget
    assert(m_pCore);
    LibWindow *lw(activeLibWindow());
    QMdiSubWindow *mdiWidget(lw->mdiArea()->activeSubWindow());
    assert(mdiWidget);
    auto [model, view, vt] = getModelViewFromWidget(mdiWidget);
    auto [fullpath, _x, _y] = getFullpathFromModel(model);
    QWidget * newWidget(openUI(m_pCore->DbIf(), fullpath, ViewType::LIBSYMBOLVIEW));
    QMdiSubWindow *newMdiWidget(static_cast<QMdiSubWindow *>(newWidget));
    if (mdiWidget->isMaximized())
        newMdiWidget->showMaximized();

}
void UIManager::popOutMainView() {
    // Move current symbol or main QMdiSubWindow into new window
    // This is a precursor to tear-away mdi subwindow
    // Keep maximized or non-maximized state in src and dst windows

    QMdiSubWindow *mdiSubWindow(activeMdiSubWindow());
    assert(mdiSubWindow);
    bool ismax(mdiSubWindow->isMaximized());
    auto selectopt(selectWhere(mdiSubWindow));
    assert(selectopt);
    ConnView cv(*selectopt);
    m_connViews.remove(cv);
    
    LibWindow *oldlw(activeLibWindow());
    LibWindow *newlw(static_cast<LibWindow *>(duplicateWindow()));
    newlw->show();
    oldlw->mdiArea()->removeSubWindow(mdiSubWindow);

    // It seems that when you remove a subwindow from mdiArea, the remaining windows
    // become unmaximized, so here we fix that if they had been previously maximized
    if (ismax) {
        QList<QMdiSubWindow *> lst(oldlw->mdiArea()->subWindowList());
        if (!lst.isEmpty())
            lst.first()->showMaximized();
        mdiSubWindow->showMaximized();
    }

    // Add the new one in the same maximized state as it was before popping out
    newlw->mdiArea()->addSubWindow(mdiSubWindow);
    if (ismax)
        mdiSubWindow->showMaximized();
    else
        mdiSubWindow->showNormal();
    newlw->updateLibActions(true);
    m_connViews.push_back({cv.fullpath, cv.viewType, cv.model, cv.view, mdiSubWindow, newlw});
    cvlog(m_connViews, m_pLogger);
}
void UIManager::closeMainView() {
    // Closes current main QMdiSubWindow
    // TODO: Make sure proper mdisubwindow is activated to close
    QMdiSubWindow *mdiSubWindow(activeMdiSubWindow());
    assert(mdiSubWindow);
    auto selectopt(selectWhere(mdiSubWindow));
    assert(selectopt);
    mdiSubWindow->close();
    m_connViews.remove(*selectopt);
}
