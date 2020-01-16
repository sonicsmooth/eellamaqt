#ifndef LIBVIEWWIDGET_H
#define LIBVIEWWIDGET_H

#include "coreable.h"
#include "loggable.h"

#include <QDockWidget>
#include <QAbstractItemView>
#include <QPushButton>


// Inherits Loggable functions setLogger(), logger(), log()
// Inherits coreable functions setCore(), core()
// Adds db connection

class LibViewWidget : public QWidget, public Coreable, public Loggable
{
private:
    QAbstractItemView* m_pQView;
    std::string m_DbConn;

public:
    LibViewWidget(QAbstractItemView * = nullptr, QWidget * = nullptr, Qt::WindowFlags = Qt::WindowFlags());
    void setDbConn(std::string);
    void setView(QAbstractItemView *);
    QAbstractItemView *view() const;
    std::string DbConn() const;
};

#endif // LIBVIEWWIDGET_H
