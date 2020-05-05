#ifndef UIMANAGER_H
#define UIMANAGER_H

#include "iuimanager.h"
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


typedef struct {
    std::string conn;
    ViewType vt;
    QAbstractItemModel *model;
    QAbstractItemView *view;
    bool selected;
    QMainWindow *mainwindow;
} ConnView;
typedef std::list<ConnView> ConnViews;

// Why is this a QObject?
class UIManager : public QObject, public IUIManager, public Coreable, public Loggable
{
private:

    std::map<ViewType, Qt::DockWidgetArea> dwam = 
     {{ViewType::LIBSYMBOLVIEW, Qt::DockWidgetArea::NoDockWidgetArea},
      {ViewType::LIBTREEVIEW, Qt::DockWidgetArea::RightDockWidgetArea},
      {ViewType::LIBTABLEVIEW, Qt::DockWidgetArea::LeftDockWidgetArea}};
    
    typedef QAbstractItemModel * (UIManager::*abstractModelFn)(IDbIf *, std::string);
    std::map<ViewType, abstractModelFn> makeModelfm = 
     {{ViewType::LIBSYMBOLVIEW, &UIManager::makeLibSymbolModel},
      {ViewType::LIBTREEVIEW, &UIManager::makeLibTreeModel},
      {ViewType::LIBTABLEVIEW, &UIManager::makeLibTableModel}};

    typedef QAbstractItemView * (UIManager::*abstractViewFn)(QAbstractItemModel *);
    std::map<ViewType, abstractViewFn> makeViewfm = 
     {{ViewType::LIBSYMBOLVIEW, &UIManager::makeLibSymbolView},
      {ViewType::LIBTREEVIEW, &UIManager::makeLibTreeView},
      {ViewType::LIBTABLEVIEW, &UIManager::makeLibTableView}};

    QMainWindow *m_parentMW;
    std::list<ViewType> m_defaultViewTypes;
    ConnViews m_connViews;
    std::optional<ConnView> selectWhere(std::string, ViewType);
    std::optional<ConnView> selectWhere(std::string, ViewType, QMainWindow *);
    ConnViews selectWheres(std::string, ViewType); 

    void newMainWindow(IDbIf *, std::string);
    QAbstractItemModel *makeLibSymbolModel(IDbIf *, std::string);
    QAbstractItemModel *makeLibTreeModel(IDbIf *, std::string);
    QAbstractItemModel *makeLibTableModel(IDbIf *, std::string);
    QAbstractItemView *makeLibSymbolView(QAbstractItemModel * /*, std::string*/);
    QAbstractItemView *makeLibTreeView(QAbstractItemModel * /*, std::string*/);
    QAbstractItemView *makeLibTableView(QAbstractItemModel * /*, std::string*/);
    ClosingMDIWidget *makeMDILibWidget(QWidget *, QAbstractItemView *, std::string);
    ClosingDockWidget *makeCDWLibWidget(QWidget *, QAbstractItemView *, std::string);
    void dockLibView(ClosingDockWidget *, Qt::DockWidgetArea); // moves libview to left or right
    void openUI(IDbIf *, std::string, ViewType); // opens named UI type
    //void closeUI(std::string, ViewType);
    //void removeView(QWidget *qw);
    void updateTitle(); // Grabs current database, or delegates to main window
    void onDockWidgetClose(QWidget *);
    void onDockWidgetActivate(QWidget *);

public:
    UIManager(QObject * = nullptr);
    void setParentMW(QMainWindow *);
    QMainWindow *parentMW() const;

    void notifyDbOpen(IDbIf *, std::string) override; // opens default UI types
    void notifyDbClose(IDbIf *, std::string) override;
    void notifyDbRename(IDbIf *, std::string, std::string) override;


public slots:
    //void treeSelectionChangeSlot(const QItemSelection &, const QItemSelection &);

};

#endif // UIMANAGER_H
