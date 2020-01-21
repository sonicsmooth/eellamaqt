#ifndef LIBVIEWWIDGET_H
#define LIBVIEWWIDGET_H

#include "coreable.h"
#include "loggable.h"
#include "connable.h"

#include <QDockWidget>
#include <QAbstractItemView>
#include <QPushButton>


// Inherits Loggable functions setLogger(), logger(), log()
// Inherits coreable functions setCore(), core()
// Adds db connection

class LibViewWidget : public QWidget, public Coreable, public Loggable, public Connable
{
private:
    QAbstractItemView* m_pQView;

public:
    LibViewWidget(QAbstractItemView * = nullptr, QWidget * = nullptr, Qt::WindowFlags = Qt::WindowFlags());
    void setView(QAbstractItemView *);
    QAbstractItemView *view() const;
};

#endif // LIBVIEWWIDGET_H
