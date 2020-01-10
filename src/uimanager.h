#ifndef UIMANAGER_H
#define UIMANAGER_H

#include "iuimanager.h"
#include "coreable.h"
#include "loggable.h"
#include "closingdockwidget.h"
#include "libtreewidget.h"
#include <QString>
#include <QWidget>
#include <QMainWindow>
#include <string>
#include <map>


class UIManager : public QObject, public IUIManager, public Coreable, public Loggable
{
private:
    QMainWindow *m_parentMW;
    ClosingDockWidget *openLibTreeView(QString, QString);
    std::map<QWidget *, std::string> m_openWidgets;
public:
    UIManager(QObject * = nullptr);
    virtual std::any openUI(UITYPE, std::string) override;
    virtual void closeUI(std::string) override;
    void onWidgetClose(QWidget *);
    void setParentMW(QMainWindow *);
    QMainWindow *parentMW() const;


};

#endif // UIMANAGER_H
