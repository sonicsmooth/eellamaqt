#include "libcore.h"
#include "libwindow.h"
#include "logger.h"
#include "qdbif.h"
#include "uimanager.h"
#include "utils.h"
#include "allicons.h"
#include "mymodel.h"


#include <QApplication>
#include <QFrame>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QDockWidget>
#include <QMessageBox>
#include <QTreeView>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QFileSystemModel>

int main(int argc, char *argv[])
{
    //QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    LibCore core;
    //LibWindow *pLibwin;
    QSQDbIf dbif;
    UIManager uim;
    //QModelManager qmm;
    //QViewManager qvm;

    // Show all default icons
    //AllIcons ai;
    //ai.show();


    // Set up logger and its log window
    Logger logger(new QTextEdit);
    QDockWidget* pLogWidget = new QDockWidget;
    pLogWidget->setWindowTitle("Log");
    pLogWidget->setWidget(logger.textEdit());

    // Set up database interface
    dbif.setLogger(&logger);

    // Set up ui slave
    uim.setCore(&core);
    uim.setLogger(&logger);
    uim.newWindow();
    std::list<QMainWindow *> mainWindows(uim.mainWindows());
    LibWindow *recentMainWindow(static_cast<LibWindow *>(mainWindows.back()));
    recentMainWindow->addDockWidget(Qt::DockWidgetArea::BottomDockWidgetArea, pLogWidget);
    recentMainWindow->reloadStyle();
    recentMainWindow->show();

    // Set up main window
    //libwin.setCore(&core);
    //libwin.setLogger(&logger);
    //libwin.show();

    // Set up core and ui slave
    core.setDbIf(&dbif);
    core.setUIManager(&uim);
    core.setLogger(&logger);

    // Go!
     return app.exec();
}
