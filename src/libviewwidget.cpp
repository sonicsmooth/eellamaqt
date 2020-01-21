#include "libviewwidget.h"
#include "libwindow.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTreeView>
#include <QPushButton>

#include <exception>

LibViewWidget::LibViewWidget(QAbstractItemView *qaiv, QWidget *parent, Qt::WindowFlags flags) :
    QWidget(parent, flags)
{
    m_pQView = qaiv;
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

    vb->addWidget(m_pQView);
    vb->addItem(hb);
    setLayout(vb);
}

//void LibViewWidget::setDbConn(std::string s) {
//    m_DbConn = s;
//}

//const std::string LibViewWidget::dbConn() const {
//    return m_DbConn;
//}

void LibViewWidget::setView(QAbstractItemView *vw = nullptr) {
    m_pQView = vw;
    QVBoxLayout *vb = dynamic_cast<QVBoxLayout *>(layout());
    QLayoutItem *item = vb->takeAt(0);
    vb->removeWidget(item->widget());
    delete item;
    vb->insertWidget(0, vw);
}
QAbstractItemView *LibViewWidget::view() const {return m_pQView;}
