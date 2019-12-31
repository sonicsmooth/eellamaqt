#ifndef DBIF_H
#define DBIF_H

#include "idbif.h"
#include "logger.h"

// This is the interface between the core and the Qt database

class DbIf : public IDbIf
{
private:
    Logger *m_pLogger;
public:
    DbIf();
    void setLogger(Logger *);
    virtual void createDatabase(std::string) override;
};

#endif // DBIF_H
