#include "mymodel.h"
#include <QString>
#include <QDebug>
#include <QFont>
#include <QBrush>
#include <QTime>
#include <QTimer>

MyModel::MyModel(QObject *parent)
    : QAbstractTableModel(parent),
      m_pTimer(new QTimer(this))
{
    m_pTimer->setInterval(1000);
    QObject::connect(m_pTimer, &QTimer::timeout, this, &MyModel::timerHit);
    m_pTimer->start();
}

void MyModel::timerHit() {
    //we identify the top left cell
    QModelIndex topLeft = createIndex(0,0);
    //emit a signal to make the view reread identified data
    emit dataChanged(topLeft, topLeft, {Qt::DisplayRole});
}

int MyModel::rowCount(const QModelIndex & idx) const
{
   (void) idx;
   return 6;
}

int MyModel::columnCount(const QModelIndex & idx) const
{
    (void) idx;
    return 4;
}

QVariant MyModel::data(const QModelIndex &idx, int role) const
{
    int row = idx.row();
    int col = idx.column();
    // generate a log message when this method gets called
    switch (role) {
    case Qt::DisplayRole:
        //return m_gridData[row][col];
//        if (row == 0 && col == 0) return QTime::currentTime().toString();
//        if (row == 0 && col == 1) return QString("<--left");
//        if (row == 1 && col == 1) return QString("right-->");

        return QString("Prod %1")
                .arg(row * col);
    case Qt::FontRole:
        if (row == 0 && col == 0) { //change font only for cell(0,0)
            QFont boldFont;
            boldFont.setBold(true);
            return boldFont;
        }
        break;
    case Qt::BackgroundRole:
        if (row == 1 && col == 2)  //change background only for cell(1,2)
            return QBrush(Qt::red);
        break;
    case Qt::TextAlignmentRole:
        if (row == 1 && col == 1) //change text alignment only for cell(1,1)
            return Qt::AlignRight + Qt::AlignVCenter;
        break;
    case Qt::CheckStateRole:
        if (row == 1 && col == 0) //add a checkbox to cell(1,0)
            return Qt::Checked;
        break;


    }
    return QVariant();
}
QVariant MyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch(role) {
    case Qt::DisplayRole:
        if (orientation == Qt::Horizontal) {
            switch (section) {
            case 0:
                return QString("first");
            case 1:
                return QString("second");
            case 2:
                return QString("third");
            case 3:
                return QString("fourth");
            }
        }
        break;
    case Qt::FontRole:
        return QFont("Courier New");
    }
    return QVariant();
}

bool MyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole) {
        if (!checkIndex(index))
            return false;
        //save value from editor to member m_gridData
        m_gridData[index.row()][index.column()] = value.toString();
        //for presentation purposes only: build and emit a joined string
        QString result;
        for (int row = 0; row < ROWS; row++) {
            for (int col= 0; col < COLS; col++)
                result += m_gridData[row][col] + ' ';
        }
        emit editCompleted(result);
        return true;
    }
    return false;
}
Qt::ItemFlags MyModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}
