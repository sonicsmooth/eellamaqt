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

    LibWindow w;
    LibCore core;

    QDockWidget* plogw = new QDockWidget;
    QTextEdit* pte = new QTextEdit;
    Logger logger(pte);
    DbIf dbif;

    core.setLogger(&logger);
    core.setDbIf(&dbif);
    plogw->setWindowTitle("Log");
    plogw->setWidget(logger.textEdit());
    app.setStyleSheet("QTextEdit {background: qlineargradient(x1:0 y1:0, x2:0 y2:1, "
                      "                                       stop:0 rgba(255, 255, 255, 255), "
                      "                                       stop:1 rgba(235, 235, 235, 255));}"
                    );

    w.addDockWidget(Qt::DockWidgetArea::BottomDockWidgetArea, plogw);
    w.setCore(&core);
    w.show();
    return app.exec();
}
