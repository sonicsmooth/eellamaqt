#ifndef DBIF_H
#define DBIF_H

#include "idbif.h"
#include "coreable.h"
#include "loggable.h"

#include <QtSql>

// This is the interface between the core and the Qt database

class QSQDbIf : public IDbIf, public Coreable, public Loggable
{
private:
    //QSqlDatabase* m_pQDb;
public:
    QSQDbIf();
    virtual void createDatabase(std::string) override;
};

#endif // DBIF_H
