#include <iostream>
#include <sstream>

#include "libcore.h"

LibCore::LibCore() {}
LibCore::~LibCore() {}

void LibCore::setLogger(ILogger* pLogger) {
    m_pLogger = pLogger;
}

void LibCore::newLib(std::string name) {
    std::stringstream ss;
    ss << "Creating new library " << name;
    m_pLogger->log(ss.str());
}
void LibCore::openLib(std::string name) {
    std::stringstream ss;
    ss << "Opening library " << name;
    m_pLogger->log(ss.str());
}
void LibCore::create(Shape shape) {
    std::stringstream ss;
    ss << "Creating shape " << shape.name();
    m_pLogger->log(ss.str());
}
void LibCore::create(Symbol symbol) {
    std::stringstream ss;
    ss << "Creating shape " << symbol.name();
    m_pLogger->log(ss.str());
}

