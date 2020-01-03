#ifndef ILOGGER_H
#define ILOGGER_H

#include <iostream>
#include <sstream>

class ILogger {
public:
    virtual ~ILogger()  {}
    virtual void log(const char *, ...) = 0; // Use like printf
    virtual void log(std::string) = 0;
    virtual void log(std::stringstream &) = 0; // clears string when done
};

#endif
