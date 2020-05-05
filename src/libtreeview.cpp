#include "libtreeview.h"
#include "libwindow.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTreeView>
#include <QPushButton>
#include <QAbstractItemModel>

#include <exception>

LibTreeView::LibTreeView(QWidget *parent) :
    LibTreeView(parent, nullptr, nullptr, nullptr/*, std::string()*/)
{

}

LibTreeView::LibTreeView(QWidget *parent, QAbstractItemModel *model) :
    LibTreeView(parent, model, nullptr, nullptr/*, std::string()*/)
{

}

LibTreeView::LibTreeView(QWidget *parent, QAbstractItemModel *model, LibCore *pc, ILogger *pl/*, std::string connpath*/) :
    QTreeView(parent)
    /*, LibClient(pc, pl, connpath)*/
    {

    // model should already have hadits internal path set the same as connpath
    QTreeView::setModel(model);
    expandAll();

//    m_pQView = qaiv;
//    QVBoxLayout* vb = new QVBoxLayout();
//    QHBoxLayout* hb = new QHBoxLayout();

//    setLogger(nullptr);

//    QPushButton *pPBNewShape = new QPushButton("New Shape", this);
//    pPBNewShape->setFocusProxy(this);
//    hb->addWidget(pPBNewShape);
//    connect(pPBNewShape, &QPushButton::clicked, [=](){
//        m_pCore->newShape();});

//    QPushButton *pPBNewSymbol = new QPushButton("New Symbol", this);
//    pPBNewSymbol->setFocusProxy(parent);
//    hb->addWidget(pPBNewSymbol);
//    connect(pPBNewSymbol, &QPushButton::clicked, [=](){
//        m_pCore->newSymbol();});

//    QPushButton *pPBDelete = new QPushButton("Delete Item", this);
//    pPBDelete->setFocusProxy(parent);
//    hb->addWidget(pPBDelete);
//    connect(pPBDelete, &QPushButton::clicked, [=](){
//        m_pCore->deleteShape("some_shape_to_delete");});

//    vb->addWidget(m_pQView);
//    vb->addItem(hb);
//    setLayout(vb);
}


