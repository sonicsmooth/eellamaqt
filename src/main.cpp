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

    // Set up style
    QFile style(":/ui/llamastyle.css");
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

//    std::map<std::string, std::list<int>> junk;
//    junk["cat"].push_back(10);
//    junk["cat"].push_back(20);
//    junk["cat"].push_back(30);
//    junk["dog"].push_back(50);
//    junk["dog"].push_back(60);
//    junk["dog"].push_back(70);



    // Go!
     return app.exec();
}
