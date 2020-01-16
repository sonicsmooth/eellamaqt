#include "filesaveas.h"

#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

FileSaveAs::FileSaveAs(QWidget *parent, std::string existingName) :
    QDialog(parent, Qt::WindowFlags())
{
    auto te = new QLineEdit("hi");
    auto pb = new QPushButton("Browse");
    auto hb1 = new QHBoxLayout;
    hb1->addWidget(te);
    hb1->addWidget(pb);

    auto rb1 = new QRadioButton("Replace");
    auto rb2 = new QRadioButton("Keep Open");
    auto rb3 = new QRadioButton("Qietly");
    auto hb2 = new QVBoxLayout;
    rb1->setToolTip("Replace existing views with views of new file");
    rb2->setToolTip("Keep existing views open; don't open view of new file");
    rb3->setToolTip("Don't close existing views; don't open view of new file");
    hb2->addWidget(rb1);
    hb2->addWidget(rb2);
    hb2->addWidget(rb3);

    auto vb = new QVBoxLayout;
    auto lab1 = new QLabel("Existing library name");
    lab1->setStyleSheet("font-weight: bold;");
    auto lab2 = new QLabel(QString::fromStdString(existingName));
    vb->addWidget(lab1);
    vb->addWidget(lab2);
    vb->addItem(hb1);
    vb->addItem(hb2);

    setWindowTitle("File Save As...");
    setLayout(vb);

}
