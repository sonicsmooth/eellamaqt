#include "libwindow.h"
#include "libtreeview.h"
#include "ui_libwindow.h"
#include "closingdockwidget.h"
#include "uimanager.h"
#include "filesaveas.h"
#include "libcore.h"

#include <iostream>
#include <cstdio>
#include <sstream>
#include <cassert>
#include <any>

#include <QEvent>
#include <QStatusBar>
#include <QPushButton>
#include <QAction>
#include <QFileDialog>
#include <QDir>
#include <QTreeView>
#include <QTableView>
#include <QTableWidget>
#include <QDockWidget>
#include <QtSql>
#include <QInputDialog>
#include <QObjectList>
#include <QCommonStyle>




LibWindow::LibWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::LibWindow)
{
    ui->setupUi(this);

    updateTitle();

    QPushButton *pb = new QPushButton(this);
    pb->setText("Push me");
    //connect(pb, &QPushButton::clicked,[=](){statusBar()->showMessage("hi", 1000);});
    connect(pb, &QPushButton::clicked,[=](){m_pCore->newShape();});
    statusBar()->showMessage("The message!", 1000);
    statusBar()->addPermanentWidget(pb);
    
    connect(ui->actionFileNewLib, &QAction::triggered, this, &LibWindow::fileNewLib);
    connect(ui->actionFileOpenLib, &QAction::triggered, this, &LibWindow::fileOpenLib);
    connect(ui->actionFileSaveAs, &QAction::triggered, this, &LibWindow::fileSaveAs);
    connect(ui->actionFileRename, &QAction::triggered, this, &LibWindow::fileRename);
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
    connect(ui->actionDuplicateSymbolView, &QAction::triggered, this, &LibWindow::viewNewSymbolView);
    connect(ui->actionCloseSymbolView, &QAction::triggered, this, &LibWindow::viewCloseSymbolView);
    connect(ui->actionLibTreeView, &QAction::triggered, this, &LibWindow::viewLibTreeView);
    connect(ui->actionLibTableView, &QAction::triggered, this, &LibWindow::viewLibTableView);
    connect(ui->actionNewWindow, &QAction::triggered, this, &LibWindow::windowNewWindow);
    connect(ui->actionCloseWindow, &QAction::triggered, this, &LibWindow::windowCloseWindow);
    connect(ui->actionHelpAbout, &QAction::triggered, this, &LibWindow::helpAbout);
    connect(ui->actionReloadStyle, &QAction::triggered, this, &LibWindow::reloadStyle);

    // This handles focusing on the dockwidgets and their content widgets
    connect(qApp, &QApplication::focusChanged, [&](QWidget* old, QWidget* now) {
        // The goal is to get to the LibTreeWidget which has the dbConn.
        // But the LibTreeWidget doesn't receive focus events; only the QTreeView
        // and the ClosingDockWidget get focus events.  In the first case the
        // LibTreeWidget is the parent of the QTreeView.  In the second case the
        // LibTreeWidget is the content widget of the ClosingDockWidget.  We don't
        // know which one we get here unless we try to cast it.

        // ClosingDockWidget
        // --- LibViewWidget (dbConn)
        // ------ QTreeView

        // ClosingDockWidget
        // --- LibViewWidget (no data)
        // ------ QTableView


        (void) old;
        QDockWidget *qdw(dynamic_cast<QDockWidget *>(now));
        LibClient *libc(dynamic_cast<LibClient *>(now));
        //QTreeView *qtreev(dynamic_cast<QTreeView *>(now));
        //QTableView *qtablev(dynamic_cast<QTableView *>(now));

        if (qdw) {
            libc = dynamic_cast<LibClient *>(qdw->widget());
        }/* else if (qtreev) {
            lvc = dynamic_cast<LibViewClient *>(qtreev->parentWidget());
        } else if (qtablev) {
            lvc = dynamic_cast<LibViewClient *>(qtablev->parentWidget());
        } */

        if (libc) {
//            m_pCore->pushActiveDb(libc->dbConn());
        }

        // Might not assign lvw since there are other things getting focus
    });

    //ui->mdiArea->addSubWindow(new QTextEdit());


}


LibWindow::~LibWindow()
{
    delete ui;
}

