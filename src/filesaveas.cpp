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

FileSaveAs::FileSaveAs(QWidget *parent, std::string existingName) :
    QDialog(parent, Qt::WindowFlags()) {
    // Create little dialog box which allows user to type in new filename,
    // or to browse using system dialog box to specify filename.  In either case,
    // user can specify whole path.  In either case, the full path is shown
    // before user clicks ok.
    // TODO:  examine whether this can or should be modeless

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
    cbOpenNew->setChecked(true);
    cbCloseExisting->setChecked(true);
    QObject::connect(cbOpenNew, &QPushButton::clicked, [=](bool c) {
        cbCloseExisting->setEnabled(c);
        if (!c)
            cbCloseExisting->setChecked(false);
    });

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
    auto te = new QLabel("Save copy of existing library.  You can open the new library or keep it closed, and you can keep the existing library open or close it.");
    te->setStyleSheet("* {/*background-color: white;*/"
                      "   font: 13pt;}");
    te->resize(0, 100);
    te->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    te->setWordWrap(true);
    //te->setReadOnly(true);


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
        if (userfi.isAbsolute()) {
            if (userbase.length() == 0 || userbase[0] == '.') // eg .tmp
                finalPath = userabsdir.path();
            else
                finalPath = userabsdir.filePath(userbase) + "." + suffix;
        } else {
            finalPath = exdir.filePath(userdir.path());
            if (userbase.length()) {
                finalPath = QDir::cleanPath(finalPath);
                finalPath = QDir(finalPath).filePath(userbase) + "." + suffix;
            }
        }
        labNewTxt->setText(finalPath);
        m_fileName = finalPath;

        cbOpenNew->setChecked(true);
        cbCloseExisting->setChecked(true);
//        LibCore::DupOptions::RENAME, QUIETLY, OPEN_NEW, CLOSE_OLD

//        // openNew      closeExisting       option
//        // false        false               QUIETLY
//        // false        true                N/A
//        // true         false               OPEN_NEW
//        // true         true                CLOSE_OLD

    };

    updateText(""); // fills in label
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
//void FileSaveAs::accept() {
//    m_selectedFileName = m_pLineEdit->text();
//    // Not sure why we don't emit accepted.
//    emit QDialog::accept();
//}
//void FileSaveAs::reject() {
//    m_selectedFileName.clear();
//    emit QDialog::reject();
//}
