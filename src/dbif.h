#ifndef DBIF_H
#define DBIF_H

#include "idbif.h"

// This is the interface between the core and the Qt database

class DbIf : public IDbIf
{
private:

public:
    DbIf() = default;
    virtual void createDatabase(std::string) override;
};

#endif // DBIF_H
