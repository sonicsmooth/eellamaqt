#include "libcore.h"
#include <QSqlDatabase>
#include <iostream>
#include <sstream>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <optional>
#include <algorithm>
#include <any>


#ifdef _WIN32
const char kPathSeparator = '\\';
#else
const char kPathseparator =  '/';
#endif

const QString GLibDir("EELLama Libraries");


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
void LibCore::newLib(std::string fullpath) {
    // LibCore assumes the caller has already verified the path and name
    // So we log whether it exsts, then override
    assert(m_pDbIf);
    // TODO: check for an existing lib with that name
    m_pDbIf->createDatabase(fullpath);
    if (m_pUIManager)
        m_pUIManager->notifyDbOpen(m_pDbIf, fullpath);
}
void LibCore::openLib(std::string fullpath) {
    assert(m_pDbIf);
    //log("LibCore::openLib Opening library " + fullpath);
    if (m_pDbIf->isDatabaseOpen(fullpath)) {
        log("Library %s already open", fullpath.c_str());
    } else {
        m_pDbIf->openDatabase(fullpath);
        if (m_pUIManager)
            m_pUIManager->notifyDbOpen(m_pDbIf, fullpath);
    }
}



void LibCore::saveLib(std::string oldpath, std::string newpath, DupOptions opt) {
    // Typically called from the rename menu item
    // Should only be called with oldpath in activeDb list.
    assert(m_pDbIf);
    try {
        m_pDbIf->saveDatabase(oldpath, newpath, opt);
    } catch (std::filesystem::filesystem_error err) {
        log("LibCore::saveLib: Failed to save library with some option");
    }

    try {
        switch(opt) {
        case DupOptions::CLOSE_OLD:
            // Use existing UI for new, close old one
            log("LibCore::saveLib: Saving library CLOSE_OLD" + newpath);
            if (m_pUIManager)
                // notifyDbRename must know what to do if oldpath is not currently open
                m_pUIManager->notifyDbRename(m_pDbIf, oldpath, newpath);
            break;
        case DupOptions::OPEN_NEW:
            // Keep old one open, open new one too
            log("LibCore::saveLib: Saving library OPEN_NEW " + newpath);
            if (m_pUIManager)
                m_pUIManager->notifyDbOpen(m_pDbIf, newpath);
            break;
        case DupOptions::QUIETLY:
            // Neither open new one nor close old one
            log("LibCore::saveLib: Saving library QUIETLY " + newpath);
            // No need to tell UI anything
            break;
        case DupOptions::RENAME:
            log("LibCore::saveLib: Saving library RENAME " + newpath);
            if (m_pUIManager)
                m_pUIManager->notifyDbRename(m_pDbIf, oldpath, newpath);
            break;
        }
    }
    catch (...) {
        log("Could not notify UI of file save");
    }
}

void LibCore::closeLib(std::string fullpath) {
    // Remove specified db from list and call UI to close related windows
    // UI must be closed before db or db won't close properly
    // Call dbif to close lib
    // Caller should know the proper name of lib; throws error otherwise
    assert(m_pDbIf);
    log("LibCore::closeLib Closing library %s", fullpath.c_str());
    try {
        if (m_pUIManager)
            m_pUIManager->notifyDbClose(m_pDbIf, fullpath);
        // If this goes first, then there are notifications
        m_pDbIf->closeDatabase(fullpath);

    } catch (...) {
        log("Could not close library");
    }
}
 void LibCore::closeActiveLib() {
     // Get active lib and foward to closeLib
     assert(m_pDbIf);
     if (m_pDbIf->isDatabaseOpen()) {
         log("LibCore::closeActiveLib");
         closeLib(m_pDbIf->activeDatabase().value());
     } else {
         log("LibCore::closeActiveLib No active db to close");
     }
 }
 void LibCore::deleteLib(std::string fullpath) {
     // Remove specified db from list and call UI to close related windows
     // Call dbif to delete lib
     // Caller should know the proper name of lib; throws error otherwise
     assert(m_pDbIf);
     if (m_pDbIf->isDatabaseOpen(fullpath)) {
         log("LibCore::deleteLib " + fullpath);
         m_pDbIf->deleteDatabase(fullpath);
         if (m_pUIManager)
             m_pUIManager->notifyDbClose(m_pDbIf, fullpath);
     } else {
         throw std::logic_error("LibCore: No active db to close");
     }
 }
 void LibCore::deleteActiveLib() {
     // Get active lib and foward to deleteLib
     assert(m_pDbIf);
     if (m_pDbIf->isDatabaseOpen()) {
         log("LibCore::deleteActiveLib");
         deleteLib(m_pDbIf->activeDatabase().value());
     } else {
         log("LibCore: No active db to delete");
     }
 }

void LibCore::newShape() {
//     if (activeDb())
//         log("LibCore: Creating a new shape in " + activeDb().value());
//     else
//         log("LibCore: Set active db to create new shape");
}
 void LibCore::newSymbol() {
//     log("LibCore: Creating a new symbol in " + activeDb().value());
}
void LibCore::deleteShape(std::string name) {
//     log("LibCore: Deleting shape " + name + " from " + activeDb().value());
}
void LibCore::deleteSymbol(std::string name) {
//     log("LibCore: Deleting shape " + name + " from " + activeDb().value());
}
void LibCore::renameShape(std::string name) {
//     log("LibCore: Renaming shape " + name + " from " + activeDb().value());
}
void LibCore::renameSymbol(std::string name) {
//     log("LibCore: Deleting shape " + name + " from " + activeDb().value());
}

//void LibCore::insertShape(std::string dbConn, std::string symbolName, Shape shape,
//    double x, double y) {
//     assert(m_pLogger);
//     m_pLogger->log("LibCore: insertShape %s into %s in %s",
//         shape.name().c_str(), symbolName.c_str(), dbConn.c_str());
// }
// void LibCore::removeShape(std::string dbConn, std::string symbolName, std::string shapeName) {
//     assert(m_pLogger);
//     m_pLogger->log("LibCore: removeShape" + shapeName + " from " + symbolName);
//}
