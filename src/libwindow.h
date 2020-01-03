#ifndef LIBWINDOW_H
#define LIBWINDOW_H

#include "libcore.h"
#include "logger.h"

#include <any>

#include <QMainWindow>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class LibWindow; }
QT_END_NAMESPACE

class LibWindow : public QMainWindow
{
    Q_OBJECT

private:
    LibCore* m_pCore;
    Logger* m_pLogger;
    std::any m_DbHandle;
    Ui::LibWindow *ui;
    void _openLibTreeView(QString);

public:
    LibWindow(QWidget *parent = nullptr);
    ~LibWindow();
    void setCore(LibCore*);
    void setLogger(Logger *);
    void setDbHandle(std::any);
    std::any DbHandle() const;

    void fileNewLib();
    void fileOpenLib();
    void fileSaveLib();
    void fileSaveLibAs();
    void fileCloseLib();
    void fileDeleteLib();
    void newShape();
    void insertShape();
    void newSymbol();
    void newRectangle();
    void newLine();
    void newText();
    void newCircle();
    void newArrow();
    void newPolygon();
    void newPolyline();
    void newPin();
    void editUndo();
    void editRedo();
    void editMove();
    void editRotate();
    void editCut();
    void editCopy();
    void editPaste();
    void editDelete();
    void helpAbout();




};
#endif // LIBWINDOW_H
