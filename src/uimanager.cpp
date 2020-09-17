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
#include "connviews.h"

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
#include <list>
#include <exception>
#include <stdexcept>
#include <optional>
#include <functional>
#include <algorithm>
#include <vector>
#include <set>



/*
Manage open models, views, widgets, and windows m_connViews manages open and
viewable windows, and all models and views Exactly one instance of each sub view
type allowed per window. Many instances of main view types allowed per window.
The problem is with the subviews aka dockwidgets and one per view type. When
user has one document open and closes the document, then everything about that
document should close and be deleted.  If other documents are currently using a
subview then the associated widget should not be deleted.  If a subwidget is
closed, then do the associated views and models of the current document get
deleted?  Or the views and models of other documents that also use that subview?
My inclination is to keep the models and subviews around, even if they are not
visible, because they may be expensive to create.  However this leads to the
case where the models and views may hang around forever even if they're not
visible, if the user decides not to open that type of subview again.  It also
makes things harder when dealing with the m_connViews table because the table
then no longer maps directly to what's being shown on screen.  If we keep the
table to exactly what's being shown on screen, then we can delete models and
views when not showing.  But then that seems dumb if you have a subview and
you're just switching back and forth between documents.  So clearly we should
keep some things around.  Do we keep each ClosingDockWidget and the one view
that is associated with it, attaching to main window as needed?  Or do we keep
exactly one ClosingDockWidget per ViewType and attach the view to the
ClosingDockWidget as needed?  Currently we are doing the latter.  Probably no
more exensive, and certainly simpler, to maintain the 1-1 relationship between
views and ClosingDockWidgets.  So then it should be as follows: When a
ClosingDockWidget is closed in a window, all views, models, and other
ClosingDockWidgets of that ViewType and window are also closed/deleted and
removed from m_connViews.  When a document requests a new subview, then a new
subview is opened only for that document.  If the user tabs to an existing
document, then a new view and ClosingDockWidget are created.  If the user tabs
back to the previous document then the view and dockwidget are shown. So the
solution is a hybrid. Create views and ClosingDockWidgets as needed and keep
them open until the ClosingDockWidget is closed, or the document is closed.
Thus if something is showing then it is definitely in the table, but it's
possible there are things in the table that aren't showing.
*/ /*
OpenUI:
if main view requested
    create and select given view
else if subview requested
    if this requested (fullpath, viewtype, window) exists and is currently attached
        shouldn't happen -- throw exception
    if any of this viewtype and window exists in table
        hide it
    if requested already exists in table
        select existing
    else
        create and select given view
attach selection

OnMDISubWindowActivate:
For each vt in unique list of all existing viewtypes for this window
    OpenUI(fullpath, vt, window) -- will use existing if available, else new

OnMDISubWindowClose:
f this subwidget closes, then when next mdi is activated, it'll
see theres's nothing left, so it won't attach its subwidgets.  So the trick is to
select the next one first

If last MDI with this fullpath and window
    Close and delete all entries with this fullpath in this window
    If this fullpath isn't assocated with another window
        close library
else (more MDIs available )
    Close and delete only this MDISubWindow's entry -- leave other subviews alone

OnClosingDockWidgetClose:
Close/delete all entries associated with this viewtype and window

NotifyCloseDb:
Close/delete all MDISubWindows associated with this fullpath, in all windows

EnableSubView:
OpenUI(fullpath, vt, window)

Popout:
Duplicate window
OpenUI in new window that already exists in old window




*/



// Standalone function declarations
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

