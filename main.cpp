#include "libcore.h"
#include "libwindow.h"
#include "logger.h"


#include <QApplication>
#include <QFrame>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QDockWidget>
/*
"qlineargradient(spread:pad, x1:0 y1:0, x2:1 y2:0, "
"                          stop:0 rgba(20, 0, 200, 255), "
"                          stop:1 rgba(255, 255, 255, 255));"
*/

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);


    LibWindow w;
    LibCore core;

    QDockWidget* plogw = new QDockWidget;
    QTextEdit* pte = new QTextEdit;
    Logger logger(pte);

    core.setLogger(&logger);
    plogw->setWindowTitle("Log");
    plogw->setWidget(logger.textEdit());
    w.setStyleSheet("QTextEdit {background: qlineargradient(x1:0 y1:0, x2:0 y2:1, "
                    "                               stop:0 rgba(255, 255, 255, 255), "
                    "                               stop:1 rgba(235, 235, 235, 255));}"
                    "QDockWidget {color: slate;"
                    "             background: teal;}");

    w.addDockWidget(Qt::DockWidgetArea::BottomDockWidgetArea, plogw);
    w.setCore(&core);
    w.show();
    return a.exec();
}
