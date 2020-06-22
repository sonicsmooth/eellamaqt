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

// Standalone functions
template <typename W>
W activeWindow();
QMdiSubWindow *activeMdiSubWindow();
template <typename T_WIDGET>
auto modelViewFromWidget(T_WIDGET *pw);
auto fullpathFromModel(QAbstractItemModel *model);
std::string fullpathFromMdiSubWindow(QMdiSubWindow *);
std::string fullpathFromActiveMdiSubWindow();

//////////
// LOCAL
//////////

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
template <typename W>
W activeWindow() { // could return null
    W w(static_cast<W>(QApplication::activeWindow()));
    return w;
}
QMdiSubWindow *activeMdiSubWindow() {
    auto adw(activeWindow<DocWindow *>());
    auto mdia(adw->mdiArea());
    assert(mdia);
    return mdia->activeSubWindow();
}
template <typename T_WIDGET>
auto modelViewFromWidget(T_WIDGET *containerWidget) {
    // Attempt to cast model to different things to determine vt
    QAbstractItemView *view(dynamic_cast<QAbstractItemView *>(containerWidget->widget()));
    assert(view);
    QAbstractItemModel *model(view->model());
    assert(model);
    ViewType vt(dynamic_cast<LibSymbolView *>(view) ? ViewType::LIBSYMBOLVIEW :
                dynamic_cast<LibTreeView *>(view) ? ViewType::LIBTREEVIEW :
                dynamic_cast<LibTableView *>(view) ? ViewType::LIBTABLEVIEW :
                dynamic_cast<QAbstractItemView *>(view) ? ViewType::INVALID :
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
// TODO: Templatize this the same way modelViewFromWidget is templatized
// TODO: That is, make fullpathfromSubWindow take at least a QMDISubWindow
// TODO: or a QDockWidget equally
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
//std::optional<ConnView> UIManager::selectWhere(const QWidget *w) {
//    for (auto cv : m_connViews)
//        if (cv.subWidget == w)
//            return std::move(cv);
//    return std::nullopt;
//}
//std::optional<ConnView> UIManager::selectWhere(const QMainWindow *mw) {
//    for (auto cv : m_connViews)
//        if (cv.mainWindow == mw)
//            return std::move(cv);
//    return std::nullopt;
//}
//std::optional<ConnView> UIManager::selectWhere(ViewType vt) {
//    for (auto cv : m_connViews)
//        if (cv.viewType == vt)
//            return std::move(cv);
//    return std::nullopt;
//}
//std::optional<ConnView> UIManager::selectWhere(std::string conn, ViewType vt) {
//    for (auto cv : m_connViews)
//        if (cv.fullpath == conn && cv.viewType == vt)
//            return std::move(cv);
//    return std::nullopt;
//}
//std::optional<ConnView> UIManager::selectWhere(std::string conn, ViewType vt, const QMainWindow *mw) {
//    for (auto cv : m_connViews)
//        if (cv.fullpath == conn && cv.viewType == vt && cv.mainWindow==mw)
//            return std::move(cv);
//    return std::nullopt;
//}
std::optional<ConnView> UIManager::selectWhere(const std::function<bool (const ConnView &)> & fn) {
    // Return first entry where predicate is true, else nullopt
    for (auto cv : m_connViews)
        if (fn(cv))
            return std::move(cv);
    return std::nullopt;
}

// This is the generic base case, must be specialized
// By keeping this commented, we can force a linker error
// if the fn is called with an arg for which there is no
// specialization
//template<typename Arg>
//ConnViews UIManager::selectWhere_ext(ConnViews&& cvs, Arg arg) {
//    (void) cvs;
//    (void) arg;
//    log("Single selectWhere_ext, shouldn't be called directly");
//    throw("Wrong function call");
//}
template<> // specialization
ConnViews UIManager::selectWhere_ext(ConnViews&& cvs, std::string arg) {
    ConnViews retval;
    for (auto cv: cvs)
        if(cv.fullpath == arg) retval.push_back(cv);
    return retval;
}
template<> // specialization
ConnViews UIManager::selectWhere_ext(ConnViews&& cvs, ViewType arg) {
    ConnViews retval;
    for (auto cv: cvs)
        if(cv.viewType == arg) retval.push_back(cv);
    return retval;
}
template<> // specialization
ConnViews UIManager::selectWhere_ext(ConnViews&& cvs, const QAbstractItemModel *arg) {
    ConnViews retval;
    for (auto cv: cvs)
        if(cv.model == arg) retval.push_back(cv);
    return retval;
}
template<> // specialization
ConnViews UIManager::selectWhere_ext(ConnViews&& cvs, const QAbstractItemView *arg) {
    ConnViews retval;
    for (auto cv: cvs)
        if(cv.view == arg) retval.push_back(cv);
    return retval;
}
template<> // specialization
ConnViews UIManager::selectWhere_ext(ConnViews&& cvs, const QWidget *arg) {
    ConnViews retval;
    for (auto cv: cvs)
        if(cv.subWidget == arg) retval.push_back(cv);
    return retval;
}
template<> // specialization
ConnViews UIManager::selectWhere_ext(ConnViews&& cvs, const QMainWindow *arg) {
    ConnViews retval;
    for (auto cv: cvs)
        if(cv.mainWindow == arg) retval.push_back(cv);
    return retval;
}

// This is the internal recursive case
template <typename Arg, typename... Args>
ConnViews UIManager::selectWhere_ext(ConnViews&& cvs, Arg firstarg, Args... restargs) {
    ConnViews cvs2(selectWhere_ext(std::move(cvs), firstarg));
    if (cvs2.size())
        return selectWhere_ext(std::move(cvs2), restargs...);
    else
        return cvs2;
}

// This case gets called from the client code for single searches
template <typename... Args>
std::optional<ConnView> UIManager::selectWhere_ext(Args... allargs) {
    // Returns first item found
    // Does NOT assert that exactly one item was found
    ConnViews cvs(selectWhere_ext(std::move(m_connViews), allargs...));
    if (cvs.size())
        return std::move(cvs.front());
    else
        return std::nullopt;
}

// This case gets called from the client code for list searches
template <typename... Args>
ConnViews UIManager::selectWheres_ext(Args... allargs) {
    return selectWhere_ext(std::move(m_connViews), allargs...);
}

ConnViews UIManager::selectWheres(const std::function<bool (const ConnView &)> & fn) {
    ConnViews retval;
    for (auto cv: m_connViews)
        if(fn(cv))
            retval.push_back(cv);
    return retval;
}
QAbstractItemModel *UIManager::makeLibSymbolModel(IDbIf *dbif, std::string fullpath) {
    QSqlDatabase db(dynamic_cast<QSQDbIf *>(dbif)->database(fullpath));
    return new QSqlTableModel(this, db);
}
QAbstractItemModel *UIManager::makeLibTreeModel(IDbIf *dbif, std::string fullpath) {
   QSqlDatabase db(dynamic_cast<QSQDbIf *>(dbif)->database(fullpath));
   return new QSqlTreeModel(this, db);
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
    QAbstractItemView *view(new LibSymbolView(activeWindow<QMainWindow *>(), model, m_pCore, m_pLogger));
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
   QAbstractItemView *view(new LibTreeView(activeWindow<QMainWindow *>(), model, m_pCore, m_pLogger));
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
   QAbstractItemView *view(new LibTableView(activeWindow<QMainWindow *>(), model, m_pCore, m_pLogger));
   sqlModel->setTable("firsttable");
   sqlModel->setEditStrategy(QSqlTableModel::OnRowChange);
   sqlModel->select();
   return view;
}
QWidget *UIManager::makeCDWLibWidget(QWidget *parent, QWidget *contents, std::string title) {
    contents->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding));
    ClosingDockWidget* widget(new ClosingDockWidget(parent));
    contents->setFocusProxy(widget); // doesn't seem to do anything
    widget->setFocusPolicy(Qt::StrongFocus); // accepts focus by tabbing or clicking
    widget->setWidget(contents);
    widget->setWindowTitle(QString::fromStdString(title));
    widget->setAttribute(Qt::WA_DeleteOnClose);
    return widget;
}
QWidget *UIManager::makeMDILibWidget(QWidget *parent, QWidget *contents, std::string title) {
   contents->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding));
   ClosingMDIWidget* widget(new ClosingMDIWidget(parent));
   widget->setWidget(contents);
   widget->setWindowTitle(QString::fromStdString(title));
   widget->setAttribute(Qt::WA_DeleteOnClose);
   QObject::connect(widget, &ClosingMDIWidget::closing, this, &UIManager::onMdiSubWindowClose);
   return widget;
}

