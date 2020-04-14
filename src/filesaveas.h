#ifndef FILESAVEAS_H
#define FILESAVEAS_H

#include "loggable.h"
#include "logger.h"
#include "libcore.h"
#include "saveoptions.h"


#include <QDialog>
#include <QLineEdit>

// Starts up little dialog to choose file save
// Line edit with browse button
// Keep open, close old, quiet radio buttons

class FileSaveAs : public QDialog, public Loggable
{
public:
    enum class Mode {SAVEAS, RENAME};

private:
    QString m_fileName;
    bool m_overwrite;
    DupOptions m_option;
    Mode m_mode;
public:
    FileSaveAs(QWidget * = nullptr, std::string = "", Logger * = nullptr, Mode = Mode::SAVEAS);
    void setMode(Mode);
    Mode mode() const;
    QString fileName() const;
    DupOptions option() const;

public slots:
//    void accept() override;
//    void reject() override;
};

#endif // FILESAVEAS_H
