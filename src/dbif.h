#ifndef DBIF_H
#define DBIF_H

#include "idbif.h"
#include "logger.h"

#include <QtSql>

// This is the interface between the core and the Qt database

class QSQDbIf : public IDbIf
{
private:
    Logger *m_pLogger;
    //QSqlDatabase* m_pQDb;
public:
    QSQDbIf();
    void setLogger(Logger *);
    virtual void createDatabase(std::string) override;
};

#endif // DBIF_H
