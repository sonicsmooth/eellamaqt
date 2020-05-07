#ifndef LIBWINDOW_H
#define LIBWINDOW_H

#include "libcore.h"
#include "coreable.h"
#include "loggable.h"

#include <QMainWindow>
#include <QString>
#include <QMdiArea>
#include <QEvent>

#include <any>


QT_BEGIN_NAMESPACE
namespace Ui { class LibWindow; }
QT_END_NAMESPACE

class LibWindow : public QMainWindow, public Coreable, public Loggable
{
    Q_OBJECT
private:
    Ui::LibWindow *ui;

public:
    LibWindow(QWidget *parent = nullptr);
    ~LibWindow() override;
    void updateActions(bool);
    void updateTitle();
    void updateTitle(std::string);
    QMdiArea *mdiArea();
    void fileNewLib();
    void fileOpenLib();
    void fileSaveAs();
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
    void viewNewSymbolView();
    void viewCloseSymbolView();
    void viewLibTreeView();
    void viewLibTableView();
    void windowNewWindow();
    void windowCloseWindow();
    void helpAbout();
    void reloadStyle();

    // Overloaded
    void changeEvent(QEvent *) override;

    signals:
    void activated() const;



};
#endif // LIBWINDOW_H
