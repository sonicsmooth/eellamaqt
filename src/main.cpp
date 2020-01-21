#include "libcore.h"
#include "libwindow.h"
#include "logger.h"
#include "dbif.h"
#include "uimanager.h"
#include "utils.h"


#include <QApplication>
#include <QFrame>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QDockWidget>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);

    // Set up style
    QFile style(":/ui/llamastyle.css");
    //QFile style(":/ui/darkstyle.css");
    style.open(QIODevice::ReadOnly);
    app.setStyleSheet(style.readAll());

    LibCore core;
    LibWindow libwin;
    QSQDbIf dbif;
    UIManager uim;


    // Set up logger and its log window
    Logger logger(new QTextEdit);
    QDockWidget* pLogWidget = new QDockWidget;
    pLogWidget->setWindowTitle("Log");
    pLogWidget->setWidget(logger.textEdit());

    // Set up databae interface
    dbif.setLogger(&logger);

    // Set up ui slave
    uim.setCore(&core);
    uim.setLogger(&logger);
    uim.setParentMW(&libwin);

    // Set up main window
    libwin.setCore(&core);
    libwin.setLogger(&logger);
    libwin.addDockWidget(Qt::DockWidgetArea::BottomDockWidgetArea, pLogWidget);
    libwin.show();

    // Set up core and ui slave
    core.setDbIf(&dbif);
    core.setUIManager(&uim);
    core.setLogger(&logger);


    // Go!
     return app.exec();
}
