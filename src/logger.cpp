
#include "logger.h"
#include <QString>
#include <memory>
#include <cassert>



void Logger::setTextEdit(QTextEdit* pte) {
	m_pte = pte;
}

void Logger::vlog(const char *fmt, va_list argp) {
    size_t sz = static_cast<size_t>(vsnprintf(nullptr, 0, fmt, argp)) + 1;
    std::unique_ptr<char> buff(new char [sz]);
    vsnprintf(buff.get(), sz, fmt, argp);
    m_pte->append(buff.get());
}

void Logger::log(const char *fmt, ...) {
    assert(m_pte);
    va_list args;
    va_start(args, fmt);
    vlog(fmt, args);
    va_end(args);

}

void Logger::log(std::string msg) {
    assert(m_pte);
    m_pte->append(QString::fromStdString(msg));
}

void Logger::log(std::stringstream & msg) {
    assert(m_pte);
    m_pte->append(QString::fromStdString(msg.str()));
    // Clear stringstream when done
    msg.str(std::string());
}

void Logger::log(QString msg) {
     assert(m_pte);
    m_pte->append(msg);
 }
