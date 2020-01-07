#ifndef UIMANAGER_H
#define UIMANAGER_H

#include "iuimanager.h"
#include "coreable.h"
#include "loggable.h"
#include "libtreewidget.h"
#include <QString>
#include <QWidget>
#include <QMainWindow>
#include <string>
#include <map>


class UIManager : public IUIManager, public Coreable, public Loggable
{
private:
    QMainWindow *m_parentMW;
    QDockWidget *openLibTreeView(QString, QString);
    std::map<std::string, QWidget *> m_openWidgets;
public:
    UIManager();
    virtual std::any OpenUI(UITYPE, std::string) override;
    virtual void CloseUI(std::string) override;
    void setParentMW(QMainWindow *);
    QMainWindow *parentMW() const;


};

#endif // UIMANAGER_H
