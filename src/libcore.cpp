#include <iostream>
#include <sstream>
#include <cassert>
#include <filesystem>
#include <fstream>
#include "libcore.h"


#ifdef _WIN32
const char kPathSeparator = '\\';
#else
const char kPathseparator =  '/';
#endif

namespace fs = std::filesystem;

LibCore::LibCore() : m_pLogger(nullptr) {}
LibCore::~LibCore() {}

void LibCore::setLogger(ILogger* pLogger) {
    m_pLogger = pLogger;
}
void LibCore::setDbIf(IDbIf* pDbIf) {
    m_pDbIf = pDbIf;
}
void LibCore::newLib(std::string fullpath) {
    // LibCore assumes the caller has already verified the path and name
    // So we log whether it exsts, then override
    assert(m_pDbIf);
    assert(m_pLogger);

    m_pLogger->log("LibCore: Creating new Library " + fullpath);
    m_pDbIf->createDatabase(fullpath);
}
void LibCore::openLib(std::string name) {
    assert(m_pLogger);
    m_pLogger->log("LibCore: Opening library " + name);
}
void LibCore::saveLib(std::string name) {
    assert(m_pLogger);
    m_pLogger->log("LibCore: Saving library " + name);
}
void LibCore::closeLib(std::string name) {
    assert(m_pLogger);
    m_pLogger->log("LibCore: Closing library " + name);
}
void LibCore::deleteLib(std::string name) {
    assert(m_pLogger);
    m_pLogger->log("LibCore: Deleting library " + name);
}
void LibCore::create(Shape shape) {
    assert(m_pLogger);
    m_pLogger->log("LibCore: Creating shape " + shape.name());
}
void LibCore::create(Symbol symbol) {
    assert(m_pLogger);
    m_pLogger->log("LibCore: Creating symbol " + symbol.name());
}
void LibCore::deleteItem(Shape shape) {
    assert(m_pLogger);
    m_pLogger->log("LibCore: Deleting shape " + shape.name());
}
void LibCore::deleteItem(Symbol symbol) {
    assert(m_pLogger);
    m_pLogger->log("LibCore: Deleting symbol " + symbol.name());
}
