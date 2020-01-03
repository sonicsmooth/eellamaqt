#include "libwindow.h"
#include "libtreewidget.h"
#include "ui_libwindow.h"

#include <iostream>
#include <cstdio>
#include <sstream>
#include <cassert>
#include <any>

#include <QStatusBar>
#include <QPushButton>
#include <QAction>
#include <QFileDialog>
#include <QDir>
#include <QDockWidget>
#include <QtSql>

LibWindow::LibWindow(QWidget *parent)
    : QMainWindow(parent),
    m_pCore(nullptr),
    m_pLogger(nullptr),
    m_DbHandle(),
    ui(new Ui::LibWindow)
{
    ui->setupUi(this);

    setWindowTitle("Library Editor");

    QPushButton *pb = new QPushButton(this);
    pb->setText("Push me");
    connect(pb, &QPushButton::clicked,[=](){statusBar()->showMessage("hi",1000);});
    statusBar()->showMessage("The message!", 1000);
    statusBar()->addPermanentWidget(pb);

    
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
void LibWindow::setLogger(Logger* pl) {
    m_pLogger = pl;
}
void LibWindow::setDbHandle(std::any dbh) {
    m_DbHandle = dbh;
}
std::any LibWindow::DbHandle() const {
    return m_DbHandle;
}

void LibWindow::_openLibTreeView(QString title) {
    LibTreeWidget* trLibItems = new LibTreeWidget();
    connect(trLibItems->PBShape(), &QPushButton::clicked, ui->actionNewShape, &QAction::trigger);
    connect(trLibItems->PBSymbol(), &QPushButton::clicked, ui->actionNewSymbol, &QAction::trigger);
    connect(trLibItems->PBDelete(), &QPushButton::clicked, ui->actionEditDelete, &QAction::trigger);

    QDockWidget* dwLibItems = new QDockWidget(this);
    dwLibItems->setWidget(trLibItems);
    dwLibItems->setWindowTitle(title);
    this->addDockWidget(static_cast<Qt::DockWidgetArea>(2), dwLibItems);
}

void LibWindow::fileNewLib() {
    // Check existing fies and add suffix if needed.  Finds first "open" spot between 00 and 99, inclusive
    assert(m_pCore);
    assert(m_pLogger);

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
    QString fullpath = currdir.filePath(libname_extended.get());

    m_pLogger->log("LibWindow: file new lib");
    m_pLogger->log("Before creating db:");
    QSqlDatabase db = QSqlDatabase::database(fullpath);
    QStringList tables = db.tables();
    for (auto s : tables) {
        m_pLogger->log(s);
    }

    m_pCore->newLib(fullpath.toStdString());

    m_pLogger->log("After creating db:");
    db = QSqlDatabase::database(fullpath);
    tables = db.tables();
    for (auto s : tables) {
        m_pLogger->log(s);
    }


   // _openLibTreeView(QString::fromStdString(libname_extstr));

}
void LibWindow::fileOpenLib() {
    assert(m_pCore);
    assert(m_pLogger);
    QString qfilename = QFileDialog::getOpenFileName(this,
        ".", tr("Open Library"), tr("Any (*);;Library files (*.SchLib *.db)"));
    std::string filename(qfilename.toStdString());
    m_pCore->openLib(filename);
}
void LibWindow::fileSaveLib() {
    assert(m_pCore);
    assert(m_pLogger);
    m_pLogger->log("LibWindow: file save lib");
}
void LibWindow::fileSaveLibAs() {
    assert(m_pCore);
    assert(m_pLogger);
    m_pLogger->log("LibWindow: file save lib as");
}
void LibWindow::fileCloseLib() {
    assert(m_pCore);
    assert(m_pLogger);
    m_pLogger->log("LibWindow: file close lib");
}
void LibWindow::fileDeleteLib() {
    assert(m_pCore);
    assert(m_pLogger);
    m_pLogger->log("LibWindow: file delete lib");
}
void LibWindow::newShape() {
    assert(m_pCore);
    assert(m_pLogger);
    m_pLogger->log("LibWindow: new shape");
}
void LibWindow::insertShape() {
    assert(m_pCore);
    assert(m_pLogger);
    m_pLogger->log("LibWindow: insert shape");
}
void LibWindow::newSymbol() {
    assert(m_pCore);
    assert(m_pLogger);
    m_pLogger->log("LibWindow: new symbol");
}
void LibWindow::newRectangle() {    assert(m_pCore);
    assert(m_pLogger);
    m_pLogger->log("LibWindow: new rectangle");
}

void LibWindow::newLine() {
    assert(m_pCore);
    assert(m_pLogger);
    m_pLogger->log("LibWindow: new line");

}
void LibWindow::newText() {
    assert(m_pCore);
    assert(m_pLogger);
    m_pLogger->log("LibWindow: new text");

}
void LibWindow::newCircle() {
    assert(m_pCore);
    assert(m_pLogger);
    m_pLogger->log("LibWindow: new circle");

}
void LibWindow::newArrow() {
    assert(m_pCore);
    assert(m_pLogger);
    m_pLogger->log("LibWindow: new arrow");

}
void LibWindow::newPolygon() {
    assert(m_pCore);
    assert(m_pLogger);
    m_pLogger->log("LibWindow: new polygon");

}
void LibWindow::newPolyline() {
    assert(m_pCore);
    assert(m_pLogger);
    m_pLogger->log("LibWindow: new polyline");

}
void LibWindow::newPin() {
    assert(m_pCore);
    assert(m_pLogger);
    m_pLogger->log("LibWindow: new pin");

}
void LibWindow::editUndo() {
    assert(m_pCore);
    assert(m_pLogger);
    m_pLogger->log("LibWindow: edit undo");

}
void LibWindow::editRedo() {
    assert(m_pCore);
    assert(m_pLogger);
    m_pLogger->log("LibWindow: edit redo");

}
void LibWindow::editMove() {
    assert(m_pCore);
    assert(m_pLogger);
    m_pLogger->log("LibWindow: edit move");

}
void LibWindow::editRotate() {
    assert(m_pCore);
    assert(m_pLogger);
    m_pLogger->log("LibWindow: edit rotate");

}
void LibWindow::editCut() {
    assert(m_pCore);
    assert(m_pLogger);
    m_pLogger->log("LibWindow: edit cut");

}
void LibWindow::editCopy() {
    assert(m_pCore);
    assert(m_pLogger);
    m_pLogger->log("LibWindow: edit copy");

}
void LibWindow::editPaste() {
    assert(m_pCore);
    assert(m_pLogger);
    m_pLogger->log("LibWindow: edit paste");

}
void LibWindow::editDelete() {
    assert(m_pCore);
    assert(m_pLogger);
    m_pLogger->log("LibWindow: edit delete");
    m_pCore->deleteItem(Symbol("Some symbol"));
    m_pCore->deleteItem(Shape("Some shape"));
}
void LibWindow::helpAbout() {}