void LibWindow::updateActions(bool en) {
    ui->actionFileRename->setEnabled(en);
    ui->actionFileSaveAs->setEnabled(en);
    ui->actionFileCloseLib->setEnabled(en);
    ui->actionFileDeleteLib->setEnabled(en);
    ui->actionEditCut->setEnabled(en);
    ui->actionEditCopy->setEnabled(en);
    ui->actionEditPaste->setEnabled(en);
    ui->actionEditMove->setEnabled(en);
    ui->actionEditRedo->setEnabled(en);
    ui->actionEditUndo->setEnabled(en);
    ui->actionEditDelete->setEnabled(en);
    ui->actionEditRotate->setEnabled(en);
    ui->actionNewPin->setEnabled(en);
    ui->actionNewLine->setEnabled(en);
    ui->actionNewText->setEnabled(en);
    ui->actionNewArrow->setEnabled(en);
    ui->actionNewShape->setEnabled(en);
    ui->actionInsertShape->setEnabled(en);
    ui->actionNewCircle->setEnabled(en);
    ui->actionNewSymbol->setEnabled(en);
    ui->actionNewPolygon->setEnabled(en);
    ui->actionNewPolyline->setEnabled(en);
    ui->actionNewRectangle->setEnabled(en);
    ui->actionDuplicateSymbolView->setEnabled(en);
    ui->actionCloseSymbolView->setEnabled(en);
    ui->actionLibTreeView->setEnabled(en);
    ui->actionLibTableView->setEnabled(en);
}
void LibWindow::updateTitle() {
    setWindowTitle("Library Editor");
}
void LibWindow::updateTitle(std::string title) {
    setWindowTitle(QString::fromStdString(title));
}
QMdiArea *LibWindow::mdiArea() {
    return ui->mdiArea;
}


void LibWindow::fileNewLib() {
    // Check existing fies and add suffix if needed.  Finds first "open" spot between 00 and 99, inclusive
    assert(m_pCore);
    assert(m_pLogger);

    // Ensure directory exists ~/EELlama Libraries
    QDir currdir;
    QDir home(QDir::home());
    if (!home.exists(GLibDir))
        home.mkdir(GLibDir);
    currdir.setPath(home.filePath(GLibDir));
    assert(currdir.exists());

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

    //log("LibWindow::fileNewLib");
    m_pCore->newLib(fullpath.toStdString());
    updateActions(m_pCore->DbIf()->isDatabaseOpen());
}
void LibWindow::fileOpenLib() {
    assert(m_pCore);
    QDir home(QDir::home());
    QFileDialog qfd(this);
    // Set to /home/eellama libraries if it exists else home
    qfd.setDirectory(home.exists(GLibDir) ? home.filePath(GLibDir) : home);
    qfd.setFileMode(QFileDialog::ExistingFiles);
    qfd.setNameFilter("Any (*);;Library files (*.SchLib *.db)");

    if(qfd.exec()) {
        QApplication::setActiveWindow(this); // shouldn't be necessary
        for (QString f : qfd.selectedFiles())
            m_pCore->openLib(f.toStdString());
    }
    updateActions(m_pCore->DbIf()->isDatabaseOpen());
}

void LibWindow::fileSaveAs() {
    //  Choose options, then call core
    assert(m_pCore);
    assert(m_pCore->DbIf()->isDatabaseOpen());
    FileSaveAs fsa(this, m_pCore->DbIf()->activeDatabase().value(),
                   static_cast<Logger *>(m_pLogger), FileSaveAs::Mode::SAVEAS);
    if(fsa.exec()) {
        std::string fname(fsa.fileName().toStdString());
        if (fname.length()) {
            m_pCore->saveLib(m_pCore->DbIf()->activeDatabase().value(), fname, fsa.option());
        }
    } else {
        log("LibWindow::fileSaveAs: Canceled");
    }
    updateActions(m_pCore->DbIf()->isDatabaseOpen());
}

void LibWindow::fileRename() {
    // Rename -- effectively fileSaveAsAndClose followed by deleting old one,
    // or closing, renaming, then opening again
    assert(m_pCore);
    assert(m_pCore->DbIf()->isDatabaseOpen());
    //log("LibWindow::fileRename");
    FileSaveAs frn(this, m_pCore->DbIf()->activeDatabase().value(),
                   static_cast<Logger *>(m_pLogger), FileSaveAs::Mode::RENAME);
    if(frn.exec()) {
        std::string fname(frn.fileName().toStdString());
        if (fname.length()) {
            auto option = frn.option();
            m_pCore->saveLib(m_pCore->DbIf()->activeDatabase().value(), fname, option);
        }
    } else {
        log("LibWindow::fileRename: Canceled");
    }
    updateActions(m_pCore->DbIf()->isDatabaseOpen());
}

