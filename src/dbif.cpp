#include "dbif.h"

#include <iostream>
#include <fstream>

#include <QtSql>


void DbIf::createDatabase(std::string fullpath) {
    std::cout << "Creating qt database " << fullpath;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QString::fromStdString(fullpath));
    bool ok = db.open();
    QSqlQuery q;
    q.exec("CREATE TABLE firsttable (ID INTEGER PRIMARY KEY, NAME TEXT)");
    






    //std::ofstream f;
    //f.open(fullpath, std::fstream::out);
    //f << "hi there";
    //f.close();

}
