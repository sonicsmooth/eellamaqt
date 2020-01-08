#include "libcore.h"
#include <iostream>
#include <sstream>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <optional>


#ifdef _WIN32
const char kPathSeparator = '\\';
#else
const char kPathseparator =  '/';
#endif

namespace fs = std::filesystem;

LibCore::LibCore() {}
LibCore::~LibCore() {}

void LibCore::setDbIf(IDbIf* pDbIf) {
    m_pDbIf = pDbIf;
}

IDbIf *LibCore::DbIf() const {
    return m_pDbIf;
}
void LibCore::setUIManager(IUIManager *uim) {
    m_pUIManager = uim;
}
IUIManager *LibCore::UIManager() const {
    return m_pUIManager;
}

void LibCore::pushActiveDb(std::string adb) {
    // Ensure exactly one adb is in the list
    // and that it's at the front
    m_activeDb.remove(adb);
    m_activeDb.push_front(adb);
    log("Active db set to " + activeDb().value());
}
void LibCore::popActiveDb(std::string adb) {
    // Ensure exactly one adb is in the list
    // and that it's at the front
    m_activeDb.remove(adb);
    if (m_activeDb.size())
        log("Active db set to " + activeDb().value());
    else
        log("No active db to pop");
}
std::optional<std::string> LibCore::activeDb() const {
    return m_activeDb.size() ? std::optional<std::string>{m_activeDb.front()} :
                               std::nullopt;
}
void LibCore::newLib(std::string fullpath) {
    // LibCore assumes the caller has already verified the path and name
    // So we log whether it exsts, then override
    assert(m_pDbIf);
    log("LibCore: Creating new Library " + fullpath);
    DbIf()->createDatabase(fullpath);
    pushActiveDb(fullpath);
    m_pUIManager->OpenUI(UITYPE::LIBVIEW, fullpath);
}
void LibCore::openLib(std::string fullpath) {
    log("LibCore: Opening library " + fullpath);
    pushActiveDb(fullpath);
    m_pUIManager->OpenUI(UITYPE::LIBVIEW, fullpath);
}
void LibCore::saveLib(std::string fullpath) {
    log("LibCore: Saving library " + fullpath);
    pushActiveDb(fullpath);
}
void LibCore::closeLib(std::string fullpath) {
    size_t sz = m_activeDb.size();
    if (sz) {
        log("LibCore: Closing library " + fullpath);
        popActiveDb(fullpath);
        // Should have no effect if call chain originated from UI
        // Because when this fn is called from UI, it's because the
        // final window referring to this lib has been closed
        m_pUIManager->CloseUI(fullpath);
    } else {
        log("LibCore: No active db to close");
    }
}
void LibCore::deleteLib(std::string fullpath) {
    log("LibCore: Deleting library " + fullpath);
    m_pUIManager->CloseUI(fullpath);
}

void LibCore::newShape() {
    if (activeDb())
        log("LibCore: Creating a new shape in " + activeDb().value());
    else
        log("LibCore: Set active db to create new shape");
}
void LibCore::newSymbol() {
    log("LibCore: Creating a new symbol in " + activeDb().value());
}
void LibCore::deleteShape(std::string name) {
    log("LibCore: Deleting shape " + name + " from " + activeDb().value());
}
void LibCore::deleteSymbol(std::string name) {
    log("LibCore: Deleting shape " + name + " from " + activeDb().value());
}
void LibCore::renameShape(std::string name) {
    log("LibCore: Renaming shape " + name + " from " + activeDb().value());
}
void LibCore::renameSymbol(std::string name) {
    log("LibCore: Deleting shape " + name + " from " + activeDb().value());
}

// void LibCore::insertShape(std::string dbConn, std::string symbolName, Shape shape, 
//     double x, double y) {
//     assert(m_pLogger);
//     m_pLogger->log("LibCore: insertShape %s into %s in %s",  
//         shape.name().c_str(), symbolName.c_str(), dbConn.c_str());
// }
// void LibCore::removeShape(std::string dbConn, std::string symbolName, std::string shapeName) {
//     assert(m_pLogger);
//     m_pLogger->log("LibCore: removeShape" + shapeName + " from " + symbolName);
// }