void LibWindow::fileCloseLib() {
    assert(m_pCore);
    if (m_pCore->DbIf()->isDatabaseOpen()) {
        m_pCore->closeActiveLib();
    }
    updateActions(m_pCore->DbIf()->isDatabaseOpen());
}
void LibWindow::fileDeleteLib() {
    assert(m_pCore);
    if (m_pCore->DbIf()->isDatabaseOpen()) {
        m_pCore->deleteActiveLib();
    }
    updateActions(m_pCore->DbIf()->isDatabaseOpen());
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
//    log("LibWindow: edit delete item from " + m_pCore->activeDb().value());
//    m_pCore->deleteShape("Some shape");
//    m_pCore->deleteSymbol("Some symbol");
}

void LibWindow::viewNewSymbolView() {
    log("New Symbol View");
    assert(m_pCore);
    UIManager *uim(static_cast<UIManager *>(m_pCore->UIManager()));
    uim->duplicateSymbolView();
}
void LibWindow::viewCloseSymbolView() {
    log("Close Symbol View");
}

void LibWindow::viewLibTreeView() {
    assert(m_pCore);
//    log("LibWindow: view LibTreeView");
//    if (m_pCore->activeDb()) {
//        UIManager *mgr = dynamic_cast<UIManager *>(m_pCore->UIManager());
//        //mgr->openUI(m_pCore->activeDb().value(), UIType::LIBTREEVIEW);
//    }
}
void LibWindow::viewLibTableView() {
    assert(m_pCore);
//    log("LibWindow: view LibTableView");
//    if (m_pCore->activeDb()) {
//        UIManager *mgr = dynamic_cast<UIManager *>(m_pCore->UIManager());
//        //mgr->openUI(m_pCore->activeDb().value(), UIType::LIBTABLEVIEW);
//    }
}

void LibWindow::windowNewWindow() {
    assert(m_pCore);
    m_pCore->UIManager()->newWindow();
    static_cast<UIManager *>(m_pCore->UIManager())->mainWindows().back()->show();
}
void LibWindow::windowCloseWindow() {
    assert(m_pCore);
    m_pCore->UIManager()->closeWindow();
}


void LibWindow::helpAbout() {

}
void LibWindow::reloadStyle() {
    // Set up style -- this only works at startup, not after!!
    QFile styleFile(":/ui/llamastyle.css");
    styleFile.open(QIODevice::ReadOnly);
    QString fileContents = styleFile.readAll();

    QApplication *qapp(dynamic_cast<QApplication *>(QApplication::instance()));
    QApplication::setStyle("WindowsVista");
    qapp->setStyleSheet(fileContents);



    // Nothing after this point works to update stylesheets after initialization
//    this->setStyleSheet(fileContents);
//    this->style()->unpolish(this);
//    this->style()->polish(this);
//    this->update();

//    auto ch = findChildren<QWidget *>();
//     //QApplication *app = dynamic_cast<QApplication *>(QApplication::instance());
//     //QCommonStyle *style = dynamic_cast<QCommonStyle *>(app->style());
//     for (QWidget *child : ch) {
//        // child->style()->unpolish(child);
//        child->setStyleSheet(fileContents);
//        child->style()->unpolish(child);
//        child->style()->polish(child);
//        child->update();
//     }

}
void LibWindow::changeEvent(QEvent *e) {
    //if(!m_pLogger) return;
    if (e->type() == QEvent::WindowActivate && isActiveWindow())
        emit activated();
    // std::string s(isActiveWindow() ? "active" : "not active");
    // auto et(e->type());
    // switch(et) {
    //     case QEvent::ActivationChange:
    //         log("Activation change: %x %s", this, s.c_str());
    //         break;
    //     case QEvent::WindowActivate:
    //         log("Window activate %x %s", this, s.c_str());
    //         break;
    //     default:
    //             log("event %d", et);
    // }

}

