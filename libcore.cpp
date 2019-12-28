#include <iostream>
#include <sstream>
#include <cassert>
#include "libcore.h"

LibCore::LibCore() : m_pLogger(nullptr) {}
LibCore::~LibCore() {}

void LibCore::setLogger(ILogger* pLogger) {
    m_pLogger = pLogger;
}

void LibCore::newLib(std::string name) {
    std::stringstream ss;
    ss << "Creating new library " << name;
    assert(m_pLogger);
    m_pLogger->log(ss.str());
}
void LibCore::openLib(std::string name) {
    std::stringstream ss;
    ss << "Opening library " << name;
    assert(m_pLogger);
    m_pLogger->log(ss.str());
}
void LibCore::saveLib(std::string name) {
    std::stringstream ss;
    ss << "Saving library... ";
    assert(m_pLogger);
    m_pLogger->log(ss.str());
}
void LibCore::closeLib(std::string name) {
    std::stringstream ss;
    ss << "Closing library... ";
    assert(m_pLogger);
    m_pLogger->log(ss.str());
}
void LibCore::deleteLib(std::string name) {
    std::stringstream ss;
    ss << "Deleting library... ";
    assert(m_pLogger);
    m_pLogger->log(ss.str());
}
void LibCore::create(Shape shape) {
    std::stringstream ss;
    ss << "Creating shape " << shape.name();
    assert(m_pLogger);
    m_pLogger->log(ss.str());
}
void LibCore::create(Symbol symbol) {
    std::stringstream ss;
    ss << "Creating shape " << symbol.name();
    assert(m_pLogger);
    m_pLogger->log(ss.str());
}
void LibCore::deleteItem(Symbol symbol) {
    std::stringstream ss;
    ss << "Deleting symbol " << symbol.name();
    assert(m_pLogger);
    m_pLogger->log(ss.str());
}
void LibCore::deleteItem(Shape shape) {
    std::stringstream ss;
    ss << "Deleting shape " << shape.name();
    assert(m_pLogger);
    m_pLogger->log(ss.str());
}
