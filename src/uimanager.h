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
#include <vector>

// Why is this a QObject?

typedef std::map<ViewType, std::list<QAbstractItemView *>> TypeViewMap;

class UIManager : public QObject, public IUIManager, public Coreable, public Loggable
{
private:
    QMainWindow *m_parentMW;
    std::list<ViewType> m_defaultViewTypes;
    // Each model maps to list of views, each also a LibClient
    std::map<std::string, TypeViewMap> m_connViews;

    QAbstractItemView *makeLibTreeView(IDbIf *, std::string);
    QAbstractItemView *makeLibTableView(IDbIf *, std::string);
    ClosingMDIWidget *makeMDILibWidget(QAbstractItemView *, std::string);
    ClosingDockWidget *makeCDWLibWidget(QAbstractItemView *, std::string);
    void dockLibView(ClosingDockWidget *, Qt::DockWidgetArea);
    void openUI(IDbIf *, std::string, ViewType); // opens named UI type
    void closeUI(std::string, ViewType);

public:
    UIManager(QObject * = nullptr);

    void notifyDbOpen(IDbIf *, std::string) override; // opens default UI types
    void notifyDbClose(IDbIf *, std::string) override;
    void notifyDbRename(IDbIf *, std::string, std::string) override;

    // Do these need to be public?
    void onDockWidgetClose(QWidget *);
    void onDockWidgetActivate(QWidget *);
    void setParentMW(QMainWindow *);
    QMainWindow *parentMW() const;

public slots:
    //void treeSelectionChangeSlot(const QItemSelection &, const QItemSelection &);

};

#endif // UIMANAGER_H
