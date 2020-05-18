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
#include <QFont>
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
    QSQDbIf dbif;
    UIManager uim;

    // Show all default icons
    //AllIcons ai;
    //ai.show();


    // Set up logger and its log window, and notify when textEdit is destroyed
    QTextEdit *pte = new QTextEdit();
    Logger logger(pte);
    QObject::connect(pte, &QTextEdit::destroyed, &logger, &Logger::nullifyLogger);
    pte->setFontFamily("Consolas");

    ClosingDockWidget* pLogWidget = new ClosingDockWidget;
    pLogWidget->setAttribute(Qt::WA_DeleteOnClose, true);
    pLogWidget->setWindowTitle("Log");
    pLogWidget->setWidget(pte); // pLogWidget owns textEdit


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

    // Set up core and ui slave
    core.setDbIf(&dbif);
    core.setUIManager(&uim);
    core.setLogger(&logger);

    // Go!
     return app.exec();
}
