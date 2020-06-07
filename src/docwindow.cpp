#include "docwindow.h"
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
#include <QMdiArea>


DocWindow::DocWindow(QWidget *parent, LibCore *pcore, ILogger *plgr)
    : QMainWindow(parent),
      Coreable(pcore),
      Loggable(plgr),
      ui(new Ui::LibWindow)
{
    ui->setupUi(this);

    updateTitle();

    QPushButton *pb = new QPushButton(this);
    pb->setText("Push me");
    connect(pb, &QPushButton::clicked,[=](){statusBar()->showMessage("hi", 1000);});
    connect(pb, &QPushButton::clicked,[=](){m_pCore->newShape();});
    statusBar()->showMessage("The message!", 1000);
    statusBar()->addPermanentWidget(pb);

    connect(ui->actionFileNewLib, &QAction::triggered, this, &DocWindow::fileNewLib);
    connect(ui->actionFileOpenLib, &QAction::triggered, this, &DocWindow::fileOpenLib);
    connect(ui->actionFileSaveAs, &QAction::triggered, this, &DocWindow::fileSaveAs);
    connect(ui->actionFileRename, &QAction::triggered, this, &DocWindow::fileRename);
    connect(ui->actionFileCloseLib, &QAction::triggered, this, &DocWindow::fileCloseLib);
    connect(ui->actionFileDeleteLib, &QAction::triggered, this, &DocWindow::fileDeleteLib);
    connect(ui->actionNewShape, &QAction::triggered, this, &DocWindow::newShape);
    connect(ui->actionInsertShape, &QAction::triggered, this, &DocWindow::insertShape);
    connect(ui->actionNewSymbol, &QAction::triggered, this, &DocWindow::newSymbol);
    connect(ui->actionNewRectangle, &QAction::triggered, this, &DocWindow::newRectangle);
    connect(ui->actionNewLine, &QAction::triggered, this, &DocWindow::newLine);
    connect(ui->actionNewText, &QAction::triggered, this, &DocWindow::newText);
    connect(ui->actionNewCircle, &QAction::triggered, this, &DocWindow::newCircle);
    connect(ui->actionNewArrow, &QAction::triggered, this, &DocWindow::newArrow);
    connect(ui->actionNewPolygon, &QAction::triggered, this, &DocWindow::newPolygon);
    connect(ui->actionNewPolyline, &QAction::triggered, this, &DocWindow::newPolyline);
    connect(ui->actionNewPin, &QAction::triggered, this, &DocWindow::newPin);
    connect(ui->actionEditUndo, &QAction::triggered, this, &DocWindow::editUndo);
    connect(ui->actionEditRedo, &QAction::triggered, this, &DocWindow::editRedo);
    connect(ui->actionEditMove, &QAction::triggered, this, &DocWindow::editMove);
    connect(ui->actionEditRotate, &QAction::triggered, this, &DocWindow::editRotate);
    connect(ui->actionEditCut, &QAction::triggered, this, &DocWindow::editCut);
    connect(ui->actionEditCopy, &QAction::triggered, this, &DocWindow::editCopy);
    connect(ui->actionEditPaste, &QAction::triggered, this, &DocWindow::editPaste);
    connect(ui->actionEditDelete, &QAction::triggered, this, &DocWindow::editDelete);
    connect(ui->actionPopOutSymbolView, &QAction::triggered, this, &DocWindow::popOutMainView);
    connect(ui->actionDuplicateSymbolView, &QAction::triggered, this, &DocWindow::duplicateMainView);
    connect(ui->actionCloseSymbolView, &QAction::triggered, this, &DocWindow::closeMainView);
    connect(ui->actionLibTreeView, &QAction::triggered, this, &DocWindow::toggleLibTreeView);
    connect(ui->actionLibTableView, &QAction::triggered, this, &DocWindow::toggleLibTableView);
    connect(ui->actionNewWindow, &QAction::triggered, this, &DocWindow::newWindow);
    connect(ui->actionCloseWindow, &QAction::triggered, this, &DocWindow::closeWindow);
    connect(ui->actionHelpAbout, &QAction::triggered, this, &DocWindow::helpAbout);
    connect(ui->actionReloadStyle, &QAction::triggered, this, &DocWindow::reloadStyle);
    connect(ui->actionTabs, &QAction::triggered, this, &DocWindow::mdiTabMode);
    connect(ui->actionTile, &QAction::triggered, this, &DocWindow::mdiTileSubWindows);
    connect(ui->actionCascade, &QAction::triggered, this, &DocWindow::mdiCascadeSubWindows);

    updateLibActions(false);

    // This handles focusing on the dockwidgets and their content widgets
//     connect(qApp, &QApplication::focusChanged, [&](QWidget* old, QWidget* now) {
//         // The goal is to get to the LibTreeWidget which has the dbConn.
//         // But the LibTreeWidget doesn't receive focus events; only the QTreeView
//         // and the ClosingDockWidget get focus events.  In the first case the
//         // LibTreeWidget is the parent of the QTreeView.  In the second case the
//         // LibTreeWidget is the content widget of the ClosingDockWidget.  We don't
//         // know which one we get here unless we try to cast it.

//         // ClosingDockWidget
//         // --- LibViewWidget (dbConn)
//         // ------ QTreeView

//         // ClosingDockWidget
//         // --- LibViewWidget (no data)
//         // ------ QTableView


//         (void) old;
//         QDockWidget *qdw(dynamic_cast<QDockWidget *>(now));
//         LibClient *libc(dynamic_cast<LibClient *>(now));
//         //QTreeView *qtreev(dynamic_cast<QTreeView *>(now));
//         //QTableView *qtablev(dynamic_cast<QTableView *>(now));

//         if (qdw) {
//             libc = dynamic_cast<LibClient *>(qdw->widget());
//         }/* else if (qtreev) {
//             lvc = dynamic_cast<LibViewClient *>(qtreev->parentWidget());
//         } else if (qtablev) {
//             lvc = dynamic_cast<LibViewClient *>(qtablev->parentWidget());
//         } */

//         if (libc) {
// //            m_pCore->pushActiveDb(libc->dbConn());
//         }

//         // Might not assign lvw since there are other things getting focus
//     });

    //ui->mdiArea->addSubWindow(new QTextEdit());


}


