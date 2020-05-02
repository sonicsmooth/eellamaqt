#ifndef QDBIF_H
#define QDBIF_H

#include "libcore.h"
#include "idbif.h"
#include "coreable.h"
#include "loggable.h"

#include <QSqlDatabase>

#include <list>
#include <string>
#include <optional>

// This is the interface between the core and the Qt database

class QSQDbIf : public IDbIf, public Coreable, public Loggable
{
private:
    std::list<std::string> m_activeDb;

    void pushActiveDb(std::string);
    void popActiveDb(std::string);
    void cloneDb(std::string, std::string);
    void renameDb(std::string, std::string);

public:
    QSQDbIf();
    void createDatabase(std::string) override;
    void openDatabase(std::string) override;
    void saveDatabase(std::string, std::string, DupOptions) override;
    std::optional<std::string> activeDatabase() const override; // returns top active Db or null_opt
    void activateDatabase(std::string) override;
    void closeDatabase(std::string) override;
    void deleteDatabase(std::string) override;
    bool isDatabaseOpen(std::string) const override;
    bool isDatabaseOpen() const override;
    QSqlDatabase database(std::string);
};

#endif // DBIF_H
