#ifndef UIMANAGER_H
#define UIMANAGER_H

#include "libcore.h"
#include "iuimanager.h"
#include "docwindow.h"
#include "coreable.h"
#include "loggable.h"
#include "libtreeview.h"
#include "libtableview.h"
#include "closingdockwidget.h"
#include "closingmdiwidget.h"
#include "idbif.h"
#include "mvtypes.h"

#include <QString>
#include <QWidget>
#include <QMainWindow>
#include <QAbstractItemView>
#include <QStandardItemModel>
#include <QAbstractItemView>
#include <QTimer>

#include <string>
#include <map>
#include <list>
#include <optional>
#include <functional>

typedef struct connview {
    std::string fullpath;
    ViewType viewType;
    QAbstractItemModel *model;
    QAbstractItemView *view;
    QWidget *subWidget; // could be QMdiSubWindow or QDockWidget
    QMainWindow *mainWindow;
    // operator== is used for removing a struct from m_connViews
    bool operator==(const struct connview& a) const  {
         return ( a.fullpath == this->fullpath && 
                  a.viewType == this->viewType &&
                  a.model == this->model &&
                  a.view  == this->view &&
                  a.subWidget == this->subWidget &&
                  a.mainWindow == this->mainWindow);
    }
    bool operator!=(const struct connview& a) const  {
         return ( a.fullpath != this->fullpath ||
                  a.viewType != this->viewType ||
                  a.model != this->model ||
                  a.view  != this->view ||
                  a.subWidget != this->subWidget ||
                  a.mainWindow != this->mainWindow);
    }
    void log(ILogger *);
} ConnView;

typedef std::list<ConnView> ConnViews;
void cvlog(ConnViews cvs, ILogger *lgr);




// This a QObject so it can participate in signals/slots
class UIManager : public QObject, public IUIManager, public Coreable, public Loggable
{
private:
    QTimer m_hackTimer;
    
    typedef QAbstractItemModel *(UIManager::*abstractModelFn)(std::string);
    std::map<ViewType, abstractModelFn> makeModelfm = 
     {{ViewType::LIBSYMBOLVIEW, &UIManager::makeLibSymbolModel},
      {ViewType::LIBTREEVIEW, &UIManager::makeLibTreeModel},
      {ViewType::LIBTABLEVIEW, &UIManager::makeLibTableModel}};

    typedef QAbstractItemView *(UIManager::*abstractViewFn)(QAbstractItemModel *);
    std::map<ViewType, abstractViewFn> makeViewfm = 
     {{ViewType::LIBSYMBOLVIEW, &UIManager::makeLibSymbolView},
      {ViewType::LIBTREEVIEW, &UIManager::makeLibTreeView},
      {ViewType::LIBTABLEVIEW, &UIManager::makeLibTableView}};

    typedef QWidget *(UIManager::*widgetFactoryFn)(QWidget *, QWidget *, std::string);
    std::map<ViewType, widgetFactoryFn> makeWidgetfm =
    {{ViewType::LIBSYMBOLVIEW, &UIManager::makeMDILibWidget},
     {ViewType::LIBTREEVIEW, &UIManager::makeCDWLibWidget},
     {ViewType::LIBTABLEVIEW, &UIManager::makeCDWLibWidget}};

    typedef void (UIManager::*attachFn)(QMainWindow *, QWidget *);
    std::map<ViewType, attachFn> attachWidgetfm =
    {{ViewType::LIBSYMBOLVIEW, &UIManager::attachMDISubWindow},
     {ViewType::LIBTREEVIEW, &UIManager::attachDockWidget},
     {ViewType::LIBTABLEVIEW, &UIManager::attachDockWidget}};

    std::map<ViewType, Qt::DockWidgetArea> dockWidgetAreaMap = 
     {//{ViewType::LIBSYMBOLVIEW, Qt::DockWidgetArea::NoDockWidgetArea},
      {ViewType::LIBTREEVIEW, Qt::DockWidgetArea::RightDockWidgetArea},
      {ViewType::LIBTABLEVIEW, Qt::DockWidgetArea::LeftDockWidgetArea}};

    ConnViews m_connViews;
    std::optional<ConnView> selectWhere(const QWidget *);
    std::optional<ConnView> selectWhere(const std::function<bool (const ConnView &)> &);
    template <typename Arg>
    ConnViews selectWhere_ext(const ConnViews &, Arg);
    template <typename Arg, typename... Args>
    ConnViews selectWhere_ext(const ConnViews &, Arg, Args... );
    template <typename... Args>
    std::optional<ConnView> selectWhere_ext(Args... );
    template <typename... Args>
    ConnViews selectWheres_ext(Args... );
    ConnViews selectWheres(const std::function<bool(const ConnView &)> &);

    QAbstractItemModel *makeLibSymbolModel(std::string);
    QAbstractItemModel *makeLibTreeModel(std::string);
    QAbstractItemModel *makeLibTableModel(std::string);
    QAbstractItemView  *makeLibSymbolView(QAbstractItemModel *);
    QAbstractItemView  *makeLibTreeView(QAbstractItemModel *);
    QAbstractItemView  *makeLibTableView(QAbstractItemModel *);
    QWidget *makeMDILibWidget(QWidget *, QWidget *, std::string);
    QWidget *makeCDWLibWidget(QWidget *, QWidget *, std::string);
    QWidget *makeModelViewWidget(std::string, ViewType, QMainWindow *);
    void attachMDISubWindow(QMainWindow *, QWidget *);
    void attachDockWidget(QMainWindow *, QWidget *);

    QWidget *openUI(std::string, ViewType, QMainWindow * = nullptr); // opens named UI type
    //void closeUI(std::string, ViewType);
    //void removeView(QWidget *qw);
    void onDocWindowActivate(QWidget *);
    void onDocWindowClose(QWidget *);
    void onMdiSubWindowActivate(QWidget *);
    void onMdiSubWindowClose(QWidget *);
    void onDockWidgetActivate(QWidget *);
    void onDockWidgetClose(QWidget *);
    void updateLibActions();
    std::list<ViewType> subViewTypesShowing(const QMainWindow *);
    //std::list<ViewType> mainViewTypesShowing(const DocWindow *);
    bool isChildWidgetShowing(const QMainWindow *, const QWidget *);
    

public:
    UIManager(QObject * = nullptr);
    std::list<QMainWindow *> mainWindows(); // Returns list of main windows
    void notifyDbOpen(std::string) override; // opens default UI types
    void notifyDbClose(std::string) override;
    void notifyDbRename(std::string, std::string) override;
    void *newWindow() override; // Creates new top level window
    void *newWindow(LibCore *, ILogger *) override;
    void *duplicateWindow() override; // duplicate active w/o children
    void *duplicateWindow(void *) override; // duplicate given w/o children
    void closeWindow() override; // Closes current top level window
    void closeWindow(void *) override; // Closes given window
    void duplicateMainView() override;
    void popOutMainView() override;
    void closeMainView() override;
    void enableSubView(ViewType) override;
    bool isViewTypeShowing(ViewType, const DocWindow *);

public slots:
    //void treeSelectionChangeSlot(const QItemSelection &, const QItemSelection &);

};

#endif // UIMANAGER_H
