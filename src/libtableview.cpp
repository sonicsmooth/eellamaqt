#include "libtableview.h"
#include "docwindow.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTableView>
#include <QPushButton>
#include <QAbstractItemModel>

#include <exception>

LibTableView::LibTableView(QWidget *parent) :
    LibTableView(parent, nullptr, nullptr, nullptr, std::string())
{

}

LibTableView::LibTableView(QWidget *parent, QAbstractItemModel *model) :
    LibTableView(parent, model, nullptr, nullptr, std::string())
{

}

LibTableView::LibTableView(QWidget *parent, QAbstractItemModel *model, LibCore *pc, ILogger *pl, std::string connpath) :
    QTableView(parent),
    LibClient(pc, pl, connpath)
    {

    // model should already have had its internal path set the same as connpath
    QTableView::setModel(model);
    
    //expandAll();
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


