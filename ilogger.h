#ifndef ILOGGER_H
#define ILOGGER_H

class ILogger {
public:
    virtual ~ILogger()  {}
    virtual void log(const char *) = 0;
    virtual void log(std::string) = 0;
};

#endif