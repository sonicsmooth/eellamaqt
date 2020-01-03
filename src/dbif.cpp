#include "dbif.h"

#include <iostream>
#include <fstream>
#include <cassert>

#include <QtSql>

QSQDbIf::QSQDbIf() : m_pLogger(nullptr) {}
void QSQDbIf::setLogger(Logger *pLogger) {
    m_pLogger = pLogger;
}


void QSQDbIf::createDatabase(std::string fullpath) {
    assert(m_pLogger);
    m_pLogger->log("DbiF: Creating QtSQL database %s", fullpath.c_str());

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", QString::fromStdString(fullpath));
    bool ok = db.open();
    if (!ok) throw;
    QSqlQuery q(db);
    q.exec("CREATE TABLE firsttable (ID INTEGER PRIMARY KEY, NAME TEXT)");
    auto err = q.lastError();
    m_pLogger->log(err.driverText());
    db.close();

}
