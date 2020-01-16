#ifndef FILESAVEAS_H
#define FILESAVEAS_H

#include <QDialog>

// Starts up little dialog to choose file save
// Line edit with browse button
// Keep open, close old, quiet radio buttons

class FileSaveAs : public QDialog
{
public:
    FileSaveAs(QWidget * = nullptr, std::string = "");
};

#endif // FILESAVEAS_H
