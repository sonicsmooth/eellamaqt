
#include "uimanager.h"
#include "closingdockwidget.h"
#include <QObject>
#include <QString>

UIManager::UIManager()
{

}
void UIManager::setParentMW(QMainWindow *p) {
    m_parentMW = p;
}
QMainWindow *UIManager::parentMW() const {
    return m_parentMW;
}

ClosingDockWidget *UIManager::openLibTreeView(QString title, QString tooltip) {
    LibTreeWidget* libTreeWidget = new LibTreeWidget(parentMW());
    libTreeWidget->setCore(core());
    libTreeWidget->setLogger(logger());
    libTreeWidget->setDbConn(title.toStdString());

    ClosingDockWidget* libDockWidget = new ClosingDockWidget(parentMW());
    libTreeWidget->setFocusProxy(libDockWidget); // doesn't seem to do anything
    libDockWidget->setFocusPolicy(Qt::StrongFocus);
    libDockWidget->setWidget(libTreeWidget);
    libDockWidget->setWindowTitle(title);

    // Send close signal somewhere
    QObject::connect(libDockWidget, &ClosingDockWidget::closing, [=](){log("Closed lambda");});

    parentMW()->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, libDockWidget);
    return libDockWidget;
}
std::any UIManager::OpenUI(UITYPE uit, std::string title) {
    if(uit == UITYPE::LIBVIEW) {
        log("UIManager::OpenUI" + title);
        if (m_openWidgets.find(title) == m_openWidgets.end()) { // not found
            QWidget *w = openLibTreeView(QString::fromStdString(title), "sometooltip");
            m_openWidgets[title] = w;
        }
        return m_openWidgets[title];
    } else
        return nullptr;
}
void UIManager::CloseUI(std::string title) {
    log("UIManager::CloseUI " + title);
    QWidget *w = m_openWidgets.find(title)->second;
    w->close();

}
