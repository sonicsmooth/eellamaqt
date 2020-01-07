#include "closingdockwidget.h"

ClosingDockWidget::ClosingDockWidget(QWidget *parent) : QDockWidget(parent)
{

}

void ClosingDockWidget::closeEvent(QCloseEvent *event) {
    emit closing();
    QDockWidget::closeEvent(event);

}
//void CLosingDockWidget::focusInEvent(QFocusEvent *event) {
//    (void) event;
//    int a = 6;
//    qDebug("Custom focus event in LibDockWidget");
//    (void) a;
//}
