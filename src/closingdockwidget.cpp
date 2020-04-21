#include "closingdockwidget.h"

#include <iostream>

ClosingDockWidget::ClosingDockWidget(QWidget *parent) : QDockWidget(parent)
{

}

void ClosingDockWidget::closeEvent(QCloseEvent *event) {
    // The receiver gets a pointer to the just-now-closing dock widget
    // Please don't do anything dumb with the pointer.  Mostly it's
    // for looking up in a table to see if anything else needs to be done
    emit closing(this);
    QDockWidget::closeEvent(event);
}

//void ClosingDockWidget::retarget(std::string *conn) {
//    setWindowTitle(QString::fromStdString(*conn));
//}

