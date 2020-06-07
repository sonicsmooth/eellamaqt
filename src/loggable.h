#ifndef ILOGGABLE_H
#define ILOGGABLE_H

#include "ilogger.h"
#include "logger.h"


// This class gets derived-from so the deriver doesn't 
// have to instantiate a Logger *, setLogger(), etc.
// Any class that wants to output text should derive
// from here, and it will have setLogger() and log()
// functions.


class Loggable {
    public:
        Loggable(ILogger * = nullptr);
        ~Loggable();

        ILogger *m_pLogger;
        void setLogger(ILogger *);
        ILogger *logger() const;
        
        // Just forward log to embedded logger
        void log(const char *, ...);
        void log(std::string);
        void log(std::stringstream &); // clears string when done
};


#endif
