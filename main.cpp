#include "libwindow.h"
#include "logger.h"

#include <QApplication>
#include <QFrame>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QDockWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LibWindow w;
    LibEdCore core;

    QDockWidget* plogw = new QDockWidget;
    QTextEdit* pte = new QTextEdit;
    Logger logger(pte);

    core.setLogger(&logger);

    plogw->setWindowTitle("Log");
    plogw->setWidget(logger.textEdit());


    w.addDockWidget(Qt::DockWidgetArea::BottomDockWidgetArea, plogw);
    w.setCore(&core);
    w.show();
    return a.exec();
}
