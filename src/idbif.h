#ifndef IDBIF_H
#define IDBIF_H

// This is the abstract interface to the interface between the core and the Qt database

#include <iostream>
//#include <optional>

class IDbIf
{
public:
    virtual ~IDbIf() {}
    virtual void createDatabase(std::string) = 0;
    virtual void openDatabase(std::string) = 0;
    virtual void cloneDatabase(std::string, std::string) = 0;
    virtual void renameDatabase(std::string, std::string) = 0;
    virtual void closeDatabase(std::string) = 0;
    virtual void deleteDatabase(std::string) = 0;
};

#endif // DBIF_H
