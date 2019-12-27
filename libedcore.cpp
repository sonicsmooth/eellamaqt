#include <iostream>
#include <sstream>

#include "libedcore.h"

LibEdCore::LibEdCore() {}
LibEdCore::~LibEdCore() {}

void LibEdCore::setLogger(ILogger* pLogger) {
    m_pLogger = pLogger;
}

void LibEdCore::Create(Shape shape) {
    std::stringstream ss;
    ss << "Creating shape " << shape.name();
    m_pLogger->log(ss.str());
}
void LibEdCore::Create(Symbol symbol) {
    std::stringstream ss;
    ss << "Creating shape " << symbol.name();
    m_pLogger->log(ss.str());
}

