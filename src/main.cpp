#include "libcore.h"
#include "libwindow.h"
#include "logger.h"
#include "dbif.h"


#include <QApplication>
#include <QFrame>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QDockWidget>

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);

    LibWindow libwin;
    LibCore core;
    DbIf dbif;

    // Set up logger and its log window
    QDockWidget* plogw = new QDockWidget;
    QTextEdit* pte = new QTextEdit;
    Logger logger(pte);
    plogw->setWindowTitle("Log");
    plogw->setWidget(logger.textEdit());
    core.setLogger(&logger);
    dbif.setLogger(&logger);
    libwin.setLogger(&logger);
    libwin.addDockWidget(Qt::DockWidgetArea::BottomDockWidgetArea, plogw);
    
    app.setStyleSheet("QTextEdit {background: qlineargradient(x1:0 y1:0, x2:0 y2:1, "
                      "                                       stop:0 rgba(255, 255, 255, 255), "
                      "                                       stop:1 rgba(235, 235, 235, 255));}");

    core.setDbIf(&dbif);
    libwin.setCore(&core);
    libwin.show();
    return app.exec();
}