void UIManager::attachMDISubWindow(QMainWindow *mw, QWidget *widget) {
    assert(mw);
    assert(widget);
    DocWindow *dw(static_cast<DocWindow *>(mw));
    QList<QMdiSubWindow *> swl(dw->mdiArea()->subWindowList());
    dw->mdiArea()->addSubWindow(widget);
    if (dw->mdiArea()->viewMode() == QMdiArea::ViewMode::SubWindowView &&
        ((!swl.empty() && swl.first()->isMaximized()) || swl.empty()))
        widget->showMaximized();
    else
        widget->show();

    // Before adding this UI, find another top level window for below hack
    auto selectopt (selectWhere([mw](ConnView cv) {
        return cv.viewType == ViewType::INVALID && cv.mainWindow != mw;
    }));

    // Hack to activate current main window
    // Select another window first, then select this one
    if (selectopt)
        m_hackTimer.singleShot(0, [=]{
            selectopt->mainWindow->activateWindow();
            dw->activateWindow();});
}
void UIManager::attachDockWidget(QMainWindow *mw, QWidget *widget) {
    assert(mw);
    ClosingDockWidget *cdw(dynamic_cast<ClosingDockWidget *>(widget));
    assert(cdw);

    // Find entry for new widget; there should be only one
    auto newselect(selectWhere_ext(static_cast<const QWidget *>(widget)));
    std::string newpath(newselect->fullpath);
    ViewType newvt(newselect->viewType);

    // Find entries matching this type but not this fullpath
    ConnViews oldselects(selectWheres([&newpath, newvt](ConnView cv)
        {return cv.fullpath != newpath && cv.viewType == newvt;}));

    // Remove any previous dockwidget matching this type but not this fullpath
    for (ConnView oldselect : oldselects) {
        log("Detaching %s at 0x%08x", oldselect.fullpath.c_str(), oldselect.subWidget);
        mw->removeDockWidget(static_cast<QDockWidget *>(oldselect.subWidget));
        QObject::disconnect(oldselect.subWidget, nullptr, nullptr, nullptr);
    }
    log("Attaching %s 0x%08x == 0x%08x", newpath.c_str(), newselect->subWidget, cdw);
    mw->addDockWidget(dockWidgetAreaMap[newvt], cdw);
    cdw->show();
    QObject::connect(cdw, &ClosingDockWidget::closing, this, &UIManager::onDockWidgetClose);
}


