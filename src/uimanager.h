#ifndef UIMANAGER_H
#define UIMANAGER_H

#include "iuimanager.h"
#include "coreable.h"
#include "loggable.h"
#include "closingdockwidget.h"
#include "closingmdiwidget.h"
#include "libclient.h"
#include "libtreeview.h"
//#include "libtableview.h"
#include "idbif.h"
#include <QString>
#include <QWidget>
#include <QMainWindow>
#include <QAbstractItemView>
#include <QStandardItemModel>
#include <QAbstractItemView>
#include <QSqlTableModel>
#include <string>
#include <map>
#include <vector>
#include <any>

// WHy is this a QObject?
class UIManager : public QObject, public IUIManager, public Coreable, public Loggable
{
private:
    QMainWindow *m_parentMW;
    std::list<UIType> m_defaultUITypes;
    std::map<QAbstractItemModel *, std::list<QAbstractItemView *>> m_libViews; // which will also be LibClients
    QStandardItemModel m_siModel; // temporary until real model comes in
    IModelManager *m_pModelManager;
    IViewManager *m_pViewManager;

    ClosingMDIWidget *makeMDILibView(QAbstractItemView *, QString title);
    ClosingDockWidget *makeCDWLibView(QAbstractItemView *, QString title);

    void dockLibView(ClosingDockWidget *, Qt::DockWidgetArea);
public:
    UIManager(QObject * = nullptr);
    void setModelManager(IModelManager *);
    IModelManager *modelManager() const;
    void setViewManager(IViewManager *);
    IViewManager *viewManager() const;

    void notifyDbOpen(IDbIf *, std::string) override; // opens default UI types
    void notifyDbClose(IDbIf *, std::string) override;
    void notifyDbRename(IDbIf *, std::string oldpath, std::string newpath) override;
    void openUI(std::string, UIType); // opens named UI type
    void onDockWidgetClose(QWidget *);
    void onDockWidgetActivate(QWidget *);
    void setParentMW(QMainWindow *);
    //void setDbIf(IDbIf *);
    //IDbIf *dbIf() const;
    QMainWindow *parentMW() const;

public slots:
    //void treeSelectionChangeSlot(const QItemSelection &, const QItemSelection &);

};

#endif // UIMANAGER_H
