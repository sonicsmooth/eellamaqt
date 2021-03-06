#include "libcore.h"
#include "docwindow.h"
#include "logger.h"
#include "qdbif.h"
#include "uimanager.h"
#include "utils.h"
#include "allicons.h"
#include "mymodel.h"


#include <QApplication>
#include <QTextEdit>
#include <QDebug>

#include <functional>


void testfun(std::string s) {
    qDebug("std::string");
}
void testfun(const std::function<void(int)> & f) {
    qDebug("F f");

}

int main(int argc, char *argv[])
{
    int x = 5;
    testfun("hi");
    testfun([x](int y){qDebug() << "inside" << x;});

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
    DocWindow *recentMainWindow(static_cast<DocWindow *>(mainWindows.back()));
    recentMainWindow->addDockWidget(Qt::DockWidgetArea::BottomDockWidgetArea, pLogWidget);
    recentMainWindow->reloadStyle();
    recentMainWindow->show();

    // Set up core and ui slave
    core.setDbIf(&dbif);
    core.setUIManager(&uim);
    core.setLogger(&logger);

//    QApplication::processEvents();

    // Debug test
//    core.openLib("C:\\Users\\mschw\\EELLama Libraries\\NewLibrary.SchLib");
//    LibWindow *w = static_cast<LibWindow *>(uim.newWindow());
//    w->show();
//    QApplication::processEvents();
//    core.openLib("C:\\Users\\mschw\\EELLama Libraries\\NewLibrary09.SchLib");


    // Go!
     return app.exec();
}