// TODO: Add ability to open some type of UI other than model/view, eg text window, browser, etc.
QWidget *UIManager::openUI(IDbIf *dbif, std::string fullpath, ViewType vt) {
    // For LibSymbolView, allows any number of instances in any number of mainwindows
    // For auxilliary views, only one type of view is allowed per mainwindow

    // Check whether this viewtype is a subwidget type with this path and
    // already exists in this mainwindow.  If true, then skip creation.
    QMainWindow *mw(activeWindow<QMainWindow *>());
    const QMainWindow *cmw(static_cast<const QMainWindow *>(mw));
    auto selectopt(selectWhere_ext(vt, cmw, fullpath));
    bool condition((vt == ViewType::LIBLISTVIEW  || 
                    vt == ViewType::LIBTABLEVIEW ||
                    vt == ViewType::LIBTREEVIEW) && 
                    selectopt);
    QWidget *widget(nullptr);
    if (condition) {
        widget = selectopt->subWidget;
    } else {
        // Create or find appropriate model and view
        QAbstractItemModel *model(nullptr);
        QAbstractItemView *view(nullptr);
        auto selectopt2(selectWhere_ext(fullpath, vt)); // selects first entry with fullpath and viewtype
        model = selectopt2 ? selectopt2->model : (this->*makeModelfm[vt])(dbif, fullpath);
        view = (this->*makeViewfm[vt])(model);

        // Make new widget, store, and attach
        std::string title(std::filesystem::path(fullpath).filename().string());
        widget = (this->*makeWidgetfm[vt])(nullptr, view, title);
        m_connViews.push_back({fullpath, vt, model, view, widget, mw});
        cvlog(m_connViews, m_pLogger);
    }

    (this->*attachWidgetfm[vt])(mw, widget);
    updateLibActions();
    return widget;
 }
