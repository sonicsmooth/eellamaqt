#include "closingmdiwidget.h"

ClosingMDIWidget::ClosingMDIWidget(QWidget *parent) :
    QMdiSubWindow(parent)
{

}

void ClosingMDIWidget::closeEvent(QCloseEvent *event) {
    // The receiver gets a pointer to the just-now-closing MDI widget
    // Please don't do anything dumb with the pointer.  Mostly it's
    // for looking up in a table to see if anything else needs to be done
    emit closing(this);
    QMdiSubWindow::closeEvent(event);

}

//void ClosingMDIWidget::retarget(std::string *conn) {
//    setWindowTitle(QString::fromStdString(*conn));
//}

