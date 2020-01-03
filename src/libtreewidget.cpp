#include "libtreewidget.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTreeView>
#include <QPushButton>
#include <QTextEdit>

LibTreeWidget::LibTreeWidget(QWidget * parent, Qt::WindowFlags flags) :
    QWidget(parent, flags)
{
    m_pQTreeView = new QTreeView();
    QVBoxLayout* vb = new QVBoxLayout(this);
    QHBoxLayout* hb = new QHBoxLayout(this);
    m_pPBNewShape = new QPushButton("New Shape", this);
    m_pPBNewSymbol = new QPushButton("New Symbol", this);
    m_pPBDelete = new QPushButton("Delete Item", this);
    vb->addWidget(m_pQTreeView);
    hb->addWidget(m_pPBNewShape);
    hb->addWidget(m_pPBNewSymbol);
    hb->addWidget(m_pPBDelete);
    vb->addItem(hb);
}

QPushButton* LibTreeWidget::PBShape() const {return m_pPBNewShape;}
QPushButton* LibTreeWidget::PBSymbol() const {return m_pPBNewSymbol;}
QPushButton* LibTreeWidget::PBDelete() const {return m_pPBDelete;}
void LibTreeWidget::setDbIf(IDbIf* pidbif) {m_pIDbIf = pidbif;}
IDbIf* LibTreeWidget::DbIf() const {return m_pIDbIf;}
