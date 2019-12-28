#ifndef LIBWINDOW_H
#define LIBWINDOW_H

#include "libcore.h"

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
    void setCore(LibCore* pc);
    void newLib(bool);
    void openLib(bool);
    void closeLib(bool);
    void deleteLib(bool);
    void newLibShape(bool);
    void newLibSymbol(bool);
    void deleteSelected(bool);

private:
    LibCore* m_pCore;
    Ui::LibWindow *ui;



};
#endif // LIBWINDOW_H
