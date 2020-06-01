#ifndef CLOSINGDOCKWIDGET_H
#define CLOSINGDOCKWIDGET_H

#include <QDockWidget>
#include <QCloseEvent>
#include <QFocusEvent>

#include <string>

// Captures close event and sends closing() signal

class ClosingDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit ClosingDockWidget(QWidget * = nullptr);
    ~ClosingDockWidget() override;
    void closeEvent(QCloseEvent *) override;

signals:
    void *closing(ClosingDockWidget *) const;

public slots:
};

#endif // LIBDOCKWIDGET_H
