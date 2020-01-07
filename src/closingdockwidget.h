#ifndef LIBDOCKWIDGET_H
#define LIBDOCKWIDGET_H

#include <QDockWidget>
#include <QCloseEvent>
#include <QFocusEvent>

// Captures close event

class ClosingDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit ClosingDockWidget(QWidget * = nullptr);
    void closeEvent(QCloseEvent *) override;

signals:
    void closing();

public slots:
};

#endif // LIBDOCKWIDGET_H
