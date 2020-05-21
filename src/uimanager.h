#ifndef UIMANAGER_H
#define UIMANAGER_H

#include "libcore.h"
#include "iuimanager.h"
#include "libwindow.h"
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
    QWidget *subWidget; // colud be QMdiSubWindow or QDockWidget
    QMainWindow *mainWindow;
    // operator== is used for removing a struct from m_connViews
    bool operator==(const struct connview& a) const
    {
         return ( a.fullpath == this->fullpath && 
                  a.viewType == this->viewType &&
                  a.model == this->model &&
                  a.view  == this->view &&
                  a.subWidget == this->subWidget &&
                  a.mainWindow == this->mainWindow);
    }
    void log(ILogger *);
} ConnView;
typedef std::list<ConnView> ConnViews;
void cvlog(ConnViews cvs, ILogger *lgr);




// Why is this a QObject?
class UIManager : public QObject, public IUIManager, public Coreable, public Loggable
{
private:

    std::map<ViewType, Qt::DockWidgetArea> dockWidgetAream = 
     {//{ViewType::LIBSYMBOLVIEW, Qt::DockWidgetArea::NoDockWidgetArea},
      {ViewType::LIBTREEVIEW, Qt::DockWidgetArea::RightDockWidgetArea},
      {ViewType::LIBTABLEVIEW, Qt::DockWidgetArea::LeftDockWidgetArea}};
    
    typedef QAbstractItemModel *(UIManager::*abstractModelFn)(IDbIf *, std::string);
    std::map<ViewType, abstractModelFn> makeModelfm = 
     {{ViewType::LIBSYMBOLVIEW, &UIManager::makeLibSymbolModel},
      {ViewType::LIBTREEVIEW, &UIManager::makeLibTreeModel},
      {ViewType::LIBTABLEVIEW, &UIManager::makeLibTableModel}};

    typedef QAbstractItemView * (UIManager::*abstractViewFn)(QAbstractItemModel *);
    std::map<ViewType, abstractViewFn> makeViewfm = 
     {{ViewType::LIBSYMBOLVIEW, &UIManager::makeLibSymbolView},
      {ViewType::LIBTREEVIEW, &UIManager::makeLibTreeView},
      {ViewType::LIBTABLEVIEW, &UIManager::makeLibTableView}};

    std::list<ViewType> m_defaultViewTypes;
    ConnViews m_connViews;
    std::optional<ConnView> selectWhere(QDockWidget *);
    std::optional<ConnView> selectWhere(QMdiSubWindow *);
    std::optional<ConnView> selectWhere(QMainWindow *);
    std::optional<ConnView> selectWhere(std::string, ViewType);
    std::optional<ConnView> selectWhere(std::string, ViewType, QMainWindow *);
    // From https://stackoverflow.com/questions/40844622/use-a-lambda-as-a-parameter-for-a-c-function
    template <typename F>
    std::optional<ConnView> selectWhere(F&&);
    ConnViews selectWheres(std::string);
    ConnViews selectWheres(ViewType);
    ConnViews selectWheres(QMainWindow *);
    ConnViews selectWheres(std::string, ViewType);

    QAbstractItemModel *makeLibSymbolModel(IDbIf *, std::string);
    QAbstractItemModel *makeLibTreeModel(IDbIf *, std::string);
    QAbstractItemModel *makeLibTableModel(IDbIf *, std::string);
    QAbstractItemView *makeLibSymbolView(QAbstractItemModel *);
    QAbstractItemView *makeLibTreeView(QAbstractItemModel *);
    QAbstractItemView *makeLibTableView(QAbstractItemModel *);
    ClosingMDIWidget *makeMDILibWidget(QWidget *, QAbstractItemView *, std::string);
    ClosingDockWidget *makeCDWLibWidget(QWidget *, QAbstractItemView *, std::string);
    void dockLibView(ClosingDockWidget *, Qt::DockWidgetArea); // moves libview to left or right
    QWidget *openUI(IDbIf *, std::string, ViewType); // opens named UI type
    //void closeUI(std::string, ViewType);
    //void removeView(QWidget *qw);
    void onDockWidgetActivate(QWidget *);
    void onDockWidgetClose(QWidget *);
    void onMdiSubWindowActivate(QWidget *);
    void onMdiSubWindowClose(QWidget *);
    void onMainWindowClose(QWidget *);
    

public:
    UIManager(QObject * = nullptr);
    std::list<QMainWindow *> mainWindows(); // Returns list of main windows
    void notifyDbOpen(IDbIf *, std::string) override; // opens default UI types
    void notifyDbClose(IDbIf *, std::string) override;
    void notifyDbRename(IDbIf *, std::string, std::string) override;
    void *newWindow() override; // Creates new top level window
    void *newWindow(LibCore *, ILogger *) override;
    void *duplicateWindow() override; // duplicate active w/o children
    void *duplicateWindow(void *) override; // duplicate given w/o children
    void closeWindow() override; // Closes current top level window
    void closeWindow(void *) override; // Closes given window
    void duplicateMainView() override;
    void popOutMainView() override;
    void closeMainView() override;

public slots:
    //void treeSelectionChangeSlot(const QItemSelection &, const QItemSelection &);

};

#endif // UIMANAGER_H
