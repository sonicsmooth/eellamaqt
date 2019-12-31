#include "dbif.h"

#include <iostream>
#include <fstream>

#include <QtSql>

DbIf::DbIf() : m_pLogger(nullptr) {}
void DbIf::setLogger(Logger *pLogger) {
    m_pLogger = pLogger;
}


void DbIf::createDatabase(std::string fullpath) {
    if (m_pLogger)
        m_pLogger->log("DbiF: Creating QtSQL database %s", fullpath.c_str());

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QString::fromStdString(fullpath));
    bool ok = db.open();
    QSqlQuery q;
    q.exec("CREATE TABLE firsttable (ID INTEGER PRIMARY KEY, NAME TEXT)");
    
}
