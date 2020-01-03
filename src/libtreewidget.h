#ifndef LIBTREEWIDGET_H
#define LIBTREEWIDGET_H

#include <QDockWidget>
#include <QTreeView>
#include <QPushButton>

#include "idbif.h"


class LibTreeWidget : public QWidget
{
private:
    IDbIf* m_pIDbIf;
    QTreeView* m_pQTreeView;
    QPushButton* m_pPBNewShape;
    QPushButton* m_pPBNewSymbol;
    QPushButton* m_pPBDelete;

public:
    LibTreeWidget(QWidget* parent = nullptr, Qt::WindowFlags flags = 0);
    QPushButton* PBShape() const;
    QPushButton* PBSymbol() const;
    QPushButton* PBDelete() const;
    void setDbIf(IDbIf*);
    IDbIf* DbIf() const;

};

#endif // LIBTREEWIDGET_H
