#include "libwindow.h"
#include "ui_libwindow.h"

#include <iostream>
//#include <iomanip>
#include <cstdio>
#include <sstream>
#include <cassert>
#include <QStatusBar>
#include <QPushButton>
#include <QAction>
#include <QFileDialog>
#include <QDir>

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
    
    connect(ui->actionFileNewLib, &QAction::triggered, this, &LibWindow::fileNewLib);
    connect(ui->actionFileOpenLib, &QAction::triggered, this, &LibWindow::fileOpenLib);
    connect(ui->actionFileSaveLib, &QAction::triggered, this, &LibWindow::fileSaveLib);
    connect(ui->actionFileSaveLibAs, &QAction::triggered, this, &LibWindow::fileSaveLibAs);
    connect(ui->actionFileCloseLib, &QAction::triggered, this, &LibWindow::fileCloseLib);
    connect(ui->actionFileDeleteLib, &QAction::triggered, this, &LibWindow::fileDeleteLib);
    connect(ui->actionNewShape, &QAction::triggered, this, &LibWindow::newShape);
    connect(ui->actionInsertShape, &QAction::triggered, this, &LibWindow::insertShape);
    connect(ui->actionNewSymbol, &QAction::triggered, this, &LibWindow::newSymbol);
    connect(ui->actionNewRectangle, &QAction::triggered, this, &LibWindow::newRectangle);
    connect(ui->actionNewLine, &QAction::triggered, this, &LibWindow::newLine);
    connect(ui->actionNewText, &QAction::triggered, this, &LibWindow::newText);
    connect(ui->actionNewCircle, &QAction::triggered, this, &LibWindow::newCircle);
    connect(ui->actionNewArrow, &QAction::triggered, this, &LibWindow::newArrow);
    connect(ui->actionNewPolygon, &QAction::triggered, this, &LibWindow::newPolygon);
    connect(ui->actionNewPolyline, &QAction::triggered, this, &LibWindow::newPolyline);
    connect(ui->actionNewPin, &QAction::triggered, this, &LibWindow::newPin);
    connect(ui->actionEditUndo, &QAction::triggered, this, &LibWindow::editUndo);
    connect(ui->actionEditRedo, &QAction::triggered, this, &LibWindow::editRedo);
    connect(ui->actionEditMove, &QAction::triggered, this, &LibWindow::editMove);
    connect(ui->actionEditRotate, &QAction::triggered, this, &LibWindow::editRotate);
    connect(ui->actionEditCut, &QAction::triggered, this, &LibWindow::editCut);
    connect(ui->actionEditCopy, &QAction::triggered, this, &LibWindow::editCopy);
    connect(ui->actionEditPaste, &QAction::triggered, this, &LibWindow::editPaste);
    connect(ui->actionEditDelete, &QAction::triggered, this, &LibWindow::editDelete);
    connect(ui->actionHelpAbout, &QAction::triggered, this, &LibWindow::helpAbout);
}

LibWindow::~LibWindow()
{
    delete ui;
}

void LibWindow::setCore(LibCore* pc) {
    m_pCore = pc;
}

void LibWindow::newLib(bool checked) {
    // Check existing fies and add suffix if needed.  Finds first "open" spot between 00 and 99, inclusive
    (void) checked;
    assert(m_pCore);

    // Ensure directory exists ~/EELlama Libraries
    QDir currdir = QDir::home();
    QString dirname = "EE LLama Libraries";
    if (!currdir.exists(dirname))
        currdir.mkdir(dirname);
    currdir.cd(dirname);

    // Keep trying until we find a non-existing filename
    const char base[] = "NewLibrary";
    const char ext[] = ".SchLib";
    QString libname = QString(base) + ext;
    const int sz = libname.size();
    std::unique_ptr<char> libname_extended(new char [static_cast<size_t>(sz)+3]);
    snprintf(libname_extended.get(), static_cast<size_t>(sz)+1, "%s%s", base, ext);
    if (currdir.exists(libname_extended.get())) {
        for (int suffix = 0; suffix < 100; suffix++) {
            snprintf(libname_extended.get(), static_cast<size_t>(sz)+3, "%s%02d%s", base, suffix, ext);
            if (!currdir.exists(QString(libname_extended.get())))
                break;
        }
    }
    std::cout << libname_extended;
    m_pCore->newLib(currdir.filePath("").toStdString(),
                    std::string(libname_extended.get())
                    //std::move(libname_extended.get())
                    );
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
