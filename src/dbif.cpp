#include "dbif.h"
#include <QtSql>
#include <iostream>
#include <fstream>
#include <cassert>


QSQDbIf::QSQDbIf() {}

void QSQDbIf::createDatabase(std::string fullpath) {

    // Creates and opens qSqlite3 connection and file name fullpath. What's
    // weird is that I don't have to maintain a list of anything anywhere.  The
    // QSqlDatabase object somehow lives for the entirety of the application and
    // keeps a list of available database connection, each of which points to a
    // file of the same name.

log("DbiF: Creating QtSQL database %s", fullpath.c_str());


    // database name is not the connection name
    // There can be multiple connections, each pointing to a single database
    // (ie sqlite3 file), but that's not what I want.  I want multiple
    // database files.  However a connection can't connect to more than one
    // file.  So we need one connection per file.  The connection should
    // take a name, which is presumably unique, which means we may as well
    // name the connection the same as the database name, which is the
    // sqlite3 database file name.
    QString dbname = QString::fromStdString(fullpath);
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", dbname);
    db.setDatabaseName(dbname); // specifies file
    bool ok = db.open();
    if (!ok) throw;

    QSqlQuery q(db);
    q.exec("CREATE TABLE firsttable (ID INTEGER PRIMARY KEY, NAME TEXT)");

}
