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
#include <QIcon>

FileSaveAs::FileSaveAs(QWidget *parent,
                       std::string existingName,
                       Logger *pLogger,
                       Mode mode) :
    QDialog(parent, Qt::WindowFlags()),
    m_mode(mode) {
    // Create little dialog box which allows user to type in new filename,
    // or to browse using system dialog box to specify filename.  In either case,
    // user can specify whole path.  In either case, the full path is shown
    // before user clicks ok.
    // TODO:  examine whether this can or should be modeless

    m_pLogger = pLogger;
    QString windowTitle = mode == Mode::SAVEAS ? "Save Library As..." : "Rename Library As...";
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
        qfd.setWindowTitle(windowTitle);
        qfd.setNameFilter("Any (*);;Library files (*.SchLib *.db)");
        if (qfd.exec()) {
            qle->setText(qfd.selectedFiles()[0]);
        }
    });
    auto cbOpenNew = new QCheckBox("Open new library");
    auto cbCloseExisting = new QCheckBox("Close original library");
    auto cbOverwrite = new QCheckBox("Overwrite without prompt");
    auto updateOptions = [=]() {
        if (mode == Mode::SAVEAS) {
        m_option = !cbOpenNew->isChecked()       ? LibCore::DupOptions::QUIETLY   :
                    cbCloseExisting->isChecked() ? LibCore::DupOptions::CLOSE_OLD :
                                                   LibCore::DupOptions::OPEN_NEW;
        } else if (mode == Mode::RENAME) {
            m_option = LibCore::DupOptions::RENAME;
        } else {
            throw std::invalid_argument ("Wrong mode argument");
        }
        m_overwrite = cbOverwrite->isChecked();
    };
    if (mode == Mode::SAVEAS) {
        cbOpenNew->setChecked(true);
        cbCloseExisting->setChecked(true);
        QObject::connect(cbOpenNew, &QPushButton::clicked, [=](bool c) {
            cbCloseExisting->setEnabled(c);
            if (!c)
                cbCloseExisting->setChecked(false);
        });
    }
    // If we don't do these connections, then the checkboxes will
    // only be relevant when the user types something.
    if (mode == Mode::SAVEAS) {
        QObject::connect(cbOpenNew, &QPushButton::clicked, updateOptions);
        QObject::connect(cbCloseExisting, &QPushButton::clicked, updateOptions);
    }
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

    auto dtbw = new QWidget();
    dtbw->setObjectName("DialogTitleBlock");
    auto dtb = new QLabel(mode == Mode::SAVEAS ?
        "Save Copy of Existing Library" :
        "Rename Existing Library", dtbw);
    auto dticon = new QLabel(dtbw);
    dticon->setPixmap(QPixmap(":/actions/document-save-as.png"));
    auto dthb = new QHBoxLayout(dtbw);
    dthb->addWidget(dtb);
    dthb->addItem(new QSpacerItem(1,1, QSizePolicy::Expanding, QSizePolicy::Fixed));
    dthb->addWidget(dticon);

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
            QMessageBox qmb(this);
            qmb.setText("File exists!");
            qmb.setInformativeText("Overwrite existing file?");
            qmb.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            qmb.setStyleSheet("QLabel {min-width: 200px;}"); // hack to force width of dialog box
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
    auto vb3 = new QVBoxLayout;
    vb2->setMargin(10);
    vb3->setMargin(0);
    hb1->addWidget(qle);
    hb1->addWidget(pbBrowse);
    hb2->addWidget(labNewTxt);
    hb2->addItem(new QSpacerItem(1,1, QSizePolicy::Expanding, QSizePolicy::Fixed));
    hb2->addWidget(labWarn);
    if (mode == Mode::SAVEAS) {
        vb1->addWidget(cbOpenNew);
        vb1->addWidget(cbCloseExisting);
    }
    vb1->addWidget(cbOverwrite);
    vb3->addWidget(dtbw);
    vb3->addItem(vb2);
    vb2->addItem(new QSpacerItem(1, 10, QSizePolicy::Expanding, QSizePolicy::Fixed));
    vb2->addWidget(labExistHdr);
    vb2->addWidget(labExistTxt);
    vb2->addItem(new QSpacerItem(1, 10, QSizePolicy::Expanding, QSizePolicy::Fixed));
    vb2->addWidget(labNewHdr);
    vb2->addItem(hb2);
    vb2->addItem(hb1);
    vb2->addItem(new QSpacerItem(1, 10, QSizePolicy::Expanding, QSizePolicy::Fixed));
    vb2->addItem(vb1);
    vb2->addItem(new QSpacerItem(1, 10, QSizePolicy::Expanding, QSizePolicy::Expanding));
    vb2->addWidget(qdbb);

    setWindowTitle(windowTitle);
    setLayout(vb3);
    setSizeGripEnabled(true);
    setObjectName("FileSaveAsDialog");
    resize(400,0);
}

QString FileSaveAs::fileName() const {
    return m_fileName;
}
LibCore::DupOptions FileSaveAs::option() const {
    return m_option;
}

void FileSaveAs::setMode(FileSaveAs::Mode m) {
    m_mode = m;
}
FileSaveAs::Mode FileSaveAs::mode() const {
    return m_mode;
}
