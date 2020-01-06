#include "ilogger.h"
#include "loggable.h"

Loggable::Loggable() : m_pLogger(nullptr) {};
void Loggable::setLogger(ILogger *lgr) {m_pLogger = lgr;};
ILogger *Loggable::logger() const {return m_pLogger;};

void Loggable::log(const char *fmt, ...) {
    if (m_pLogger) {
        va_list args;
        va_start(args, fmt);
        m_pLogger->log(fmt, args);
        va_end(args);
    }
}

void Loggable::log(std::string s) {
    if (m_pLogger)
        m_pLogger->log(s);
}
void Loggable::log(std::stringstream &s) {
    if (m_pLogger)
        m_pLogger->log(s);
}
// void Loggable::log(QString s){
//     if (m_pLogger)
//         m_pLogger->log(s);
// }
