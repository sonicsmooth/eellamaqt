#include "dbif.h"
#include "utils.h"
#include <QtSql>
#include <QFileInfo>
#include <QDir>
#include <iostream>
#include <fstream>
#include <cassert>
#include <cstdio>
#include <exception>
#include <filesystem>
#include <system_error>


QSQDbIf::QSQDbIf() {}

void QSQDbIf::createDatabase(std::string fullpath) {
    // Creates and opens qSqlite3 connection and file name fullpath. What's
    // weird is that I don't have to maintain a list of anything anywhere.  The
    // QSqlDatabase object somehow lives for the entirety of the application and
    // keeps a list of available database connection, each of which points to a
    // file of the same name.

    log("DbiF::createDatabase: Creating Sqlite database %s", fullpath.c_str());


    // database name is not the connection name
    // There can be multiple connections, each pointing to a single database
    // (ie sqlite3 file), but that's not what I want.  I want multiple
    // database files.  However a connection can't connect to more than one
    // file.  So we need one connection per file.  The connection should
    // take a name, which is presumably unique, which means we may as well
    // name the connection the same as the database name, which is the
    // sqlite3 database file name.
    QString dbname(QString::fromStdString(fullpath));
    QSqlDatabase db(QSqlDatabase::addDatabase("QSQLITE", dbname));
    db.setDatabaseName(dbname); // specifies file
    bool ok = db.open();
    if (!ok) throw;

    QSqlQuery q(db);
    q.exec("CREATE TABLE firsttable (ID INTEGER PRIMARY KEY, NAME TEXT)");
}
void QSQDbIf::openDatabase(std::string fullpath) {
    log("DbIf::openDatabase: Opening Sqlite database %s", fullpath.c_str());
    QString dbname(QString::fromStdString(fullpath));
    QFile qfile(dbname);
    if (qfile.exists()) {
        QSqlDatabase db(QSqlDatabase::addDatabase("QSQLITE", dbname));
        db.setDatabaseName(dbname); // specifies file
        if (db.open())
            log("DbIf::openDatabase: %s opened", fullpath.c_str());
        else
            throw std::runtime_error("Cannot open " + fullpath);
    } else {
        log("Library '%s' not found", fullpath.c_str());
    }
}
void QSQDbIf::cloneDatabase(std::string oldpath, std::string newpath) {
    // Just copy the thing, then add it to available databases
    // We assume here that sqlite is done writing.
    // If this is run on a separate thread then we should wait until
    // everything is done, somehow

    std::error_code ec;
    try {
        QFileInfo fi(QString::fromStdString(newpath));
        QDir d(fi.absoluteDir());
        if (!d.mkpath(d.path())) {
            throw std::filesystem::filesystem_error("Could not create path", newpath,
                                                    std::make_error_code(std::errc::io_error));
        }
        std::filesystem::copy(oldpath, newpath, std::filesystem::copy_options::recursive);//, ec);
    }
    catch (std::filesystem::filesystem_error & err) {
        log("QSQDbIf::cloneDatabase: %s", err.what());
        throw err;
    }
}
void QSQDbIf::closeDatabase(std::string fullpath) {
    log("DbIf::closeDatabase: closing database " + fullpath);
    QString dbname(QString::fromStdString(fullpath));
    { // so db goes out of scope before removing from database

        QSqlDatabase db(QSqlDatabase::database(dbname));
        db.close(); // needed?
    }
    QSqlDatabase::removeDatabase(dbname);

}
void QSQDbIf::deleteDatabase(std::string fullpath) {
    log("DbIf::deleteDatabase: Deleting database " + fullpath);
    closeDatabase(fullpath);
    std::remove(fullpath.c_str());

}
