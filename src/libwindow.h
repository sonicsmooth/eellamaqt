#ifndef LIBWINDOW_H
#define LIBWINDOW_H

#include "libcore.h"
#include "coreable.h"
#include "loggable.h"

#include <QMainWindow>
#include <QString>
#include <QMdiArea>
#include <QEvent>
#include <QCloseEvent>
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
    LibWindow(QWidget *parent = nullptr, LibCore * = nullptr, ILogger * = nullptr);
    ~LibWindow() override;
    void updateLibActions(bool);
    void updateTabActions();
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
    void duplicateMainView();
    void popOutMainView();
    void closeMainView();
    void toggleLibTreeView();
    void toggleLibTableView();
    void newWindow(); 
    void closeWindow();
    void helpAbout();
    void reloadStyle();
    void mdiTabMode();
    void mdiTileSubWindows();
    void mdiCascadeSubWindows();

    // Overloaded
    void changeEvent(QEvent *) override;
    void closeEvent(QCloseEvent *) override;

    signals:
    void activated() const;
    void closing(LibWindow *) const;



};
#endif // LIBWINDOW_H

