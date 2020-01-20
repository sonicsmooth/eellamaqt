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
#include <QMessageBox>
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
    auto qle = new QLineEdit("");
    qle->setPlaceholderText("Type new library name");
    auto labWarn = new QLabel("Exists aready!");
    labWarn->setStyleSheet("color: red; font-weight: bold;");

    auto pbBrowse = new QPushButton("Browse");
    QObject::connect(pbBrowse, &QPushButton::clicked, [=]() {
        QFileDialog qfd(this);
        qfd.setFileMode(QFileDialog::AnyFile);
        qfd.setAcceptMode(QFileDialog::AcceptSave);
        qfd.setOption(QFileDialog::DontConfirmOverwrite, true);
        qfd.setWindowTitle("Save Library As...");
        qfd.setNameFilter("Any (*);;Library files (*.SchLib *.db)");
        if (qfd.exec()) {
            qle->setText(qfd.selectedFiles()[0]);
        }
    });
    auto cbOpenNew = new QCheckBox("Open new library");
    auto cbCloseExisting = new QCheckBox("Close original library");
    auto cbOverwrite = new QCheckBox("Overwrite without prompt");
    auto updateOptions = [=]() {
        m_option = !cbOpenNew->isChecked()       ? LibCore::DupOptions::QUIETLY   :
                    cbCloseExisting->isChecked() ? LibCore::DupOptions::CLOSE_OLD :
                                                   LibCore::DupOptions::OPEN_NEW;
        m_overwrite = cbOverwrite->isChecked();
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
    // If we don't do these connections, then the checkboxes will
    // only be relevant when the user types something.
    QObject::connect(cbOpenNew, &QPushButton::clicked, updateOptions);
    QObject::connect(cbCloseExisting, &QPushButton::clicked, updateOptions);
    QObject::connect(cbOverwrite, &QPushButton::clicked, updateOptions);


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

    auto te = new QLabel("Save copy of existing library");
    QFile style(":/ui/llamastyle.css");
    style.open(QIODevice::ReadOnly);
    te->setObjectName("DialogLabel");
    te->setStyleSheet(style.readAll());

    auto labExistHdr = new QLabel("Existing library name");
    auto labExistTxt = new QLabel(QString::fromStdString(existingName));
    auto labNewHdr = new QLabel("New library name");
    auto labNewTxt = new QLabel;
    auto updateText = [=](const QString & str) {
        // Take what user writes and create a full and proper pathname
        // to be displayed in real time
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
        QFileInfo fpi(finalPath);
        cbOverwrite->setEnabled(fpi.exists());
        labWarn->setVisible(fpi.exists());
        labNewTxt->setText(finalPath);
        m_fileName = finalPath;
        updateOptions();
    };

    updateText(""); // fills in label
    updateOptions(); // redundant if updateText is called with no changes
    QObject::connect(qle, &QLineEdit::textChanged, updateText);
    auto qdbb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    QObject::connect(qdbb, &QDialogButtonBox::rejected, this, &QDialog::reject);
    QObject::connect(qdbb, &QDialogButtonBox::accepted, [=]() {
        QFileInfo qfi(m_fileName);
        if (!qfi.exists() | m_overwrite)
            QDialog::accept();
        else {
            QMessageBox qmb;
            qmb.setText("File exists!");
            qmb.setInformativeText("Ovewrite existing file?");
            qmb.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            int dcode = qmb.exec();
            if (dcode == QMessageBox::Yes)
                QDialog::accept();
        }
    });
    labExistHdr->setStyleSheet("font-weight: bold;");
    labNewHdr->setStyleSheet("font-weight: bold;");

    auto hb1 = new QHBoxLayout;
    auto hb2 = new QHBoxLayout;
    auto vb1 = new QVBoxLayout;
    auto vb2 = new QVBoxLayout;
    hb1->addWidget(qle);
    hb1->addWidget(pbBrowse);
    hb2->addWidget(labNewTxt);
    hb2->addItem(new QSpacerItem(100,1, QSizePolicy::MinimumExpanding, QSizePolicy::Expanding));
    hb2->addWidget(labWarn);
    vb1->addWidget(cbOpenNew);
    vb1->addWidget(cbCloseExisting);
    vb1->addWidget(cbOverwrite);
    vb2->addWidget(te);
    vb2->addItem(new QSpacerItem(1, 15, QSizePolicy::Expanding, QSizePolicy::MinimumExpanding));
    vb2->addWidget(labExistHdr);
    vb2->addWidget(labExistTxt);
    vb2->addItem(new QSpacerItem(1, 15, QSizePolicy::Expanding, QSizePolicy::MinimumExpanding));
    vb2->addWidget(labNewHdr);
    vb2->addItem(hb2);
    vb2->addItem(hb1);
    vb2->addItem(new QSpacerItem(1, 15, QSizePolicy::Expanding, QSizePolicy::MinimumExpanding));
    vb2->addItem(vb1);
    vb2->addItem(new QSpacerItem(1, 15, QSizePolicy::Expanding, QSizePolicy::MinimumExpanding));
    vb2->addWidget(qdbb);

    setWindowTitle("File Save As...");
    setLayout(vb2);
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
//void FileSaveAs::accept() {
//}
