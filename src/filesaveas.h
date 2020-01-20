#ifndef FILESAVEAS_H
#define FILESAVEAS_H

#include "loggable.h"
#include "logger.h"
#include "libcore.h"


#include <QDialog>
#include <QLineEdit>

// Starts up little dialog to choose file save
// Line edit with browse button
// Keep open, close old, quiet radio buttons

class FileSaveAs : public QDialog, public Loggable
{
private:
    QLineEdit *m_pLineEdit;
    QString m_fileName;
    LibCore::DupOptions m_option;
public:
    FileSaveAs(QWidget * = nullptr, std::string = "", Logger * = nullptr);
    QString fileName() const;
    LibCore::DupOptions option() const;

public slots:
//    void accept() override;
//    void reject() override;
};

#endif // FILESAVEAS_H
