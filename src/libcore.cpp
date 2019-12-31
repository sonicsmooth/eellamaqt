#include <iostream>
#include <sstream>
#include <cassert>
#include <filesystem>
#include <fstream>
#include "libcore.h"


const char kPathSeparator =
#ifdef _WIN32
                            '\\';
#else
                            '/';
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
void LibCore::newLib(std::string path, std::string name) {
    // LibCore assumes the caller has already verified the path and name
    // So we log whether it exsts, then override
    assert(m_pDbIf);
    assert(m_pLogger);

    std::string fullpath = path + "/" + name;
    m_pDbIf->createDatabase(fullpath);
    m_pLogger->log("Created new Library " + fullpath);
}
void LibCore::openLib(std::string name) {
    assert(m_pLogger);
    std::stringstream ss;
    ss << "Opening library " << name;
    m_pLogger->log(ss);
}
void LibCore::saveLib(std::string name) {
    assert(m_pLogger);
    std::stringstream ss;
    ss << "Saving library... ";
    m_pLogger->log(ss);
}
void LibCore::closeLib(std::string name) {
    assert(m_pLogger);
    std::stringstream ss;
    ss << "Closing library... ";
    m_pLogger->log(ss);
}
void LibCore::deleteLib(std::string name) {
    assert(m_pLogger);
    std::stringstream ss;
    ss << "Deleting library... ";
    m_pLogger->log(ss);
}
void LibCore::create(Shape shape) {
    assert(m_pLogger);
    std::stringstream ss;
    ss << "Creating shape " << shape.name();
    m_pLogger->log(ss);
}
void LibCore::create(Symbol symbol) {
    assert(m_pLogger);
    std::stringstream ss;
    ss << "Creating shape " << symbol.name();
    m_pLogger->log(ss);
}
void LibCore::deleteItem(Symbol symbol) {
    assert(m_pLogger);
    std::stringstream ss;
    ss << "Deleting symbol " << symbol.name();
    m_pLogger->log(ss);
}
void LibCore::deleteItem(Shape shape) {
    assert(m_pLogger);
    std::stringstream ss;
    ss << "Deleting shape " << shape.name();
    m_pLogger->log(ss);
}
