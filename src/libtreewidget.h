#ifndef LIBTREEWIDGET_H
#define LIBTREEWIDGET_H

#include <QDockWidget>
#include <QTreeView>
#include <QPushButton>

#include "coreable.h"
#include "loggable.h"

// Inherits Loggable functions setLogger(), logger(), log()
// Inherits coreable functions setCore(), core()

class LibTreeWidget : public QWidget, public Coreable, public Loggable
{
private:
    QTreeView* m_pQTreeView;
    std::string m_DbConn;

public:
    LibTreeWidget(QWidget * = nullptr, Qt::WindowFlags = 0);
    void setDbConn(std::string);
    std::string DbConn() const;
};

#endif // LIBTREEWIDGET_H
