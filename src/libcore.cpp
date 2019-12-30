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


    std::string fullname = path + kPathSeparator + name;

    assert(m_pLogger);
    std::stringstream ss;
    ss << "Looking for files in " << path;
    m_pLogger->log(ss.str());
    ss.str(std::string());

    for (const auto& entry : fs::directory_iterator(path))
        ss << entry.path() << std::endl;

    m_pLogger->log(ss.str());

    assert(m_pDbIf);
    m_pDbIf->createDatabase(name);

    std::ofstream f;
    f.open(fullname, std::fstream::out);
    f << "hi there";
    f.close();
    ss << "Created new library: " << name;

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