DocWindow::~DocWindow()
{
    int z = 99;
    delete ui;
}

void DocWindow::updateLibActions(bool en) {
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
    ui->actionPopOutSymbolView->setEnabled(en);
    ui->actionDuplicateSymbolView->setEnabled(en);
    ui->actionCloseSymbolView->setEnabled(en);
    ui->actionLibTreeView->setEnabled(en);
    ui->actionLibTableView->setEnabled(en);
    ui->actionTabs->setEnabled(en);
    ui->actionCascade->setEnabled(en);
    ui->actionTile->setEnabled(en);
    updateTabActions();
}
void DocWindow::updateTabActions() {
    // Set tabbed/normal menu text based on current mode
    if (ui->mdiArea->viewMode() == QMdiArea::ViewMode::TabbedView)
        ui->actionTabs->setText("Show as subwindows");
    else
        ui->actionTabs->setText("Show as tabs");
}

void DocWindow::updateTitle() {
    setWindowTitle("Library Editor");
}
void DocWindow::updateTitle(std::string title) {
    setWindowTitle(QString::fromStdString(title));
}
QMdiArea *DocWindow::mdiArea() {
    return ui->mdiArea;
}


void DocWindow::fileNewLib() {
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

    m_pCore->newLib(fullpath.toStdString());
}
void DocWindow::fileOpenLib() {
    assert(m_pCore);
    QDir home(QDir::home());
    log("File Open from 0x%08x", this);
    QFileDialog qfd(this);
    // Set to /home/eellama libraries if it exists else home
    qfd.setDirectory(home.exists(GLibDir) ? home.filePath(GLibDir) : home);
    qfd.setFileMode(QFileDialog::ExistingFiles);
    qfd.setNameFilter("Any (*);;Library files (*.SchLib *.db)");

    if(qfd.exec()) {
        // qt appears to lose active window after file dialog
        // Appears related to https://www.qtcentre.org/threads/2950-ActiveWindow-changes-after-closing-QFileDialog
        QApplication::setActiveWindow(this);
        for (QString f : qfd.selectedFiles()) {
            m_pCore->openLib(f.toStdString());
            // this causes crash when opening multiple 
            // mdi widgets maximized.
            //QApplication::processEvents();
        }
    }
}

void DocWindow::fileSaveAs() {
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
}

void DocWindow::fileRename() {
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
}

void DocWindow::fileCloseLib() {
    assert(m_pCore);
    if (m_pCore->DbIf()->isDatabaseOpen()) {
        m_pCore->closeActiveLib();
    }
}
void DocWindow::fileDeleteLib() {
    assert(m_pCore);
    if (m_pCore->DbIf()->isDatabaseOpen()) {
        m_pCore->deleteActiveLib();
    }
}
void DocWindow::newShape() {
    assert(m_pCore);
    log("LibWindow: new shape");
}
void DocWindow::insertShape() {
    assert(m_pCore);
    log("LibWindow: insert shape");
}
void DocWindow::newSymbol() {
    assert(m_pCore);
    log("LibWindow: new symbol");
}
void DocWindow::deleteItem() {
    assert(m_pCore);
    log("LibWindow: delete item");
}
void DocWindow::newRectangle() {
     assert(m_pCore);
    log("LibWindow: new rectangle");
}

