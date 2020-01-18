#ifndef FILESAVEAS_H
#define FILESAVEAS_H

#include "loggable.h"

#include <QDialog>
#include <QLineEdit>

// Starts up little dialog to choose file save
// Line edit with browse button
// Keep open, close old, quiet radio buttons

class FileSaveAs : public QDialog, public Loggable
{
private:
    QLineEdit *m_pLineEdit;
    QString m_selectedFileName;
public:
    FileSaveAs(QWidget * = nullptr, std::string = "");
    QString selectedFileName() const;

public slots:
//    void accept() override;
//    void reject() override;
};

#endif // FILESAVEAS_H
