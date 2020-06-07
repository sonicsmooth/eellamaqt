#ifndef QSQLTREEMODEL_H
#define QSQLTREEMODEL_H

#include <QStandardItemModel>
#include <QSqlDatabase>
#include <QString>

class QSqlTreeModel : public QStandardItemModel
{
private:
    // So we don't keep the db connection around
    QString m_dbConnName;
    
public:
    QSqlTreeModel(QObject *, QSqlDatabase db);
    QSqlDatabase database() const;
    void setDatabase(QSqlDatabase);
};

#endif // QSQLTREEMODEL_H