void DocWindow::newLine() {
    assert(m_pCore);
    log("LibWindow: new line");
}
void DocWindow::newText() {
    assert(m_pCore);
    log("LibWindow: new text");
}
void DocWindow::newCircle() {
    assert(m_pCore);
    log("LibWindow: new circle");
}
void DocWindow::newArrow() {
    assert(m_pCore);
    log("LibWindow: new arrow");
}
void DocWindow::newPolygon() {
    assert(m_pCore);
    log("LibWindow: new polygon");
}
void DocWindow::newPolyline() {
    assert(m_pCore);
    log("LibWindow: new polyline");
}
void DocWindow::newPin() {
    assert(m_pCore);
    log("LibWindow: new pin");
}
void DocWindow::editUndo() {
    assert(m_pCore);
    log("LibWindow: edit undo");
}
void DocWindow::editRedo() {
    assert(m_pCore);
    log("LibWindow: edit redo");
}
void DocWindow::editMove() {
    assert(m_pCore);
    log("LibWindow: edit move");
}
void DocWindow::editRotate() {
    assert(m_pCore);
    log("LibWindow: edit rotate");
}
void DocWindow::editCut() {
    assert(m_pCore);
    log("LibWindow: edit cut");
}
void DocWindow::editCopy() {
    assert(m_pCore);
    log("LibWindow: edit copy");
}
void DocWindow::editPaste() {
    assert(m_pCore);
    log("LibWindow: edit paste");
}
void DocWindow::editDelete() {
    assert(m_pCore);
//    log("LibWindow: edit delete item from " + m_pCore->activeDb().value());
//    m_pCore->deleteShape("Some shape");
//    m_pCore->deleteSymbol("Some symbol");
}

void DocWindow::duplicateMainView() {
    log("New Symbol View");
    assert(m_pCore);
    UIManager *uim(static_cast<UIManager *>(m_pCore->UIManager()));
    uim->duplicateMainView();
}
void DocWindow::popOutMainView() {
    // Initiates popOutView of current view widget
    assert(m_pCore);
    m_pCore->UIManager()->popOutMainView();
}
void DocWindow::closeMainView() {
    assert(m_pCore);
    m_pCore->UIManager()->closeMainView();
}

void DocWindow::toggleLibTreeView() {
    assert(m_pCore);
//    log("LibWindow: view LibTreeView");
//    if (m_pCore->activeDb()) {
//        UIManager *mgr = dynamic_cast<UIManager *>(m_pCore->UIManager());
//        //mgr->openUI(m_pCore->activeDb().value(), UIType::LIBTREEVIEW);
//    }
}
void DocWindow::toggleLibTableView() {
    assert(m_pCore);
//    log("LibWindow: view LibTableView");
//    if (m_pCore->activeDb()) {
//        UIManager *mgr = dynamic_cast<UIManager *>(m_pCore->UIManager());
//        //mgr->openUI(m_pCore->activeDb().value(), UIType::LIBTABLEVIEW);
//    }
}

void DocWindow::newWindow() {
    assert(m_pCore);
    QWidget *w(static_cast<QWidget *>(m_pCore->UIManager()->duplicateWindow()));
    w->show();
}
void DocWindow::closeWindow() {
    assert(m_pCore);
    m_pCore->UIManager()->closeWindow();
}


void DocWindow::helpAbout() {

}

void DocWindow::reloadStyle() {
    // Set up style -- this only works at startup, not after!!
    QFile styleFile(":/ui/llamastyle.css");
    styleFile.open(QIODevice::ReadOnly);
    QString fileContents = styleFile.readAll();

    QApplication *qapp(dynamic_cast<QApplication *>(QApplication::instance()));
    QApplication::setStyle("WindowsVista");
    qapp->setStyleSheet(fileContents);
}

void DocWindow::mdiTabMode() {
    // Toggles view mode
    if (ui->mdiArea->viewMode() == QMdiArea::ViewMode::TabbedView)
        ui->mdiArea->setViewMode(QMdiArea::ViewMode::SubWindowView);
    else
        ui->mdiArea->setViewMode(QMdiArea::ViewMode::TabbedView);
    updateTabActions();
}

void DocWindow::mdiTileSubWindows() {
    ui->mdiArea->setViewMode(QMdiArea::ViewMode::SubWindowView);
    ui->mdiArea->tileSubWindows();
    updateTabActions();
}
void DocWindow::mdiCascadeSubWindows() {
    ui->mdiArea->setViewMode(QMdiArea::ViewMode::SubWindowView);
    ui->mdiArea->cascadeSubWindows();
    updateTabActions();
}


void DocWindow::changeEvent(QEvent *event) {
//    if (m_pLogger)
//        static_cast<Logger *>(m_pLogger)->log(event);
    if (event->type() == QEvent::ActivationChange) {
        //log("Event activate");
        if (isActiveWindow()) {
            //log("  isActiveWindow true");
            emit activated(this);
        } else {
            //log(" isActiveWindow false");
        }
    }
    QMainWindow::changeEvent(event);
}

void DocWindow::closeEvent(QCloseEvent *event) {
    ui->mdiArea->closeAllSubWindows();
    emit closing(this);
    QMainWindow::closeEvent(event);

}

