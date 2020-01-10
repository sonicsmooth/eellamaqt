#include "libtreewidget.h"
#include "libwindow.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTreeView>
#include <QPushButton>

LibTreeWidget::LibTreeWidget(QWidget *parent, Qt::WindowFlags flags) :
    QWidget(parent, flags)
{
    m_pQTreeView = new QTreeView();
    QVBoxLayout* vb = new QVBoxLayout();
    QHBoxLayout* hb = new QHBoxLayout();

    setLogger(nullptr);

    QPushButton *pPBNewShape = new QPushButton("New Shape", this);
    pPBNewShape->setFocusProxy(this);
    hb->addWidget(pPBNewShape);
    connect(pPBNewShape, &QPushButton::clicked, [=](){
        m_pCore->newShape();});

    QPushButton *pPBNewSymbol = new QPushButton("New Symbol", this);
    pPBNewSymbol->setFocusProxy(parent);
    hb->addWidget(pPBNewSymbol);
    connect(pPBNewSymbol, &QPushButton::clicked, [=](){
        m_pCore->newSymbol();});

    QPushButton *pPBDelete = new QPushButton("Delete Item", this);
    pPBDelete->setFocusProxy(parent);
    hb->addWidget(pPBDelete);
    connect(pPBDelete, &QPushButton::clicked, [=](){
        m_pCore->deleteShape("some_shape_to_delete");});

    vb->addWidget(m_pQTreeView);
    vb->addItem(hb);
    setLayout(vb);
}

void LibTreeWidget::setDbConn(std::string s) {m_DbConn = s;}
std::string LibTreeWidget::DbConn() const {return m_DbConn;}
