#ifndef LIBWINDOW_H
#define LIBWINDOW_H

#include "libcore.h"
#include "coreable.h"
#include "loggable.h"

#include <QMainWindow>
#include <QString>

#include <any>


QT_BEGIN_NAMESPACE
namespace Ui { class LibWindow; }
QT_END_NAMESPACE

class LibWindow : public QMainWindow, public Coreable, public Loggable
{
    Q_OBJECT
private:
    Ui::LibWindow *ui;
//    void _duplicateWithOptions(LibCore::DupOptions);

public:
    LibWindow(QWidget *parent = nullptr);
    ~LibWindow() override;
    void updateActions();
    void fileNewLib();
    void fileOpenLib();
    void fileSaveAs();
//    void fileSaveAsAndCloseOld();
//    void fileSaveAsAndOpenNew();
//    void fileSaveAsQuietly();
    void fileRename();
    void fileCloseLib();
    void fileDeleteLib();
    void newShape();
    void insertShape();
    void deleteItem();
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
    void viewLibTreeView();
    void viewLibTableView();
    void helpAbout();
    void reloadStyle();




};
#endif // LIBWINDOW_H