QAbstractItemModel *UIManager::makeLibSymbolModel(const std::string & fullpath) {
    QSqlDatabase db(dynamic_cast<QSQDbIf *>(m_pCore->DbIf())->database(fullpath));
    return new QSqlTableModel(this, db);
}
QAbstractItemModel *UIManager::makeLibTreeModel(const std::string & fullpath) {
   QSqlDatabase db(dynamic_cast<QSQDbIf *>(m_pCore->DbIf())->database(fullpath));
   return new QSqlTreeModel(this, db);
}
QAbstractItemModel *UIManager::makeLibTableModel(const std::string & fullpath) {
   QSqlDatabase db(dynamic_cast<QSQDbIf *>(m_pCore->DbIf())->database(fullpath));
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
QWidget *UIManager::makeCDWLibWidget(QWidget *parent, QWidget *contents, const std::string & title) {
    contents->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding));
    ClosingDockWidget* widget(new ClosingDockWidget(parent));
    contents->setFocusProxy(widget); // doesn't seem to do anything
    widget->setFocusPolicy(Qt::StrongFocus); // accepts focus by tabbing or clicking
    widget->setWidget(contents);
    widget->setWindowTitle(QString::fromStdString(title));
    QObject::connect(widget, &ClosingDockWidget::closing, this, &UIManager::onDockWidgetClose);
    widget->setAttribute(Qt::WA_DeleteOnClose);
    return widget;
}
QWidget *UIManager::makeMDILibWidget(QWidget *parent, QWidget *contents, const std::string & title) {
   contents->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding));
   ClosingMDIWidget* widget(new ClosingMDIWidget(parent));
   widget->setWidget(contents);
   widget->setWindowTitle(QString::fromStdString(title));
   widget->setAttribute(Qt::WA_DeleteOnClose);
   QObject::connect(widget, &ClosingMDIWidget::closing, this, &UIManager::onMdiSubWindowClose);
   return widget;
}
QWidget *UIManager::makeModelViewWidget(const std::string & fullpath, ViewType vt, QMainWindow *parent) {
    // Blindly creates or resuses model, creates view, creates and returns widget
    // widget returned is either ClosingMDILibWidget or ClosingDockWidget
    QAbstractItemModel *model(nullptr);
    auto selectopt2(m_connViews.selectWhere(fullpath, vt)); // selects first entry with fullpath and viewtype
    model = selectopt2 ? selectopt2->model : (this->*makeModelfm[vt])(fullpath);
    QAbstractItemView *view ((this->*makeViewfm[vt])(model));

    // Make new widget...
    std::string title(std::filesystem::path(fullpath).filename().string());
    QWidget *widget((this->*makeWidgetfm[vt])(parent, view, title));
    m_connViews.push_back({fullpath, vt, model, view, widget, parent});
    m_connViews.log(m_pLogger);
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
    // Don't know why I can't auto this predfn, or why I can't specify the template
    // when calling selectWhere
    CVPredFn cvpfn([mw](ConnView cv) {
        return cv.viewType == ViewType::INVALID && cv.mainWindow != mw;});
    auto selectopt (m_connViews.selectWhere(cvpfn));

    // Hack to activate current main window
    // Select another window first, then select this one
    if (selectopt)
        m_hackTimer.singleShot(0, [=]{
            selectopt->mainWindow->activateWindow();
            dw->activateWindow();});
}
void UIManager::attachDockWidget(QMainWindow *mw, QWidget *widget) {
    // The widget that we're trying to attach must already be in the connViews table
    assert(mw);
    ClosingDockWidget *cdw(dynamic_cast<ClosingDockWidget *>(widget));
    assert(cdw);

    // Find entry for new widget; there should be only one
    auto newselect(m_connViews.selectWhere(static_cast<const QWidget *>(widget)));
    assert(newselect);
    std::string fullpath(newselect->fullpath);
    ViewType vt(newselect->viewType);

    // Find entries matching this type but not this fullpath
    CVPredFn cvpfn([&fullpath, vt](ConnView cv)
        {return cv.fullpath != fullpath && cv.viewType == vt;});
    ConnViews oldselects(m_connViews.selectWheres(cvpfn));

    // Remove any previous dockwidget matching this type but not this fullpath
    for (ConnView oldselect : oldselects) {
        log("Detaching %s at 0x%08x", oldselect.fullpath.c_str(), oldselect.subWidget);
        mw->removeDockWidget(static_cast<QDockWidget *>(oldselect.subWidget));
        QObject::disconnect(oldselect.subWidget, nullptr, nullptr, nullptr);
    }

    log("Attaching %s 0x%08x == 0x%08x", fullpath.c_str(), newselect->subWidget, cdw);
    mw->addDockWidget(dockWidgetAreaMap[vt], cdw);
    cdw->show();
}


