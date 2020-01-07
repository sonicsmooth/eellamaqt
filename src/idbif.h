#ifndef IDBIF_H
#define IDBIF_H

// This is the abstract interface to the interface between the core and the Qt database

#include <iostream>

class IDbIf
{
public:
    virtual ~IDbIf() {}
    virtual void createDatabase(std::string) = 0;
};

#endif // DBIF_H
