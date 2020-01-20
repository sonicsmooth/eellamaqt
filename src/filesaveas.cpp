#include "filesaveas.h"
#include "libcore.h"

#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QCheckBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QDialogButtonBox>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>

FileSaveAs::FileSaveAs(QWidget *parent,
                       std::string existingName,
                       Logger *pLogger) :
    QDialog(parent, Qt::WindowFlags()) {
    // Create little dialog box which allows user to type in new filename,
    // or to browse using system dialog box to specify filename.  In either case,
    // user can specify whole path.  In either case, the full path is shown
    // before user clicks ok.
    // TODO:  examine whether this can or should be modeless

    m_pLogger = pLogger;
    m_pLineEdit = new QLineEdit("");
    m_pLineEdit->setPlaceholderText("Type new library name");
    auto pbBrowse = new QPushButton("Browse");
    QObject::connect(pbBrowse, &QPushButton::clicked, [=]() {
        QFileDialog qfd(this);
        qfd.setFileMode(QFileDialog::AnyFile);
        qfd.setAcceptMode(QFileDialog::AcceptSave);
        qfd.setNameFilter("Any (*);;Library files (*.SchLib *.db)");
        if (qfd.exec()) {
            m_pLineEdit->setText(qfd.selectedFiles()[0]);
        }
    });
    auto hb1 = new QHBoxLayout;
    hb1->addWidget(m_pLineEdit);
    hb1->addWidget(pbBrowse);
    auto cbOpenNew = new QCheckBox("Open new library");
    auto cbCloseExisting = new QCheckBox("Close existing library");
    auto updateOptions = [=]() {
        m_option = !cbOpenNew->isChecked()       ? LibCore::DupOptions::QUIETLY   :
                    cbCloseExisting->isChecked() ? LibCore::DupOptions::CLOSE_OLD :
                                                   LibCore::DupOptions::OPEN_NEW;
//        // openNew      closeExisting       option
//        // false        false               QUIETLY
//        // false        true                N/A
//        // true         false               OPEN_NEW
//        // true         true                CLOSE_OLD
    };
    cbOpenNew->setChecked(true);
    cbCloseExisting->setChecked(true);
    QObject::connect(cbOpenNew, &QPushButton::clicked, [=](bool c) {
        cbCloseExisting->setEnabled(c);
        if (!c)
            cbCloseExisting->setChecked(false);
    });
    QObject::connect(cbOpenNew, &QPushButton::clicked, updateOptions);
    QObject::connect(cbCloseExisting, &QPushButton::clicked, updateOptions);

    auto hb2 = new QVBoxLayout;
    hb2->addWidget(cbOpenNew);
    hb2->addWidget(cbCloseExisting);

    // Set current directory that of existingName
    // If for some reason that fails, then set to EELLama Libraries,
    // and if that doesn't work, then home
    QFileInfo existingFile(QString::fromStdString(existingName));
    QDir currdir(existingFile.absoluteDir());

    if (!currdir.exists()) {
        if (currdir.exists(GLibDir))
            currdir.setPath(QDir::home().filePath(GLibDir));
        else
            currdir.setPath(QDir::home().path());
    }

    auto vb = new QVBoxLayout;
    auto te = new QLabel("Save copy of existing library");

    QFile style(":/ui/llamastyle.css");
    style.open(QIODevice::ReadOnly);
    //te->setAccessibleName("DialogLabel");
    te->setObjectName("DialogLabel");
    te->setStyleSheet(style.readAll());
    te->setWordWrap(true);

    auto labExistHdr = new QLabel("Existing library name");
    auto labExistTxt = new QLabel(QString::fromStdString(existingName));
    auto labNewHdr = new QLabel("New library name");
    auto labNewTxt = new QLabel;
    auto updateText = [=](const QString & str) {
        QFileInfo userfi(str);
        QDir userdir = userfi.dir();
        QDir userabsdir = userfi.absoluteDir();
        QString userbase = userfi.completeBaseName();
        QString suffix = userfi.suffix();
        QDir exdir(existingFile.dir());

        QString libsuffix = "SchLib";
        if (suffix.length() == 0)
            suffix = libsuffix;

        QString finalPath;
        if (userbase.length()) {
            if (userfi.isAbsolute() && userbase[0] != '.') { // eg .tmp
                finalPath = userabsdir.filePath(userbase) + "." + suffix;
            } else {
                finalPath = exdir.filePath(userdir.path());
                finalPath = QDir::cleanPath(finalPath);
                finalPath = QDir(finalPath).filePath(userbase) + "." + suffix;
            }
        }
        labNewTxt->setText(finalPath);
        m_fileName = finalPath;
        updateOptions();
    };

    updateText(""); // fills in label
    updateOptions(); // redundant if updateText is called with no changes
    QObject::connect(m_pLineEdit, &QLineEdit::textChanged, updateText);
    auto qdbb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    QObject::connect(qdbb, &QDialogButtonBox::accepted, this, &QDialog::accept);
    QObject::connect(qdbb, &QDialogButtonBox::rejected, this, &QDialog::reject);
    labExistHdr->setStyleSheet("font-weight: bold;");
    labNewHdr->setStyleSheet("font-weight: bold;");

    vb->addWidget(te);
    vb->addItem(new QSpacerItem(1, 15, QSizePolicy::Expanding, QSizePolicy::MinimumExpanding));
    vb->addWidget(labExistHdr);
    vb->addWidget(labExistTxt);
    vb->addItem(new QSpacerItem(1, 15, QSizePolicy::Expanding, QSizePolicy::MinimumExpanding));
    vb->addWidget(labNewHdr);
    vb->addWidget(labNewTxt);
    vb->addItem(hb1);
    vb->addItem(new QSpacerItem(1, 15, QSizePolicy::Expanding, QSizePolicy::MinimumExpanding));
    vb->addItem(hb2);
    vb->addItem(new QSpacerItem(1, 15, QSizePolicy::Expanding, QSizePolicy::MinimumExpanding));
    vb->addWidget(qdbb);

    setWindowTitle("File Save As...");
    setLayout(vb);
    setSizeGripEnabled(true);
    setMaximumHeight(500);
    resize(640,300);
}

QString FileSaveAs::fileName() const {
    return m_fileName;
}
LibCore::DupOptions FileSaveAs::option() const {
    return m_option;
}
