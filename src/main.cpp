#include "libcore.h"
#include "libwindow.h"
#include "logger.h"
#include "dbif.h"
#include "uimanager.h"


#include <QApplication>
#include <QFrame>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QDockWidget>

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);

    app.setStyleSheet("QTextEdit {background: qlineargradient(x1:0 y1:0, x2:0 y2:1, "
                      "                                       stop:0 rgba(255, 255, 255, 255), "
                      "                                       stop:1 rgba(235, 235, 235, 255));}");

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
