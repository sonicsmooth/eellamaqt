
#include "uimanager.h"
#include "closingdockwidget.h"
#include <QObject>
#include <QString>
#include <typeinfo>
#include <iterator>

UIManager::UIManager(QObject *parent) : QObject(parent)
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
    libTreeWidget->setCore(m_pCore);
    libTreeWidget->setLogger(m_pLogger);
    libTreeWidget->setDbConn(title.toStdString());

    ClosingDockWidget* libDockWidget = new ClosingDockWidget(parentMW());
    libTreeWidget->setFocusProxy(libDockWidget); // doesn't seem to do anything
    libDockWidget->setFocusPolicy(Qt::StrongFocus);
    libDockWidget->setWidget(libTreeWidget);
    libDockWidget->setWindowTitle(title);

    // Send close signal somewhere; this catches when a dockwidget is closed
    // If all dockwindows associated with a particular connection string
    // are closed then this UIManager requests to the core to close the library.
    QObject::connect(libDockWidget, &ClosingDockWidget::closing, this, &UIManager::OnWidgetClose);

    parentMW()->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, libDockWidget);
    return libDockWidget;
}

QWidget *findValue(std::map<QWidget *, std::string> m, std::string value) {
    // finds key/val pair where val == value and returns key
    for (auto it = m.begin(); it != m.end(); it++)
        if (it->second == value)
            return it->first;
    return nullptr;
}


std::any UIManager::OpenUI(UITYPE uit, std::string title) {
    // Ensures only one of any type of view/window is opened
    // Returns pointer if one already exists, otherwise creates new one
    if(uit == UITYPE::LIBVIEW) {
        log("UIManager::OpenUI" + title);
        QWidget *pw = findValue(m_openWidgets, title);
        if (!pw) {
            pw = openLibTreeView(QString::fromStdString(title), "sometooltip");
            m_openWidgets[pw] = title;
        }
        return pw;
    } else
        return nullptr;
}
void UIManager::CloseUI(std::string title) {
    // Intended to be called from Core, which is is not aware of UIs very much.
    // When called with a given string, this fn will close all UI windows
    // associated with that string
    log("UIManager::CloseUI " + title);
    int n = 0;

    // Can't use for loop because body modifies m_openWidgets
    QWidget *pw(nullptr);
    while ((pw = findValue(m_openWidgets, title))) {
        pw->close();
        n++;
    }
    log("Closed %d windows", n);
}

void UIManager::OnWidgetClose(QWidget *pw) {
    // Remove the widget from the open widgets map
    assert(m_openWidgets.count(pw));
    std::string widgetString(m_openWidgets[pw]);
    m_openWidgets.erase(pw);

    // Check if any more widgets with the same associated name
    // are still open.  If there are none, then we ask core to close the library
    // So in effect we are counting references to the library
    size_t n = m_openWidgets.count(pw);
    if (n == 0) {
        log("Last reference to %s window closed", widgetString.c_str());
        m_pCore->closeLib(widgetString);
    } else {
        log("%d references to %s", n, widgetString.c_str());
    }
}
