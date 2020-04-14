#include "qdbif.h"
#include "utils.h"
#include <QtSql>
#include <QSqlDatabase>
#include <QFileInfo>
#include <QDir>
#include <iostream>
#include <fstream>
#include <cassert>
#include <cstdio>
#include <exception>
#include <filesystem>
#include <system_error>
#include <any>

//static void cloneDatabase(std::string oldpath, std::string newpath);
//static void renameDatabase(std::string oldpath, std::string newpath);


//// PRIVATE FUNCTIONS
void QSQDbIf::pushActiveDb(std::string adb) {
    // Ensure exactly one adb is in the list
    // and that it's at the front
    std::optional<std::string> oldFront(activeDatabase());
    m_activeDb.remove(adb); // removes all instances of adb
    m_activeDb.push_front(adb);

}
void QSQDbIf::popActiveDb(std::string adb) {
    // Remove adb from activeDb list;
    // throw error if not in list
    if (isDatabaseOpen(adb)) {
        m_activeDb.remove(adb);
    } else {
        throw std::runtime_error("Library " + adb + " not in activeDb list");
    }
}
void QSQDbIf::cloneDb(std::string oldpath, std::string newpath) {
    // Just copy the file directly
    // We assume here that sqlite is done writing to oldpath already
    // If this is run on a separate thread then we should wait until
    // everything is done, somehow
    std::error_code ec;
    try {
        // Recursively create parent paths
        QFileInfo fi(QString::fromStdString(newpath));
        QDir d(fi.absoluteDir());
        if (!d.mkpath(d.path())) {
            throw std::filesystem::filesystem_error("Could not create path", newpath,
                                                    std::make_error_code(std::errc::io_error));
        }
        std::filesystem::copy_file(oldpath, newpath, std::filesystem::copy_options::overwrite_existing);
    }
    catch (std::filesystem::filesystem_error & err) {
        throw err;
    }
}
void QSQDbIf::renameDb(std::string oldpath, std::string newpath) {
    // Just rename the file directly. This may throw an error of newpath 
    // is already open, so caller should close newpath first
    // We assume here that sqlite is done writing to oldpath already.
    // If this is run on a separate thread then we should wait until
    // everything is done, somehow
    std::error_code ec;
    try {
        // Recursively create parent paths
        QFileInfo fi(QString::fromStdString(newpath));
        QDir d(fi.absoluteDir());
        if (!d.mkpath(d.path())) {
            throw std::filesystem::filesystem_error("Could not create path", newpath,
                                                    std::make_error_code(std::errc::io_error));
        }
        std::filesystem::rename(oldpath, newpath);
    }
    catch (std::filesystem::filesystem_error & err) {
        throw err;
    }
}




QSQDbIf::QSQDbIf() {}

void QSQDbIf::createDatabase(std::string fullpath) {
    // Creates and opens qSqlite3 connection and file name fullpath. What's
    // weird is that I don't have to maintain a list of anything anywhere.  The
    // QSqlDatabase object somehow lives for the entirety of the application and
    // keeps a list of available database connections, each of which points to a
    // file of the same name.

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
    QString xx = db.databaseName();
    bool ok = db.open();
    if (!ok) throw;
    pushActiveDb(fullpath);

    QSqlQuery q(db);
    q.exec("CREATE TABLE firsttable (ID INTEGER PRIMARY KEY, NAME TEXT)");
    q.exec("INSERT INTO firsttable (ID, NAME) VALUES (10, 'Mikey'), (20, 'Masha')");
}
void QSQDbIf::openDatabase(std::string fullpath) {
    // Attempts to open existing SQLite database file
    QString dbname(QString::fromStdString(fullpath));
    QFile qfile(dbname);
    if (qfile.exists()) {
        QSqlDatabase db(QSqlDatabase::addDatabase("QSQLITE", dbname));
        db.setDatabaseName(dbname); // specifies file
        if (!db.open())
            throw std::runtime_error("Cannot open " + fullpath);
        pushActiveDb(fullpath);
    } else {
        log("Library '%s' not found", fullpath.c_str());
    }
}
void QSQDbIf::saveDatabase(std::string oldpath, std::string newpath, DupOptions opt) {
    switch(opt) {
    case DupOptions::CLOSE_OLD:
        // Use existing UI for new, close old one
        log("QSQDbIf::saveLib: Saving library CLOSE_OLD " + newpath);
        try {
            cloneDb(oldpath, newpath);
            closeDatabase(oldpath);
            openDatabase(newpath);
        }
        catch (std::filesystem::filesystem_error err) {
            log("QSQDbIf::saveLib: Failed to save library");
        }
        break;
    case DupOptions::OPEN_NEW:
        // Keep old one open, open new one too
        log("QSQDbIf::saveLib: Saving library OPEN_NEW " + newpath);
        try {
            cloneDb(oldpath, newpath);
            openDatabase(newpath);
        }
        catch (std::filesystem::filesystem_error err) {
            log("QSQDbIf::saveLib: Failed to save library");
        }
        break;
    case DupOptions::QUIETLY:
        // Neither open new one nor close old one
        log("QSQDbIf::saveLib: Saving library QUIETLY " + newpath);
        try {
            cloneDb(oldpath, newpath);
        }
        catch (std::filesystem::filesystem_error err) {
            log("QSQDbIf::saveLib: Failed to save library");
        }
        break;
    case DupOptions::RENAME:
        log("QSQDbIf::saveLib: Saving library RENAME " + newpath);
        bool newIsOpen(isDatabaseOpen(newpath));
        try {
            closeDatabase(oldpath);
            if (newIsOpen)
                closeDatabase(newpath);
            renameDb(oldpath, newpath);
            openDatabase(newpath);
        }
        catch (std::filesystem::filesystem_error err) {
            log("QSQDbIf::saveLib: Failed to rename library");
            // restore previous
            openDatabase(oldpath);
        }
        break;
    }





}
std::optional<std::string> QSQDbIf::activeDatabase() const {
    // Returns either the string of the active db or null option
    // if there is no active db
    return m_activeDb.size() ? std::optional<std::string>{m_activeDb.front()} :
                               std::nullopt;
}

void QSQDbIf::closeDatabase(std::string fullpath) {
    QString dbname(QString::fromStdString(fullpath));
    { // so db goes out of scope before removing from database
        QSqlDatabase db(QSqlDatabase::database(dbname));
        db.close();
    }
    QSqlDatabase::removeDatabase(dbname);
    popActiveDb(fullpath);
}
void QSQDbIf::deleteDatabase(std::string fullpath) {
    closeDatabase(fullpath);
    // https://en.cppreference.com/w/cpp/io/c/remove
    std::remove(fullpath.c_str()); // deletes file, from <cstdio>
}
bool QSQDbIf::isDatabaseOpen(std::string fullpath) const {
    // Returns whether fullpath is in active db
    return std::find(m_activeDb.begin(), m_activeDb.end(), fullpath) != m_activeDb.end();
}
bool QSQDbIf::isDatabaseOpen() const  {
    return m_activeDb.size() > 0;
}
// Return type here should probably be something better than std::any
QSqlDatabase QSQDbIf::database(std::string fullpath) {
    return QSqlDatabase::database(QString::fromStdString(fullpath));
}
