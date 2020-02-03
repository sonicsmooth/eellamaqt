#include "libcore.h"
#include "libwindow.h"
#include "logger.h"
#include "dbif.h"
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
    LibWindow libwin;
    QSQDbIf dbif;
    UIManager uim;

    // Show all default icons
    //AllIcons ai;
    //ai.show();

    // Set up style
    libwin.reloadStyle();

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

    // Setup fake window
    auto *mw = new QWidget;
    auto *hb = new QHBoxLayout;
    auto *qtv = new QTreeView;
    auto *fsm = new QFileSystemModel;
//    auto *sim = new QStandardItemModel;
//    auto root = sim->invisibleRootItem();
//    root->appendRow(new QStandardItem("Americas"));
//    root->child(0)->appendRow(new QStandardItem("Canada"));
//    root->child(0)->child(0)->appendRow(new QStandardItem("Calgary"));
//    root->child(0)->child(0)->appendRow(new QStandardItem("Montreal"));
//    root->child(0)->appendRow(new QStandardItem("USA"));
//    root->child(0)->child(1)->appendRow(new QStandardItem("Boston"));
//    root->child(0)->child(1)->appendRow(new QStandardItem("Seattle"));
//    root->appendRow(new QStandardItem("Europe"));
//    root->child(1)->appendRow(new QStandardItem("Italy"));
//    root->child(1)->child(0)->appendRow(new QStandardItem("Rome"));
//    root->child(1)->child(0)->appendRow(new QStandardItem("Verona"));
//    root->child(1)->appendRow(new QStandardItem("Germany"));
//    root->child(1)->child(1)->appendRow(new QStandardItem("Berlin"));
//    root->child(1)->child(1)->appendRow(new QStandardItem("Stuttgart"));
//    root->child(1)->appendRow(new QStandardItem("France"));
//    root->child(1)->child(2)->appendRow(new QStandardItem("Paris"));
//    root->child(1)->child(2)->appendRow(new QStandardItem("Marseilles"));
//    root->child(1)->appendRow(new QStandardItem("Netherlands"));
//    root->child(1)->child(3)->appendRow(new QStandardItem("Amtsterdam"));
//    root->child(1)->child(3)->appendRow(new QStandardItem("Coffee shop"));


    fsm->setRootPath(QDir::currentPath());
    qtv->setModel(fsm);


    //qtv->setModel(sim);
    //qtv->expandAll();
//    hb->addWidget(new QPushButton("what?"));
//    hb->addWidget(qtv);
//    mw->setLayout(hb);
//    QItemSelectionModel *selectionModel = qtv->selectionModel();
//    QObject::connect(selectionModel, &QItemSelectionModel::selectionChanged,
//            [=](const QItemSelection & s, const QItemSelection & os) {qDebug("selection changed");});

//    mw->show();




    // Go!
     return app.exec();
}
