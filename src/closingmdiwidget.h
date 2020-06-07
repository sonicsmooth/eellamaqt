#ifndef CLOSINGMDIWIDGET_H
#define CLOSINGMDIWIDGET_H

#include <QMdiSubWindow>
#include <QCloseEvent>
#include <QFocusEvent>

#include <string>

// Captures close event and sends closing() signal

class ClosingMDIWidget : public QMdiSubWindow
{
    Q_OBJECT
public:
    explicit ClosingMDIWidget(QWidget * = nullptr);
    void closeEvent(QCloseEvent *) override;
//    void retarget(std::string *);

signals:
    void closing(ClosingMDIWidget *) const;

public slots:
};

#endif // CLOSINGMDIWIDGET_H
