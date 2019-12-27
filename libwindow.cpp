#include "libwindow.h"
#include "ui_libwindow.h"

#include <iostream>
#include <QStatusBar>
#include <QPushButton>

LibWindow::LibWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LibWindow)
    , m_pCore(nullptr)
{
    ui->setupUi(this);

    setWindowTitle("Library Editor");


    QPushButton *pb = new QPushButton(this);
    pb->setText("Push me");
    connect(pb, &QPushButton::clicked,[=](){statusBar()->showMessage("hi",1000);});
    statusBar()->showMessage("The message!", 1000);
    statusBar()->addPermanentWidget(pb);

    connect(ui->pbNewShape, &QPushButton::clicked, this, &LibWindow::newLibShape);
    connect(ui->pbNewSymbol, &QPushButton::clicked, this, &LibWindow::newLibSymbol);
}

LibWindow::~LibWindow()
{
    delete ui;
}

void LibWindow::setCore(LibEdCore* pc) {
    m_pCore = pc;
}

void LibWindow::newLibShape(bool checked) {
    (void) checked;
    if (m_pCore)
        m_pCore->Create(Shape("Some new shape"));
}
void LibWindow::newLibSymbol(bool checked) {
    (void) checked;
    if (m_pCore)
        m_pCore->Create(Symbol("Some new symbol"));
}
