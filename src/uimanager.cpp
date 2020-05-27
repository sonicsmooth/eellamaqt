#include "uimanager.h"
#include "closingdockwidget.h"
#include "closingmdiwidget.h"
#include "docwindow.h"
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
#include <QTextEdit>
#include <QTimer>

#include <filesystem>
#include <iterator>
#include <list>
#include <set>
#include <exception>
#include <optional>
#include <functional>
// #include <thread>
// #include <chrono>

// Standalone functions
DocWindow *activeLibWindow();
QMainWindow *activeMainWindow();
QMdiSubWindow *activeMdiSubWindow();
template <typename T_WIDGET>
auto modelViewFromWidget(T_WIDGET *pw);
auto fullpathFromModel(QAbstractItemModel *model);
std::string fullpathFromMdiSubWindow(QMdiSubWindow *);
std::string fullpathFromActiveMdiSubWindow();


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
// TODO: returning final null is okay if the final fn call is legit
DocWindow *activeLibWindow() {
    DocWindow *lw(static_cast<DocWindow *>(QApplication::activeWindow()));
    assert(lw);
    return lw;
}
QMainWindow *activeMainWindow() {
    QMainWindow *mw(static_cast<QMainWindow *>(QApplication::activeWindow()));
    assert(mw);
    return mw;
}
QMdiSubWindow *activeMdiSubWindow() {
    auto alw(activeLibWindow());
    auto mdia(alw->mdiArea());
    assert(mdia);
    return mdia->activeSubWindow();
}
template <typename T_WIDGET>
auto modelViewFromWidget(T_WIDGET *pw) {
    // Attempt to cast model to different things to determine vt
    T_WIDGET *qdw(static_cast<T_WIDGET *>(pw)); // TODO: necessary?
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
auto fullpathFromModel(QAbstractItemModel *model) {
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
std::string fullpathFromMdiSubWindow(QMdiSubWindow *w){
    assert(w);
    auto [model, _v, vt] = modelViewFromWidget(w);
    auto [fullpath, _x, _y] = fullpathFromModel(model);
    return fullpath;
}
std::string fullpathFromActiveMdiSubWindow() {
    return fullpathFromMdiSubWindow(activeMdiSubWindow());
}

//////////
// PRIVATE
//////////
// Return first entry where fields match, else nullopt
std::optional<ConnView> UIManager::selectWhere(QDockWidget *w) {
    for (auto cv : m_connViews)
        if (cv.subWidget == w)
            return std::move(cv);
    return std::nullopt;
}
std::optional<ConnView> UIManager::selectWhere(QMdiSubWindow *w) {
    for (auto cv : m_connViews)
        if (cv.subWidget == w)
            return std::move(cv);
    return std::nullopt;
}
std::optional<ConnView> UIManager::selectWhere(QMainWindow *mw) {
    for (auto cv : m_connViews)
        if (cv.mainWindow == mw)
            return std::move(cv);
    return std::nullopt;
}
std::optional<ConnView> UIManager::selectWhere(std::string conn, ViewType vt) {
    for (auto cv : m_connViews)
        if (cv.fullpath == conn && cv.viewType == vt)
            return std::move(cv);
    return std::nullopt;
}
std::optional<ConnView> UIManager::selectWhere(std::string conn, ViewType vt, QMainWindow *mw) {
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
// Return list of matching entries, else empty list
ConnViews UIManager::selectWheres(std::string conn) {
    ConnViews retval;
    for (auto cv: m_connViews)
        if(cv.fullpath == conn)
            retval.push_back(cv);
    return retval;
}
ConnViews UIManager::selectWheres(ViewType vt) {
    ConnViews retval;
    for (auto cv: m_connViews)
        if(cv.viewType == vt)
            retval.push_back(cv);
    return retval;
}
ConnViews UIManager::selectWheres(QMainWindow *mw) {
    ConnViews retval;
    for (auto cv: m_connViews)
        if(cv.mainWindow == mw)
            retval.push_back(cv);
    return retval;
}
ConnViews UIManager::selectWheres(std::string conn, ViewType vt) {
    ConnViews retval;
    for (auto cv: m_connViews)
        if(cv.fullpath == conn && cv.viewType == vt)
            retval.push_back(cv);
    return retval;
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
    assert(model);
    QSqlTableModel *sqlModel(dynamic_cast<QSqlTableModel *>(model));
    if (!sqlModel)
        throw("Not a QSqlTableModel");
    QAbstractItemView *view(new LibSymbolView(activeMainWindow(), model, m_pCore, m_pLogger));
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
   QAbstractItemView *view(new LibTreeView(activeMainWindow(), model, m_pCore, m_pLogger));
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
   QAbstractItemView *view(new LibTableView(activeMainWindow(), model, m_pCore, m_pLogger));
   sqlModel->setTable("firsttable");
   sqlModel->setEditStrategy(QSqlTableModel::OnRowChange);
   sqlModel->select();
   return view;
}
//ClosingDockWidget *UIManager::makeCDWLibWidget(QWidget *parent, QWidget *contents, std::string title) {
//    view->setMaximumWidth(1000);
//    view->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding));
//    ClosingDockWidget* libDockWidget(new ClosingDockWidget(parent));
//    view->setFocusProxy(libDockWidget); // doesn't seem to do anything
//    libDockWidget->setFocusPolicy(Qt::StrongFocus); // accepts focus by tabbing or clicking
//    libDockWidget->setWidget(contents);
//    libDockWidget->setWindowTitle(QString::fromStdString(title));
//    return libDockWidget;
//}
ClosingMDIWidget *UIManager::makeMDILibWidget(QWidget *parent, QWidget *contents, std::string title) {
   contents->setMaximumWidth(1000);
   contents->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding));
   ClosingMDIWidget* widget(new ClosingMDIWidget(parent));
   widget->setWidget(contents);
   widget->setWindowTitle(QString::fromStdString(title));
   widget->setAttribute(Qt::WA_DeleteOnClose);
   return widget;
}
//void UIManager::dockLibView(ClosingDockWidget *libDockWidget, Qt::DockWidgetArea area) {
//    // Stick argument to right or left (or wherever), tabifying as necessary
//    QMainWindow *mw(activeMainWindow());
//    mw->addDockWidget(area, libDockWidget, Qt::Orientation::Horizontal);

//    // Make this dockwidget stick to right and tabify if needed
//    // Get all ClosingDockWidgets and ignore those that are either floating or not in the
//    // right dock area, then tabify on top of the first one remaining
//    QList<ClosingDockWidget *> cdws0(mw->findChildren<ClosingDockWidget *>());
//    QList<ClosingDockWidget *> cdws; // working copy
//    for (auto const & cdw : cdws0) {
//        if (mw->dockWidgetArea(cdw) == area && !cdw->isFloating()) {
//            cdws.push_back(cdw);
//        }
//    }

//    // Add to existing right ClosingDockWidget and raise
//    if (cdws.length() > 1) {
//        mw->tabifyDockWidget(cdws.first(), libDockWidget);
//        mw->blockSignals(true);
//        libDockWidget->setVisible(true);
//        libDockWidget->setFocus();
//        libDockWidget->raise();
//        mw->blockSignals(false);
//    }

//    // Resize to fixed width; start with needed lists
//    QList<QDockWidget *> qdws; // a base-classier cast-down version of ClosingDockWidget
//    QList<int> qdww; // the widths for QDockWidget
//    for (auto const & cdw : cdws) {
//        qdww.push_back(100);
//        qdws.push_back(static_cast<QDockWidget *>(cdw));
//    }
//    mw->resizeDocks(qdws, qdww, Qt::Orientation::Horizontal);

//    // Send close signal somewhere. When all dockwindows with the same
//    // string are closed, UIManager calls core to close library.
//    // For some reason these connect calls are active *before* the raise() call above,
//    // so when raise is called, the onDockWidgetActive gets called.  The callback for this
//    // refers to m_openLibTreeWidgets which has not yet been updated with the libDockWidget, so
//    // the assert fails.  Therefore the parentMW signals are blocked above so this
//    // callback doesn't get called.`
//    QObject::connect(mw, &QMainWindow::tabifiedDockWidgetActivated, this, &UIManager::onDockWidgetActivate);
//    QObject::connect(libDockWidget, &ClosingDockWidget::closing, this, &UIManager::onDockWidgetClose);
//}
// TODO: Add ability to open some type of UI other than model/view, eg text window, browser, etc.
QWidget *UIManager::openUI(IDbIf *dbif, std::string fullpath, ViewType vt) {
    // For LibSymbolView, allows any number of instances in any number of mainwindows
    // For auxilliary views, only one type of view is allowed per mainwindow
    if (vt == ViewType::LIBSYMBOLVIEW) {
        // Create or find appropriate model and view
        QAbstractItemModel *model(nullptr);
        QAbstractItemView *view(nullptr);
        DocWindow *mw = activeLibWindow();
        assert(mw);
        auto selectopt(selectWhere(fullpath, vt)); // selects the model in any window
        model = selectopt ? selectopt->model : (this->*makeModelfm[vt])(dbif, fullpath);
        view = (this->*makeViewfm[vt])(model);
        assert(view->model() == model);
        
        // Create QMdiSubWindow and show properly
        std::string title(std::filesystem::path(fullpath).filename().string());
        ClosingMDIWidget *widget(makeMDILibWidget(nullptr, view, title));
        QList<QMdiSubWindow *> swl(mw->mdiArea()->subWindowList());
        mw->mdiArea()->addSubWindow(widget);
       if (mw->mdiArea()->viewMode() == QMdiArea::ViewMode::SubWindowView &&
           ((!swl.empty() && swl.first()->isMaximized()) || swl.empty()))
           widget->showMaximized();
       else
           widget->show();
        widget->setFocus();

        // Before adding this UI, find another top level window for below hack
        selectopt = selectWhere([mw](ConnView cv) {
            return cv.viewType == ViewType::INVALID && cv.mainWindow != mw;
        });

        // Store new widget
        m_connViews.push_back({fullpath, vt, model, view, widget, mw});
        cvlog(m_connViews, m_pLogger);
        QObject::connect(widget, &ClosingMDIWidget::closing, this, &UIManager::onMdiSubWindowClose);
        updateLibActions();

        // Hack to activate current main window
        // Select another window first, then select this one
        m_hackTimer.singleShot(0, [=]{
            if (selectopt)
                selectopt->mainWindow->activateWindow();
            mw->activateWindow();});

        return widget;
    } else {
        return nullptr;
    }
 }
//void UIManager::onDockWidgetActivate(QWidget *pw) {
//    // For some reason this gets called twice when each tab is clicked
//    assert(m_pCore);
//    auto [model, _v, vt] = modelViewFromWidget(static_cast<QMdiSubWindow *>(pw));
//    auto [fullpath, _x, _y] = fullpathFromModel(model);
//    log("UIManager::OnDockWidgetActivate: activated %s", fullpath.c_str());
//    m_pCore->activateLib(fullpath);
//}
//void UIManager::onDockWidgetClose(QWidget *w) {
//    // Remove the view from list and allow library to be closed
//    // This happens when user clicks the close button
//    (void) w;
//    LibWindow *lw(activeLibWindow());
//    // TODO: Make UIManager ignorant of DbIf.
//    // TODO: Call should just be m_pCore->isDatabaseOpen()
//    lw->updateLibActions(m_pCore->DbIf()->isDatabaseOpen() &&
//                      selectWhere([lw](ConnView cv){return cv.view && cv.mainWindow == lw;}));
//}
void UIManager::onMdiSubWindowActivate(QWidget *w){
    assert(m_pCore);
    // Qt documentation specifically says the value could be 0. Doesn't say the
    // value could be nullptr.  Of course these are the same, but we're trying
    // to be correct here.  This appears no to be a problem with MSVC, but Qt
    // Creator appears to use Clang as the realtime compiler in the background
    // which issues these warning in the IDE.
    QMainWindow *mw(static_cast<QMainWindow *>(QObject::sender()->parent()->parent()));
    if (w == nullptr ) {
        // log("all QMdiSubWindows deactivated from 0x%08x", mw);
        // log("");
        return;
    }
    log("onMdiSubWindowActivate QMdiSubWindow activated 0x%08x -> 0x%08x", w, mw);
//    log("");
    QMdiSubWindow *sw(static_cast<QMdiSubWindow *>(w));
    std::string fullpath(fullpathFromMdiSubWindow(sw));
    log("Fullpath: %s", fullpath.c_str());
    m_pCore->activateLib(fullpath);
}
void UIManager::onMdiSubWindowClose(QWidget *w) {
    // Make sure we're actually getting an MDIWidget
    // aka mainWidget informally, which is different from mainWindow informally

    QMdiSubWindow *mdisw(dynamic_cast<QMdiSubWindow *>(w));
    assert(mdisw);
    auto [model, _v, vt] = modelViewFromWidget(static_cast<QMdiSubWindow *>(w));
    auto [fullpath, _x, _y] = fullpathFromModel(model);
    assert(vt == ViewType::LIBSYMBOLVIEW);

    // Count total entries with this conn and vt.
    // case 0: error; 1: close all entries with fullpath, 
    // else: remove just the one with this conn, vt, and mainwindow
    ConnViews mainViews(selectWheres(fullpath, vt));
    assert(mainViews.size());
    if (mainViews.size() == 1) {  // nothing else found, so close all (aux) views and close library
        for (auto cv : selectWheres(fullpath)) {
            cv.subWidget->blockSignals(true);
            cv.subWidget->close();
            m_connViews.remove(cv);
        }
        delete model; // hopefully no views still point here
        log("UIManager::onMainWidgetClose Closing lib %s", fullpath.c_str());
        m_pCore->closeLibNoGui(fullpath);
    } else { // Other mainviews found, so just delete this entry
        m_connViews.remove(*selectWhere(mdisw));
    }
    cvlog(m_connViews, m_pLogger);
    updateLibActions();

}
void UIManager::onLibWindowActivate(QWidget *w) {
    // Tell core which database to make active based on whatever subwindow is active
    assert(m_pCore);
    log("LibWindow Activated: 0x%08x", w);
    DocWindow *lw(static_cast<DocWindow *>(w));
    QMdiSubWindow *sw(lw->mdiArea()->activeSubWindow());
    if(sw) {
        auto [model, _v, vt] = modelViewFromWidget(sw);
        auto [fullpath, _x, _y] = fullpathFromModel(model);
        m_pCore->DbIf()->activateDatabase(fullpath);
    }
}
void UIManager::onLibWindowClose(QWidget *w) {
    // Make sure we're actually getting a LibWindow
    // aka mainWindow informally
    DocWindow *lw(dynamic_cast<DocWindow *>(w));
    // Make sure all 'mainWidget' (aka QMdiSubWindow) entries with this mainWindow in m_connViews are gone,
    // thus indicating that all QMdiSubWindows have been properly closed, the closing of each of which
    // removes their entry from m_connViews;
    auto selectopt(selectWheres(lw));
    assert(selectopt.size() == 1);
    m_connViews.remove(selectopt.front());
    cvlog(m_connViews, m_pLogger);
}
void UIManager::updateLibActions() {
    // Find mainwindows without any other associated views
    for (auto mwcv : selectWheres(ViewType::INVALID)) {
        DocWindow *lw(static_cast<DocWindow *>(mwcv.mainWindow));
        lw->updateLibActions(selectWheres(lw).size() > 1);
    }
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
    // Close QMdiSubWindows after being notified that library has closed
    (void) dbif;
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
void *UIManager::newWindow(LibCore *pcore, ILogger *plgr)  {
    // Creates new top level window using given members
    DocWindow *w(new DocWindow(nullptr, pcore, plgr));
    w->setAttribute(Qt::WA_DeleteOnClose);

    QObject::connect(w, &DocWindow::activated, this, &UIManager::onLibWindowActivate);
    QObject::connect(w, &DocWindow::closing, this, &UIManager::onLibWindowClose);
    QObject::connect(w->mdiArea(), &QMdiArea::subWindowActivated, this, &UIManager::onMdiSubWindowActivate);
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
    DocWindow *oldlw(static_cast<DocWindow *>(oldw));
    DocWindow *newlw(static_cast<DocWindow *>(newWindow()));
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
    DocWindow *lw(activeLibWindow());
    QMdiSubWindow *mdiWidget(lw->mdiArea()->activeSubWindow());
    assert(mdiWidget);
    auto [model, view, vt] = modelViewFromWidget(mdiWidget);
    auto [fullpath, _x, _y] = fullpathFromModel(model);
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
    
    DocWindow *oldlw(activeLibWindow());
    DocWindow *newlw(static_cast<DocWindow *>(duplicateWindow()));
    newlw->show();
    oldlw->mdiArea()->removeSubWindow(mdiSubWindow);

    // It seems that when you remove a subwindow from mdiArea, the remaining windows
    // become unmaximized, so here we fix that if they had been previously maximized
    if (ismax) {
        QList<QMdiSubWindow *> lst(oldlw->mdiArea()->subWindowList());
        if (!lst.isEmpty())
            lst.first()->showMaximized();
        //mdiSubWindow->showMaximized();
    }

    // Add the new one in the same maximized state as it was before popping out
    newlw->mdiArea()->addSubWindow(mdiSubWindow);
    if (ismax)
        mdiSubWindow->showMaximized();
    else
        mdiSubWindow->showNormal();
    m_connViews.push_back({cv.fullpath, cv.viewType, cv.model, cv.view, mdiSubWindow, newlw});
    cvlog(m_connViews, m_pLogger);

    updateLibActions();
}
void UIManager::closeMainView() {
    // Closes current main QMdiSubWindow
    QMdiSubWindow *sw(activeMdiSubWindow());
    assert(sw);
    auto selectopt(selectWhere(sw));
    assert(selectopt);
    sw->close();
    m_connViews.remove(*selectopt);
    cvlog(m_connViews, m_pLogger);
}
