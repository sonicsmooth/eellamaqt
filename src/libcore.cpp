#include "libcore.h"
#include <iostream>
#include <sstream>
#include <cassert>
#include <filesystem>
#include <fstream>



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
    //log("Active db set to " + activeDb());
}
void LibCore::popActiveDb(std::string adb) {
    // Ensure exactly one adb is in the list
    // and that it's at the front
    m_activeDb.remove(adb);
    if (m_activeDb.size())
        log("Active db set to " + activeDb());
    else
        log("No active db");
}

const std::string & LibCore::activeDb() const {
    return m_activeDb.front();
}
void LibCore::newLib(std::string fullpath) {
    // LibCore assumes the caller has already verified the path and name
    // So we log whether it exsts, then override
    assert(m_pDbIf);
    
    log("LibCore: Creating new Library " + fullpath);
    DbIf()->createDatabase(fullpath);
    pushActiveDb(fullpath);
    UIManager()->OpenUI(UITYPE::LIBVIEW, fullpath);
}
void LibCore::openLib(std::string fullpath) {
    log("LibCore: Opening library " + fullpath);
    pushActiveDb(fullpath);
    UIManager()->OpenUI(UITYPE::LIBVIEW, fullpath);
}
void LibCore::saveLib(std::string fullpath) {
    log("LibCore: Saving library " + fullpath);
    pushActiveDb(fullpath);
}
void LibCore::closeLib(std::string fullpath) {
    log("LibCore: Closing library " + fullpath);
    popActiveDb(fullpath);
    UIManager()->CloseUI(fullpath);
}
void LibCore::deleteLib(std::string fullpath) {
    log("LibCore: Deleting library " + fullpath);
    UIManager()->CloseUI(fullpath);
}

void LibCore::newShape() {
    log("LibCore: Creating a new shape in " + activeDb());
}
void LibCore::newSymbol() {
    log("LibCore: Creating a new symbol in " + activeDb());
}
void LibCore::deleteShape(std::string name) {
    log("LibCore: Deleting shape " + name + " from " + activeDb());
}
void LibCore::deleteSymbol(std::string name) {
    log("LibCore: Deleting shape " + name + " from " + activeDb());
}
void LibCore::renameShape(std::string name) {
    log("LibCore: Renaming shape " + name + " from " + activeDb());
}
void LibCore::renameSymbol(std::string name) {
    log("LibCore: Deleting shape " + name + " from " + activeDb());
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
