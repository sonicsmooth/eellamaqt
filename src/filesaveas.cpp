#include "filesaveas.h"
#include "libcore.h"

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

    auto hb2 = new QVBoxLayout;
    hb2->addWidget(cbOpenNew);
    hb2->addWidget(cbCloseExisting);

    // Set current directory that of existingName
    // If for some reason that fails, then set to EELLama Libraries,
    // and if that doesn't work, then home
    QFileInfo existingFile(QString::fromStdString(existingName));
    QDir currdir(existingFile.absoluteDir());

    QString xx1 = currdir.dirName();
    QString xx2 = currdir.path();
    QString xx3 = currdir.filePath("somejunk1");
    QString xx4 = currdir.absoluteFilePath("somejunk2");
    QDir testdir("hello/there");
    xx1 = testdir.dirName();
    xx2 = testdir.path();
    xx3 = testdir.filePath("somejunk1");
    xx4 = testdir.absoluteFilePath("somejunk2");

    QFileInfo finf("c:/what/there/somelib.SchLib");
    QString f1 = finf.dir().path();
    QString f2 = finf.absoluteDir().path();
    QString f3 = finf.fileName();
    QString f4 = finf.filePath();
    QString f5 = finf.absoluteFilePath();

    finf = QFileInfo("/what/there/somelib.SchLib");
    f1 = finf.dir().path();
    f2 = finf.absoluteDir().path();
    f3 = finf.fileName();
    f4 = finf.filePath();
    f5 = finf.absoluteFilePath();

    finf = QFileInfo("what/there/somelib.SchLib");
    f1 = finf.dir().path();
    f2 = finf.absoluteDir().path();
    f3 = finf.fileName();
    f4 = finf.filePath();
    f5 = finf.absoluteFilePath();

    if (!currdir.exists()) {
        if (currdir.exists(GLibDir))
            currdir.setPath(QDir::home().filePath(GLibDir));
        else
            currdir.setPath(QDir::home().path());
    }

    auto vb = new QVBoxLayout;
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
        m_selectedFileName = finalPath;
    };

    updateText(""); // fills in label
    QObject::connect(m_pLineEdit, &QLineEdit::textChanged, updateText);
    auto qdbb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    QObject::connect(qdbb, &QDialogButtonBox::accepted, this, &QDialog::accept);
    QObject::connect(qdbb, &QDialogButtonBox::rejected, this, &QDialog::reject);
    labExistHdr->setStyleSheet("font-weight: bold;");
    labNewHdr->setStyleSheet("font-weight: bold;");

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
}

QString FileSaveAs::selectedFileName() const {
    return m_selectedFileName;
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
