#ifndef IDBIF_H
#define IDBIF_H

// This is the abstract interface to the interface between the core and the Qt database
//#include "libcore.h"
#include "saveoptions.h"
#include "saveoptions.h"

#include <iostream>
#include <optional>
#include <string>
#include <any>


class IDbIf
{
public:
    virtual ~IDbIf() {}
    virtual void createDatabase(std::string) = 0;
    virtual void openDatabase(std::string) = 0;
    virtual void saveDatabase(std::string, std::string, DupOptions) = 0;
    virtual std::optional<std::string> activeDatabase() const = 0; // returns top active Db or null_opt    
    //virtual bool activeDatabase(std::string) const = 0; // checks if a specific database is the active one
    virtual void closeDatabase(std::string) = 0;
    virtual void deleteDatabase(std::string) = 0;
    virtual bool isDatabaseOpen(std::string) const = 0; // checks if a specific database is open
    virtual bool isDatabaseOpen() const = 0; // checks if any database is open
};

#endif // DBIF_H
