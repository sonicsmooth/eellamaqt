#ifndef DBIF_H
#define DBIF_H

#include "idbif.h"
#include "coreable.h"
#include "loggable.h"

#include <QtSql>

#include <any>

// This is the interface between the core and the Qt database

class QSQDbIf : public IDbIf, public Coreable, public Loggable
{
private:
public:
    QSQDbIf();
    void createDatabase(std::string) override;
    void openDatabase(std::string) override;
    void cloneDatabase(std::string, std::string) override;
    void renameDatabase(std::string, std::string) override;
    void closeDatabase(std::string) override;
    void deleteDatabase(std::string) override;
    std::any database(std::string) override;

};

#endif // DBIF_H