// TODO: Add ability to open some type of UI other than model/view, eg text window, browser, etc.
QWidget *UIManager::openUI(const std::string & fullpath, ViewType vt, QMainWindow *mw) {
    // For main view types, allows any number of instances in any number of mainwindows
    // For sub view types, only one type of view is allowed per mainwindow

    mw = mw ? mw : activeWindow<QMainWindow *>();
    const QMainWindow *cmw(static_cast<const QMainWindow *>(mw));
    QWidget *widget(nullptr);

    if (findViewType(MainViewTypes, vt)) {
        widget = makeModelViewWidget(fullpath, vt, mw);
    } else if (findViewType(SubViewTypes, vt)) {
        // Check if this view exists
        auto existingEntry(m_connViews.selectWhere(fullpath, vt, cmw));

        // Return if already showing (this happens all the time)
        if (existingEntry && isChildWidgetShowing(cmw,  existingEntry->subWidget)) {
            return nullptr;
        }

        // Detach existing same viewtypes for this window
        for (ConnView cv : m_connViews.selectWheres(vt, cmw)) {
            if (cv.subWidget->isVisible()) {
                mw->removeDockWidget(static_cast<QDockWidget *>(cv.subWidget));
            }
        }
        // select already existing or create new
        widget = existingEntry ? existingEntry->subWidget : makeModelViewWidget(fullpath, vt, mw);
    }
    else {
        throw std::logic_error("Shouldn't get here");
    }
    // ... attach selection
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
        auto selectopt(m_connViews.selectWhere(static_cast<const QWidget *>(sw)));
        //auto [model, _v, vt] = modelViewFromWidget(sw);
        //auto [fullpath, _x, _y] = fullpathFromModel(model);
        //m_pCore->DbIf()->activateDatabase(fullpath);
        m_pCore->DbIf()->activateDatabase(selectopt->fullpath);
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
    ConnViews cvs(m_connViews.selectWheres(mw));
    assert(cvs.size() == 1);
    m_connViews.remove(cvs.front());
    m_connViews.log(m_pLogger);
}
void UIManager::onMdiSubWindowActivate(QWidget *w){
    // Remove existing subwidgets, and reattach subwidgets associated with this model
    // but only if that type of subwidget already exists.  On other words, replace
    // the tree and model views with the current one.  To be clear, there can be
    // multiple dockwidgets of the same type, but only one of any type can be attached
    // at a time.

    // QMdiSubWindow can be activated with no QMainWindow active.
    const QMainWindow *cmw(activeWindow<const QMainWindow *>());
    if (!w || !cmw) // this can happen legitimately
        return;

    // Find entry for this QMdiSubWindow, which is empty on init
    auto selectopt(m_connViews.selectWhere(static_cast<const QWidget *>(w)));
    if (!selectopt)
        return;

    // For each type of subwidget already showing, open a UI using this fullpath
    std::string fullpath(selectopt->fullpath);
    for (ViewType vt : subViewTypesShowing(cmw))
        openUI(fullpath, vt);

    // Activate library associated with this mdisubwindow
    assert(m_pCore);
    m_pCore->activateLib(fullpath);
}
void UIManager::onMdiSubWindowClose(QWidget *w) {
    // Try to close this MDI cleanly.
    // If this is the last MDI in this window, then close everything
    // associated with it, leaving any subviews open for the next MDI
    // If this is the last MDI of this fullpath in any window, close lib.
    // If this is not the last MDI of this window, then just close and
    // delete all things associated with this entry
    QMainWindow *mw(activeWindow<QMainWindow *>());
    DocWindow *dw(static_cast<DocWindow *>(mw));
    const QMainWindow *cmw(static_cast<QMainWindow *>(mw));
    auto thisopt(m_connViews.selectWhere(static_cast<const QWidget *>(w), cmw));
    std::string fullpath(thisopt->fullpath);
    ViewType vt(thisopt->viewType);
    assert(findViewType(MainViewTypes, vt));

    // If we don't activate the next one first, then other subviews get closed
    // Do this == because it's possible to close an mdi window without activating
    // it first, ie when in tabbed mode you click on the close button without
    // selecting it first.
    if (w == activeMdiSubWindow())
        dw->mdiArea()->activateNextSubWindow();

    // Check how many other clone MDIs there are in this window
    ConnViews sameMdiViews(m_connViews.selectWheres(fullpath, vt, cmw));
    assert(sameMdiViews.size());
    if (sameMdiViews.size() == 1) {  // nothing else found, so close all related subwidgets and close library
        for (auto cv : m_connViews.selectWheres(fullpath, cmw)) {
            if (cv.subWidget != w) { // don't recur closing this qmdisubwindow...
                cv.subWidget->blockSignals(true); // ... not that we would recur
                cv.subWidget->close(); // deletes widget on close, which also deletes QAbstractItemView
            }
            delete cv.model;
            m_connViews.remove(cv);
        }
        // Check again, but don't specify this particular window.
        // This will pick up fullpath views in other windows
        if (!m_connViews.selectWhere(fullpath)) {
            log("UIManager::onMainWidgetClose Closing lib %s", fullpath.c_str());
            m_pCore->closeLibNoGui(fullpath);
        }
    } else { // Other mainviews found, so just delete this entry
        // TODO: verify that we don't need to delete any models, etc.
        m_connViews.remove(*m_connViews.selectWhere(static_cast<const QWidget *>(w)));
    }

    m_connViews.log(m_pLogger);
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
    // Close/delete all entries associated with this viewtype and window
    log("OnDockWidgetClose 0x%08x", w);
    const QWidget *cw(static_cast<const QWidget *>(w));
    const QMainWindow *cmw(activeWindow<const QMainWindow *>());
    std::optional<ConnView> selectopt(m_connViews.selectWhere(cw, cmw));
    assert(selectopt);
    ViewType vt(selectopt->viewType);
    for (ConnView cv : m_connViews.selectWheres(vt, cmw)) {
        if (cv.subWidget != w) {
            cv.subWidget->blockSignals(true);
            cv.subWidget->close();
        }
        // Make sure this is the last model
        if (m_connViews.selectWheres(static_cast<const QAbstractItemModel *>(cv.model)).size() == 1)
            delete cv.model;
        m_connViews.remove(cv);
    }
    m_connViews.log(m_pLogger);
    activeWindow<DocWindow *>()->updateViewEnables();
}
void UIManager::updateLibActions() {
    // Find mainwindows without any other associated views
    for (auto mwcv : m_connViews.selectWheres(ViewType::INVALID)) {
        DocWindow *dw(static_cast<DocWindow *>(mwcv.mainWindow));
        const QMainWindow *mw(mwcv.mainWindow);
        dw->updateLibActions(m_connViews.selectWheres(mw).size() > 1);
    }
}
std::list<ViewType> UIManager::subViewTypesShowing(const QMainWindow *cmw) {
    QList<const QDockWidget *> dwch(cmw->findChildren<const QDockWidget *>());
    std::list<ViewType> vtl;
    for (ViewType vt : SubViewTypes) {
        for (auto ch : dwch) {
            const QWidget *widget(static_cast<const QWidget *>(ch));
            auto selectopt2(m_connViews.selectWhere(vt, widget, cmw));
            if (selectopt2 && ch->isVisible())
                vtl.push_back(vt);
        }
    }
    vtl.sort();
    vtl.unique();
    return vtl;
}
bool UIManager::isChildWidgetShowing(const QMainWindow *cmw, const QWidget *w) {
    QList<const QWidget *> wch(cmw->findChildren<const QWidget *>());
    for (auto ch : wch)
        if (ch == w && ch->isVisible())
            return true;
    return false;
}




