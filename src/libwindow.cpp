#include "libwindow.h"
#include "libtreewidget.h"
#include "ui_libwindow.h"
#include "closingdockwidget.h"

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
//#include <QDockWidget>
#include <QtSql>

LibWindow::LibWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::LibWindow)
{
    ui->setupUi(this);

    setWindowTitle("Library Editor");

    QPushButton *pb = new QPushButton(this);
    pb->setText("Push me");
    //connect(pb, &QPushButton::clicked,[=](){statusBar()->showMessage("hi", 1000);});
    connect(pb, &QPushButton::clicked,[=](){m_pCore->newShape();});
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

    // This handles focusing on the dockwidget and its content widget
    connect(qApp, &QApplication::focusChanged, [&](QWidget* old, QWidget* now) {
        // The goal is to get to the LibTreeWidget which has the dbConn.
        // But the LibTreeWidget doesn't receive focus events; only the QTreeView
        // and the ClosingDockWidget get focus events.  In the first case the
        // LibTreeWidget is the parent of the QTreeView.  In the second case the
        // LibTreeWidget is the content widget of the ClosingDockWidget.  We don't
        // know which one we get here unless we try to cast it.
        (void) old;
        QTreeView* qtv(dynamic_cast<QTreeView*>(now));
        LibTreeWidget* ltw(nullptr);
        if (qtv) {
            ltw = static_cast<LibTreeWidget*>(qtv->parentWidget());
        }
        else {
            ClosingDockWidget* ldw(dynamic_cast<ClosingDockWidget*>(now));
            if (ldw) {
                ltw = static_cast<LibTreeWidget*>(ldw->widget());
            }
        }
        if (ltw) {
            m_pCore->pushActiveDb(ltw->DbConn());
        }
    });

}


LibWindow::~LibWindow()
{
    delete ui;
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
    QString libname_extended = libname;
    if (currdir.exists(libname_extended)) {
        for (int suffix = 0; suffix < 100; suffix++) {
            libname_extended = QString("%1%2%3").arg(base).arg(suffix, 2, 10, QChar('0')).arg(ext);
            if (!currdir.exists(libname_extended))
                break;
        }
    }
    QString fullpath = currdir.filePath(libname_extended);

    log("LibWindow: file new lib");
    m_pCore->newLib(fullpath.toStdString());
}
void LibWindow::fileOpenLib() {
    assert(m_pCore);
//    QString qfilename = QFileDialog::getOpenFileName(this,
//        tr("Open Library"), ".", tr("Any (*);;Library files (*.SchLib *.db)"));
//    if (qfilename.size()) // Don't open an empty file
//        m_pCore->openLib(qfilename.toStdString());



    QFileDialog qfd(this);
    qfd.setFileMode(QFileDialog::ExistingFiles);
    qfd.setNameFilter("Any (*);;Library files (*.SchLib *.db)");
    QStringList filenames;
    if(qfd.exec()) {
        filenames = qfd.selectedFiles();
        for (auto f : filenames) {
            m_pCore->openLib(f.toStdString());
        }
    }


}
void LibWindow::fileSaveLib() {
    assert(m_pCore);
    log("LibWindow: file save lib");
}
void LibWindow::fileSaveLibAs() {
    assert(m_pCore);
    log("LibWindow: file save lib as");
}
void LibWindow::fileCloseLib() {
    assert(m_pCore);
    if (m_pCore->activeDb()) {
        log("LibWindow: file close lib " + m_pCore->activeDb().value());
        m_pCore->closeActiveLib();
    }
}
void LibWindow::fileDeleteLib() {
    assert(m_pCore);
    if (m_pCore->activeDb()) {
        log("LibWindow: file delete lib " + m_pCore->activeDb().value());
        m_pCore->deleteActiveLib();
    }
}
void LibWindow::newShape() {
    assert(m_pCore);
    log("LibWindow: new shape");
}
void LibWindow::insertShape() {
    assert(m_pCore);
    log("LibWindow: insert shape");
}
void LibWindow::newSymbol() {
    assert(m_pCore);
    log("LibWindow: new symbol");
}
void LibWindow::deleteItem() {
    assert(m_pCore);
    log("LibWindow: delete item");
}
void LibWindow::newRectangle() {   
     assert(m_pCore);
    log("LibWindow: new rectangle");
}

void LibWindow::newLine() {
    assert(m_pCore);
    log("LibWindow: new line");
}
void LibWindow::newText() {
    assert(m_pCore);
    log("LibWindow: new text");
}
void LibWindow::newCircle() {
    assert(m_pCore);
    log("LibWindow: new circle");
}
void LibWindow::newArrow() {
    assert(m_pCore);
    log("LibWindow: new arrow");
}
void LibWindow::newPolygon() {
    assert(m_pCore);
    log("LibWindow: new polygon");
}
void LibWindow::newPolyline() {
    assert(m_pCore);
    log("LibWindow: new polyline");
}
void LibWindow::newPin() {
    assert(m_pCore);
    log("LibWindow: new pin");
}
void LibWindow::editUndo() {
    assert(m_pCore);
    log("LibWindow: edit undo");
}
void LibWindow::editRedo() {
    assert(m_pCore);
    log("LibWindow: edit redo");
}
void LibWindow::editMove() {
    assert(m_pCore);
    log("LibWindow: edit move");
}
void LibWindow::editRotate() {
    assert(m_pCore);
    log("LibWindow: edit rotate");
}
void LibWindow::editCut() {
    assert(m_pCore);
    log("LibWindow: edit cut");
}
void LibWindow::editCopy() {
    assert(m_pCore);
    log("LibWindow: edit copy");
}
void LibWindow::editPaste() {
    assert(m_pCore);
    log("LibWindow: edit paste");
}
void LibWindow::editDelete() {
    assert(m_pCore);
    log("LibWindow: edit delete item from " + m_pCore->activeDb().value());
    m_pCore->deleteShape("Some shape");
    m_pCore->deleteSymbol("Some symbol");
}
void LibWindow::viewLibTreeView() {
    assert(m_pCore);
    log("LibWindow: view LibTreeView");

}
void LibWindow::viewLibTableView() {

}

void LibWindow::helpAbout() {}

