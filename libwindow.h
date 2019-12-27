#ifndef LIBWINDOW_H
#define LIBWINDOW_H

#include "libedcore.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class LibWindow; }
QT_END_NAMESPACE

class LibWindow : public QMainWindow
{
    Q_OBJECT

public:
    LibWindow(QWidget *parent = nullptr);
    ~LibWindow();
    void setCore(LibEdCore* pc);
    void newLibShape(bool);
    void newLibSymbol(bool);

private:
    LibEdCore* m_pCore;
    Ui::LibWindow *ui;



};
#endif // LIBWINDOW_H