// PUBLIC
UIManager::UIManager(QObject *parent) :
    QObject(parent)
{
}
void UIManager::notifyDbOpen(const std::string & fullpath) {
    for (auto uit : DefaultViewTypes) {
        openUI(fullpath, uit);
    }
}
void UIManager::notifyDbClose(const std::string & fullpath) {
    // Close QMdiSubWindows after being notified that library has closed
    ConnViews cvs(m_connViews.selectWheres(fullpath, ViewType::LIBSYMBOLVIEW));
    for (auto cv : cvs) {
        cv.subWidget->close();
    }
}
void UIManager::notifyDbRename(const std::string & oldpath, const std::string & newpath) {
    //log("Notify rename %s to %s", oldpath.c_str(), newpath.c_str());
    // Get the new database from dbif based on newpath
    // Get all entries associated with oldpath from m_connViews
    // Among entries, delete model and create new model to the new database
    // For each entry, change m_connViews to show newpath
    // For each entry, rename QMdiSubWidget title to newpath
    ConnViews dbcvs(m_connViews.selectWheres(oldpath));
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
        cv.model = (this->*makeModelfm[cv.viewType])(newpath);
        cv.view->setModel(cv.model);
        std::string title(std::filesystem::path(newpath).filename().string());
        cv.subWidget->setWindowTitle(QString::fromStdString(title));
        m_connViews.push_back(cv);
        m_connViews.log(m_pLogger);
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
    m_connViews.log(m_pLogger);
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
    // Find current QMdiSubWindow in table, then duplicate with same fullpath and ViewType
    assert(m_pCore);
    const QWidget *cmsw(static_cast<const QWidget *>(activeMdiSubWindow()));
    assert(cmsw);
    auto selectopt(m_connViews.selectWhere(cmsw));
    assert(selectopt);
    QWidget *newWidget(openUI(selectopt->fullpath, selectopt->viewType));
    if (cmsw->isMaximized())
        static_cast<QMdiSubWindow *>(newWidget)->showMaximized();

}
void UIManager::popOutMainView() {
    // Move current main QMdiSubWindow and associated subviews into new window
    // If this is the only MDI of this fullpath in this window, then re-assign
    // mainwindow and subwidgets in table, and detach/reattach same.  If there
    // are other MDIs of this fullpath in this window, then copy the entries in
    // the table for the subviews, associate the current mdi and copied entries
    // with the new window, then detach/attach same.

    QMainWindow *mw(activeWindow<QMainWindow *>());
    DocWindow *dw(static_cast<DocWindow *>(mw));
    const QMainWindow *cmw(static_cast<const QMainWindow *>(mw));

    // Find existing mdisubwindow and its table entry, record its state, and switch
    QMdiSubWindow *currmdi(dw->mdiArea()->activeSubWindow());
    if (!currmdi) {
        log("Cannot pop out.  Sorry.");
        // attempted to switch to next subwindow, and to activate first in list
        // We only seem to get here when there is one non-maximized MDI subwindow
        // TODO: Hack this by temporarily adding fake MDI subwindow.
        return;

    }
    ConnView cmcv(*(m_connViews.selectWhere)(static_cast<const QWidget *>(currmdi)));
    std::string fullpath(cmcv.fullpath);
    ViewType vt(cmcv.viewType);
    bool ismax(currmdi->isMaximized());
    dw->mdiArea()->activateNextSubWindow();

    // make new window
    DocWindow *newdw(static_cast<DocWindow *>(duplicateWindow(mw)));
    newdw->show();


    // Count MDIs (aka main viewtypes) of this fullpath in this window
    std::vector<ViewType> mvt = MainViewTypes;
    //ConnViews sameMDIs(m_connViews.selectWheres(1));
    auto sameMDIs = m_connViews.selectWheres(1);
    //ConnViews sameMDIs(m_connViews.selectWheres(mvt));
    //ConnViews sameMDIs(m_connViews.selectWheres(fullpath, cmw));

    // if one, then reassign and reattach; if more, copy, reassign, reattach
    assert(sameMDIs.size());
    ConnViews sibs(m_connViews.selectWheres(fullpath, cmw)); //all entries of this fullpath and window
    if (sameMDIs.size() == 1) {
        assert(cmcv == sameMDIs.front()); // sanity check
        for (ConnView sibcv : sibs) {
            m_connViews.remove(sibcv);
            sibcv.mainWindow = newdw; //reassign all entries to new window
            m_connViews.push_back(sibcv);
            (this->*attachWidgetfm[sibcv.viewType])(newdw, sibcv.subWidget);
            m_connViews.log(m_pLogger);
        }

    } else { // > 1 mdi of this fullpath
        // Attach MDI widget to new window and reassign in table.
        (this->*attachWidgetfm[vt])(newdw, currmdi);
        m_connViews.remove(cmcv);
        cmcv.mainWindow = newdw;
        m_connViews.push_back(cmcv);

        // Select subview entries and OpenUI with their data in new window
        ConnViews subsibs(m_connViews.selectWheres(SubViewTypes, fullpath, cmw));
        for (auto subsibcv : subsibs) {
            openUI(subsibcv.fullpath, subsibcv.viewType, newdw);
        }
    }
    ismax ? currmdi->showMaximized() : currmdi->showNormal();
    updateLibActions();
}
void UIManager::closeMainView() {
    // Closes current main QMdiSubWindow
    QMdiSubWindow *sw(activeMdiSubWindow());
    assert(sw);
    auto selectopt(m_connViews.selectWhere(static_cast<const QWidget *>(sw)));
    assert(selectopt);
    sw->close();
    m_connViews.remove(*selectopt);
    m_connViews.log(m_pLogger);
}
void UIManager::enableSubView(ViewType vt) {
    log("UIManager::enableSubView");
    QMainWindow *mw(activeWindow<QMainWindow *>());
    const QMainWindow *cmw(mw);
    // Find fullpath assocated with current mdi view, then open
    // If the widget already exists, the openUI function will reuse it.
    const QWidget *sw(activeMdiSubWindow());
    auto selectopt(m_connViews.selectWhere(sw, cmw));
    assert(selectopt);
    assert(m_pCore);
    openUI(selectopt->fullpath, vt, mw);
}
bool UIManager::isViewTypeShowing(ViewType vt, const DocWindow *dw) {
    // This is called when someone wants to know whether a viewtype is showing,
    // not necessarily whether one exists in the table, which can be different.
    // If >0 exists, then exactly one is showing.  If none is showing, then none
    // exist.
    const QMainWindow *cmw(static_cast<const QMainWindow *>(dw));
    std::list<ViewType> svtl(subViewTypesShowing(cmw));
    //std::list<ViewType> mvtl(mainViewTypesShowing(dw));
    return std::find(svtl.begin(), svtl.end(), vt) != svtl.end() ;//||
           //std::find(mvtl.begin(), mvtl.end() != mvtl.end();

}
