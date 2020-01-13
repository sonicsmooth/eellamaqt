#include "libcore.h"
#include <iostream>
#include <sstream>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <optional>
#include <algorithm>


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
std::optional<std::string> LibCore::activeDb() const {
    return m_activeDb.size() ? std::optional<std::string>{m_activeDb.front()} :
                               std::nullopt;
}
bool LibCore::activeDb(std::string fullpath) const {
    // Returns wheter fullpath is in active db
    return std::find(m_activeDb.begin(), m_activeDb.end(), fullpath) != m_activeDb.end();
}
void LibCore::pushActiveDb(std::string adb) {
    // Ensure exactly one adb is in the list
    // and that it's at the front
    m_activeDb.remove(adb);
    m_activeDb.push_front(adb);
    log("Active db set to " + activeDb().value());
}
void LibCore::popActiveDb(std::string adb) {
    // Remove adb from activeDb list;
    // throw error if not in list
    if (activeDb(adb)) {
        log("LibCore::popActiveDb: Popping " + adb);
        m_activeDb.remove(adb);
        if (activeDb()) {
            log("LibCore::popActiveDb: Active db set to " + activeDb().value());
        }
        else {
            log("LibCore::popActiveDb: Active db now empty");
        }
    } else {
        throw std::runtime_error("Library " + adb + " not in activeDb list");
    }
}
void LibCore::newLib(std::string fullpath) {
    // LibCore assumes the caller has already verified the path and name
    // So we log whether it exsts, then override
    assert(m_pDbIf);
    log("LibCore::newLib Creating new Library " + fullpath);
    // TODO: check for an existing lib with that name
    m_pDbIf->createDatabase(fullpath);
    pushActiveDb(fullpath);
    // TODO: somehow point UI to database
    m_pUIManager->openUI(UIType::LIBVIEW, fullpath);
}
void LibCore::openLib(std::string fullpath) {
    log("LibCore::openLib Opening library " + fullpath);
    if (activeDb(fullpath)) {
        log("Library %s already open", fullpath.c_str());
    } else {
        m_pDbIf->openDatabase(fullpath);
        m_pUIManager->openUI(UIType::LIBVIEW, fullpath);
        pushActiveDb(fullpath);
    }
}


void LibCore::saveLib(std::string oldpath, std::string newpath, DupOptions opt) {
    // Should only be cassed with oldpath in activeDb list
    assert(activeDb(oldpath));
    log("LibCore::saveLib: Fake saving library from %s to %s with option %d ",
        oldpath.c_str(), newpath.c_str(), opt);
    switch(opt) {
    case DupOptions::CLOSE_OLD:
        // Use existing UI for new, close old one
        m_pDbIf->cloneDatabase(oldpath, newpath);
        m_pDbIf->closeDatabase(oldpath);
        m_pDbIf->openDatabase(newpath);
        m_pUIManager->retargetUI(oldpath, newpath);
        popActiveDb(oldpath);
        pushActiveDb(newpath);
        break;
    case DupOptions::OPEN_NEW:
        // Keep old one open, open new one too
        m_pDbIf->cloneDatabase(oldpath, newpath);
        m_pDbIf->openDatabase(newpath);
        m_pUIManager->openUI(UIType::LIBVIEW, newpath);
        pushActiveDb(newpath);
        break;
    case DupOptions::QUIETLY:
        m_pDbIf->cloneDatabase(oldpath, newpath);
        break;
    case DupOptions::RENAME:
        m_pDbIf->closeDatabase(oldpath);
        std::rename(oldpath.c_str(), newpath.c_str());
        m_pDbIf->openDatabase(newpath);
        popActiveDb(oldpath);
        pushActiveDb(newpath);
        break;
    }
}



void LibCore::closeLib(std::string fullpath) {
    // Remove specified db from list and call UI to close related windows
    // Call dbif to close lib
    // Caller should know the proper name of lib; throws error otherwise
   if (activeDb(fullpath)) {
        log("LibCore::closeLib Closing library %s", fullpath.c_str());
        popActiveDb(fullpath);
        m_pDbIf->closeDatabase(fullpath);
        m_pUIManager->closeUI(fullpath);
    } else {
        throw std::logic_error("LibCore::closeLib: No active db named this to close: " + fullpath);
    }
}
void LibCore::closeActiveLib() {
    // Get active lib and foward to closeLib
    if (activeDb()) {
        closeLib(activeDb().value());
    } else {
        log("LibCore::closeActiveLib No active db to close");
    }
}
void LibCore::deleteLib(std::string fullpath) {
    // Remove specified db from list and call UI to close related windows
    // Call dbif to delete lib
    // Caller should know the proper name of lib; throws error otherwise
    if (activeDb(fullpath)) {
        log("LibCore: Deleting library " + fullpath);
        popActiveDb(fullpath); // this before closeUI
        m_pDbIf->deleteDatabase(fullpath);
        m_pUIManager->closeUI(fullpath);
    } else {
        throw std::logic_error("LibCore: No active db to close");
    }
}
void LibCore::deleteActiveLib() {
    // Get active lib and foward to deleteLib
    if (activeDb()) {
        deleteLib(activeDb().value());
    } else {
        log("LibCore: No active db to delete");
    }
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
