#include "filesaveas.h"

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

    QDir currdir = QDir::home();
    QString dirname = "EE LLama Libraries";
    if (currdir.exists(dirname))
        currdir.cd(dirname);

    auto vb = new QVBoxLayout;
    auto labExistHdr = new QLabel("Existing library name");
    auto labExistTxt = new QLabel(QString::fromStdString(existingName));
    auto labNewHdr = new QLabel("New library name");
    auto labNewTxt = new QLabel(currdir.absolutePath());
    QObject::connect(m_pLineEdit, &QLineEdit::textChanged, [=](const QString & str) {

        QFileInfo qfi(dirname + "/" + str);
        QString fulltxt = qfi.filePath();
        labNewTxt->setText(fulltxt);
        m_selectedFileName = fulltxt;
    });

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
