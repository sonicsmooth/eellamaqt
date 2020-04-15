#ifndef MYMODEL_H
#define MYMODEL_H

#include <QAbstractTableModel>
#include <QTimer>

// From the example

const int COLS= 3;
const int ROWS= 2;

class MyModel : public QAbstractTableModel
{
    Q_OBJECT
private:
    QTimer *m_pTimer;
    QString m_gridData[ROWS][COLS];  //holds text entered into QTableView
public:
    MyModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex & = QModelIndex()) const override;
    int columnCount(const QModelIndex & = QModelIndex()) const override;
    QVariant data(const QModelIndex &, int = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
public slots:
    void timerHit();
signals:
    void editCompleted(const QString &);
};

#endif // MYMODEL_H
