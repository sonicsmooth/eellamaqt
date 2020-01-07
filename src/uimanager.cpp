
#include "uimanager.h"
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

QDockWidget *UIManager::openLibTreeView(QString title, QString tooltip) {
    LibTreeWidget* trLibItems = new LibTreeWidget(parentMW());
    trLibItems->setCore(core());
    trLibItems->setLogger(logger());
    trLibItems->setDbConn(title.toStdString());

    QDockWidget* dwLibItems = new QDockWidget(parentMW());
    dwLibItems->setFocusPolicy(Qt::StrongFocus);
    dwLibItems->setWidget(trLibItems);
    dwLibItems->setWindowTitle(title);

    parentMW()->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, dwLibItems);
    return dwLibItems;
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