void UIManager::onDocWindowActivate(QWidget *w) {
    // Tell core which database to make active based on whatever subwindow is active
    assert(m_pCore);
    log("DocWindow Activated: 0x%08x == 0x%08x", w, activeWindow<DocWindow *>());
    DocWindow *lw(static_cast<DocWindow *>(w));
    QMdiSubWindow *sw(lw->mdiArea()->activeSubWindow());
    if(sw) {
        auto [model, _v, vt] = modelViewFromWidget(sw);
        auto [fullpath, _x, _y] = fullpathFromModel(model);
        m_pCore->DbIf()->activateDatabase(fullpath);
    }
}
void UIManager::onDocWindowClose(QWidget *w) {
    const QMainWindow *mw(static_cast<const QMainWindow *>(w));
    // Make sure this DocWindow is the only entry, thus indicating that all
    // QMdiSubWindows and all subwidgets have been properly closed, the closing
    // of each of which should have happened already automatically, and which
    // removes their entry from m_connViews.  The assertion can fail if for
    // example there are subviews in a mainwindow with no associated main (or
    // doc) view, which happens when there is a bug.
    ConnViews cvs(selectWheres_ext(mw));
    assert(cvs.size() == 1);
    m_connViews.remove(cvs.front());
    cvlog(m_connViews, m_pLogger);
}
void UIManager::onMdiSubWindowActivate(QWidget *w){
    // Remove existing subwidgets, and reattached subwidgets associated with this model
    // but only if that type of subwidget already exists.  On other words, replace
    // the tree and model views with the current one.  To be clear, there can be
    // multiple dockwidgets of the same type, but only one of any type can be attached
    // at a time.

    // QMdiSubWindow can be activated with no QMainWindow active.
    const QMainWindow *cmw(activeWindow<const QMainWindow *>());
    if (!w || !cmw) // this can happen legitimately
        return;

    // Find entry for this QMdiSubWindow, empty on init
    auto selectopt(selectWhere_ext(static_cast<const QWidget *>(w)));
    if (!selectopt)
        return;

    // Find which subwidgets are already showing in this window, then determine their view type
    QList<const QDockWidget *> dwch(cmw->findChildren<const QDockWidget *>());
    std::list<ViewType> vts_to_check({ViewType::LIBLISTVIEW, ViewType::LIBTABLEVIEW, ViewType::LIBTREEVIEW});
    std::list<ViewType> vtl;
    for (ViewType vt : vts_to_check) {
        for (auto ch : dwch) {
            const QWidget *widget(static_cast<const QWidget *>(ch));
            auto selectopt2(selectWhere_ext(vt, widget, cmw));
            if (selectopt2 && ch->isVisible())
                vtl.push_back(vt);
        }
    }
    vtl.sort();
    vtl.unique();

    // For each type of subwidget already showing, open a UI using this fullpath
    std::string fullpath(selectopt->fullpath);
    for (ViewType vt : vtl) {
        openUI(m_pCore->DbIf(), fullpath, vt);
    }

    // Activate library associated with this mdisubwindow
    assert(m_pCore);
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
    ConnViews mainViews(selectWheres_ext(fullpath, vt));
    assert(mainViews.size());
    if (mainViews.size() == 1) {  // nothing else found, so close all (aux) views and close library
        for (auto cv : selectWheres_ext(fullpath)) {
            cv.subWidget->blockSignals(true); // Not necessarily what we want
            cv.subWidget->close();
            m_connViews.remove(cv);
        }
        delete model; // hopefully no views still point here
        log("UIManager::onMainWidgetClose Closing lib %s", fullpath.c_str());
        m_pCore->closeLibNoGui(fullpath);
    } else { // Other mainviews found, so just delete this entry
        m_connViews.remove(*selectWhere_ext(static_cast<const QWidget *>(w)));
    }
    cvlog(m_connViews, m_pLogger);
    updateLibActions();

}
void UIManager::onDockWidgetActivate(QWidget *w) {
    // For some reason this gets called twice when each tab is clicked
    assert(m_pCore);
    assert(w);
    auto [model, _v, vt] = modelViewFromWidget(static_cast<const QDockWidget *>(w));
    auto [fullpath, _x, _y] = fullpathFromModel(model);
    log("UIManager::OnDockWidgetActivate: activated %s", fullpath.c_str());
    m_pCore->activateLib(fullpath);
}
void UIManager::onDockWidgetClose(QWidget *w) {
    // Remove the view from list and allow library to be closed
    // This happens when user clicks the close button
    assert(w);
    log("OnDockWidgetClose 0x%08x", w);
    const QWidget *cw(static_cast<const QWidget *>(w));
    const QMainWindow *cmw(activeWindow<const QMainWindow *>());
    auto selectopt(selectWhere_ext(cw, cmw));
    if (selectopt) { // sometimes this gets called twice, so the second time it's empty.
        m_connViews.remove(*selectopt);
        cvlog(m_connViews, m_pLogger);
    }
    if (DocWindow *dw = activeWindow<DocWindow *>())
        dw->updateViewEnables();
}
void UIManager::updateLibActions() {
    // Find mainwindows without any other associated views
    for (auto mwcv : selectWheres_ext(ViewType::INVALID)) {
        DocWindow *dw(static_cast<DocWindow *>(mwcv.mainWindow));
        const QMainWindow *mw(mwcv.mainWindow);
        dw->updateLibActions(selectWheres_ext(mw).size() > 1);
    }
}





