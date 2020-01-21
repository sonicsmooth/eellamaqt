#ifndef LIBDOCKWIDGET_H
#define LIBDOCKWIDGET_H

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
    void closeEvent(QCloseEvent *) override;
    void retarget(std::string *);

signals:
    void *closing(ClosingDockWidget *) const;

public slots:
};

#endif // LIBDOCKWIDGET_H
