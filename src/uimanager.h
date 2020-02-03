#ifndef UIMANAGER_H
#define UIMANAGER_H

#include "iuimanager.h"
#include "coreable.h"
#include "loggable.h"
#include "closingdockwidget.h"
#include "libviewwidget.h"
#include <QString>
#include <QWidget>
#include <QMainWindow>
#include <QAbstractItemView>
#include <QStandardItemModel>
#include <string>
#include <map>
#include <vector>
#include <any>


class UIManager : public QObject, public IUIManager, public Coreable, public Loggable
{
private:
    QMainWindow *m_parentMW;
    std::list<UIType> m_defaultUITypes;
    std::map<std::string, std::list<ClosingDockWidget *>> m_openLibWidgets;
    std::map<std::string, std::list<Connable *>> m_openConnWidgets;
    QStandardItemModel *m_siModel;

    ClosingDockWidget *makeLibView(QAbstractItemView *, QString title);
    void dockLibView(ClosingDockWidget *, Qt::DockWidgetArea);
public:
    UIManager(QObject * = nullptr);
    void openUI(std::string) override; // opens default UI types
    std::any openUI(std::string, UIType); // opens named UI type
    void closeUI(std::string) override;
    void retargetUI(std::string oldpath, std::string newpath) override;
    void onDockWidgetClose(QWidget *);
    void onDockWidgetActivate(QWidget *);
    void setParentMW(QMainWindow *);
    QMainWindow *parentMW() const;

public slots:
    //void treeSelectionChangeSlot(const QItemSelection &, const QItemSelection &);

};

#endif // UIMANAGER_H
