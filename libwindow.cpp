#include "libwindow.h"
#include "ui_libwindow.h"

#include <iostream>
#include <cassert>
#include <QStatusBar>
#include <QPushButton>
#include <QAction>
#include <QFileDialog>

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

    connect(ui->actionNewLib, &QAction::triggered, this, &LibWindow::newLib);
    connect(ui->actionOpenLib, &QAction::triggered, this, &LibWindow::openLib);
    //connect(ui->pbNewShape, &QPushButton::clicked, this, &LibWindow::newLibShape);
    //connect(ui->pbNewSymbol, &QPushButton::clicked, this, &LibWindow::newLibSymbol);


}

LibWindow::~LibWindow()
{
    delete ui;
}

void LibWindow::setCore(LibCore* pc) {
    m_pCore = pc;
}

void LibWindow::newLib(bool checked) {
    (void) checked;
    assert(m_pCore);
    m_pCore->newLib("NewLibrary");
}
void LibWindow::openLib(bool checked) {
    (void) checked;
    assert(m_pCore);
    QString qfilename = QFileDialog::getOpenFileName(this,
        ".", tr("Open Library"), tr("Library files (*.SchLib *.db"));
    std::string filename(qfilename.toStdString());
    m_pCore->openLib(filename);
}
void LibWindow::newLibShape(bool checked) {
    (void) checked;
    if (m_pCore)
        m_pCore->create(Shape("Some new shape"));
}
void LibWindow::newLibSymbol(bool checked) {
    (void) checked;
    if (m_pCore)
        m_pCore->create(Symbol("Some new symbol"));
}
