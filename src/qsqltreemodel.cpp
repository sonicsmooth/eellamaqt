
#include "qsqltreemodel.h"
#include <QSqlDatabase>


QSqlTreeModel::QSqlTreeModel(QObject *parent, QSqlDatabase db) :
    QStandardItemModel(parent),
    m_dbConnName(db.connectionName())
{
    auto root = invisibleRootItem();
    root->appendRow(new QStandardItem("Americas"));
    root->child(0)->appendRow(new QStandardItem("Canada"));
    root->child(0)->child(0)->appendRow(new QStandardItem("Calgary"));
    root->child(0)->child(0)->appendRow(new QStandardItem("Montreal"));
    root->child(0)->appendRow(new QStandardItem("USA"));
    root->child(0)->child(1)->appendRow(new QStandardItem("Boston"));
    root->child(0)->child(1)->appendRow(new QStandardItem("Seattle"));
    root->appendRow(new QStandardItem("Europe"));
    root->child(1)->appendRow(new QStandardItem("Italy"));
    root->child(1)->child(0)->appendRow(new QStandardItem("Rome"));
    root->child(1)->child(0)->appendRow(new QStandardItem("Verona"));
    root->child(1)->appendRow(new QStandardItem("Germany"));
    root->child(1)->child(1)->appendRow(new QStandardItem("Berlin"));
    root->child(1)->child(1)->appendRow(new QStandardItem("Stuttgart"));
    root->child(1)->appendRow(new QStandardItem("France"));
    root->child(1)->child(2)->appendRow(new QStandardItem("Paris"));
    root->child(1)->child(2)->appendRow(new QStandardItem("Marseilles"));
    root->child(1)->appendRow(new QStandardItem("Netherlands"));
    root->child(1)->child(3)->appendRow(new QStandardItem("Amsterdam"));
    root->child(1)->child(3)->appendRow(new QStandardItem("Coffee shop"));

}

QSqlDatabase QSqlTreeModel::database() const {
    return QSqlDatabase::database(m_dbConnName);
}