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

    connect(ui->pbNewShape, &QPushButton::clicked, ui->actionShape, &QAction::trigger);
    connect(ui->pbNewSymbol, &QPushButton::clicked, ui->actionSymbol, &QAction::trigger);
    connect(ui->pbDelete, &QPushButton::clicked, ui->actionDelete, &QAction::trigger);
    
    connect(ui->actionNewLib, &QAction::triggered, this, &LibWindow::newLib);
    connect(ui->actionOpenLib, &QAction::triggered, this, &LibWindow::openLib);
    connect(ui->actionCloseLib, &QAction::triggered, this, &LibWindow::closeLib);
    connect(ui->actionDeleteLib, &QAction::triggered, this, &LibWindow::deleteLib);
    connect(ui->actionShape, &QAction::triggered, this, &LibWindow::newLibShape);
    connect(ui->actionSymbol, &QAction::triggered, this, &LibWindow::newLibSymbol);
    connect(ui->actionDelete, &QAction::triggered, this, &LibWindow::deleteSelected);
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
        ".", tr("Open Library"), tr("Any (*);;Library files (*.SchLib *.db)"));
    std::string filename(qfilename.toStdString());
    m_pCore->openLib(filename);
}
void LibWindow::closeLib(bool checked) {
    (void) checked;
    assert(m_pCore);
    m_pCore->closeLib("somelibname");
}
void LibWindow::deleteLib(bool checked) {
    (void) checked;
    assert(m_pCore);
    m_pCore->deleteLib("somelibname");
}
void LibWindow::newLibShape(bool checked) {
    (void) checked;
    assert(m_pCore);
    m_pCore->create(Shape("Some new shape"));
}
void LibWindow::newLibSymbol(bool checked) {
    (void) checked;
    assert(m_pCore);
    m_pCore->create(Symbol("Some new symbol"));
}
void LibWindow::deleteSelected(bool checked) {
    (void) checked;
    assert(m_pCore);
    m_pCore->deleteItem(Symbol("Some symbol"));
    m_pCore->deleteItem(Shape("Some shape"));
}