// PUBLIC
UIManager::UIManager(QObject *parent) :
    QObject(parent)
{
    m_defaultViewTypes.push_back(ViewType::LIBSYMBOLVIEW);
    m_defaultViewTypes.push_back(ViewType::LIBTREEVIEW);
    m_defaultViewTypes.push_back(ViewType::LIBTABLEVIEW);
}
void UIManager::notifyDbOpen(IDbIf *dbif, std::string fullpath) {
    for (auto uit : m_defaultViewTypes) {
        openUI(dbif, fullpath, uit);
    }
}
void UIManager::notifyDbClose(IDbIf *dbif, std::string fullpath) {
    // Close QMdiSubWindows after being notified that library has closed
    (void) dbif;
    ConnViews cvs(selectWheres_ext(fullpath, ViewType::LIBSYMBOLVIEW));
    for (auto cv : cvs) {
        cv.subWidget->close();
    }
}
void UIManager::notifyDbRename(IDbIf *dbif, std::string oldpath, std::string newpath) {
    (void) dbif;
    //log("Notify rename %s to %s", oldpath.c_str(), newpath.c_str());
    // Get the new database from dbif based on newpath
    // Get all entries associated with oldpath from m_connViews
    // Among entries, delete model and create new model to the new database
    // For each entry, change m_connViews to show newpath
    // For each entry, rename QMdiSubWidget title to newpath
    QSqlDatabase newdb(static_cast<QSQDbIf *>(dbif)->database(newpath));
    ConnViews dbcvs(selectWheres_ext(oldpath));
    std::list<QAbstractItemModel *> models;
    for (auto cv : dbcvs)
        models.push_back(cv.model);
    models.sort();
    models.unique();
    for (auto model : models)
        delete model;
    for (auto cv: dbcvs) {
        m_connViews.remove(cv);
        cv.fullpath = newpath;
        cv.model = (this->*makeModelfm[cv.viewType])(dbif, newpath);
        cv.view->setModel(cv.model);
        std::string title(std::filesystem::path(newpath).filename().string());
        cv.subWidget->setWindowTitle(QString::fromStdString(title));
        m_connViews.push_back(cv);
        cvlog(m_connViews, m_pLogger);
    }
}
void *UIManager::newWindow()  {
    // Creates new top level window using members from this UIManager instance
    return newWindow(core(), logger());
}
void *UIManager::newWindow(LibCore *pcore, ILogger *plgr)  {
    // Creates new top level window using given members
    DocWindow *w(new DocWindow(nullptr, pcore, plgr));
    w->setAttribute(Qt::WA_DeleteOnClose);

    QObject::connect(w, &DocWindow::activated, this, &UIManager::onDocWindowActivate);
    QObject::connect(w, &DocWindow::closing, this, &UIManager::onDocWindowClose);
    QObject::connect(w->mdiArea(), &QMdiArea::subWindowActivated, this, &UIManager::onMdiSubWindowActivate);
    QObject::connect(w, &QMainWindow::tabifiedDockWidgetActivated, this, &UIManager::onDockWidgetActivate);

    m_connViews.push_back({"", ViewType::INVALID, nullptr, nullptr, nullptr, w});
    cvlog(m_connViews, m_pLogger);
    return w;

}
void *UIManager::duplicateWindow() {
    // Duplicate the active window
    return duplicateWindow(activeWindow<DocWindow *>());
}
void *UIManager::duplicateWindow(void *oldw) {
    // Duplicate the given window without children
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
    // Closes current top level window. As this and children are closed, they may
    // also be destroyed if their WA_DeleteOnClose property is set. If this is
    // true, then it's possible that the textEdit object gets destroyed, leading
    // to the Logger pointing to a ghost.  Main() should connect the destroyed
    // signal from the textedit to the Logger telling Logger to reset its
    // textedit pointer.
    activeWindow<QMainWindow *>()->close();
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
    DocWindow *lw(activeWindow<DocWindow *>());
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
    // For subwidgets, create as needed in new doc window; delete as needed in
    // old doc window.

    // Find existing mdisubwindow, record its state, and remove from list
    QMdiSubWindow *mdiSubWindow(activeMdiSubWindow());
    assert(mdiSubWindow);
    bool ismax(mdiSubWindow->isMaximized());
    auto selectopt(selectWhere_ext(static_cast<const QWidget *>(mdiSubWindow)));
    assert(selectopt);
    ConnView cv(*selectopt);
    m_connViews.remove(cv);

    // Detach subwindows with this fullpath and record which ones were present
    DocWindow *olddw(activeWindow<DocWindow *>());
    ConnViews cvs;
    std::list<ViewType> vts_to_check({ViewType::LIBLISTVIEW, ViewType::LIBTREEVIEW, ViewType::LIBTREEVIEW});
    for (ViewType vt : vts_to_check) {
        selectopt = selectWhere_ext(cv.fullpath, vt);
        if (selectopt) {
            cvs.push_back(*selectopt);
            olddw->removeDockWidget(static_cast<QDockWidget *>(selectopt->subWidget));
        }
    }

    // Duplicate the main window
    DocWindow *newdw(static_cast<DocWindow *>(duplicateWindow()));
    newdw->show();
    olddw->mdiArea()->removeSubWindow(mdiSubWindow);

    // It seems that when you remove a subwindow from mdiArea, the remaining mdi
    // subwindows become unmaximized, so here we fix that if they had been
    // previously maximized
    if (ismax) {
        QList<QMdiSubWindow *> lst(olddw->mdiArea()->subWindowList());
        if (!lst.isEmpty())
            lst.first()->showMaximized();
    }

    // Add the new mdi subwindow in the same maximized state as it was before popping out
    newdw->mdiArea()->addSubWindow(mdiSubWindow);
    ismax ? mdiSubWindow->showMaximized() : mdiSubWindow->showNormal();
    m_connViews.push_back({cv.fullpath, cv.viewType, cv.model, cv.view, mdiSubWindow, newdw});
    cvlog(m_connViews, m_pLogger);

    // Add subwindows using pre-existing connview, updated with new main window
    for (ConnView cv2 : cvs) {
        cv2.mainWindow = newdw;
        (this->*attachWidgetfm[cv2.viewType])(newdw, cv2.subWidget);
        m_connViews.push_back(cv2);
    }
    updateLibActions();
}
void UIManager::closeMainView() {
    // Closes current main QMdiSubWindow
    QMdiSubWindow *sw(activeMdiSubWindow());
    assert(sw);
    auto selectopt(selectWhere_ext(static_cast<const QWidget *>(sw)));
    assert(selectopt);
    sw->close();
    m_connViews.remove(*selectopt);
    cvlog(m_connViews, m_pLogger);
}
bool UIManager::viewTypeExists(ViewType vt, const DocWindow *dw) {
    // This is called when someone wants to know whether a viewtype is showing
    // If >0 exists, then exactly one is showing.  If none is showing, then
    // none exist.
    const QMainWindow *mw(static_cast<const QMainWindow *>(dw));
    ConnViews cvs(selectWheres_ext(vt, mw));
    return cvs.size() > 0;
}
void UIManager::enableSubView(ViewType vt) {
    log("UIManager::enableSubView");
    // Fail if any view of this type is already associated with the active window
    const QMainWindow *mw(activeWindow<const QMainWindow *>());
    assert(!(selectWhere_ext(vt, mw)));
    // Find fullpath assocated with current mdi view
    const QWidget *sw(activeMdiSubWindow());
    auto selectopt(selectWhere_ext(sw));
    assert(selectopt);
    assert(m_pCore);
    openUI(m_pCore->DbIf(), selectopt->fullpath, vt);



}
