#ifndef IUIMANAGER_H
#define IUIMANAGER_H

#include "idbif.h"
#include "ilogger.h"

#include <string>

// These functions are called by the core to do bare minimal stuff to the UI.

class LibCore;

class IUIManager
{
public:
    virtual ~IUIManager() {}
    virtual void notifyDbOpen(IDbIf *, std::string) = 0;
    virtual void notifyDbClose(IDbIf *, std::string) = 0;
    virtual void notifyDbRename(IDbIf *, std::string, std::string) = 0;
    virtual void *newWindow() = 0;                      // Creates new top level window
    virtual void *newWindow(LibCore *, ILogger *) = 0;  // Creates new top level window
    virtual void *duplicateWindow() = 0;  // Dupes size/pos without children
    virtual void *duplicateWindow(void *) = 0;  // Dupes size/pos without children
    virtual void closeWindow() = 0; // Closes current top level window
    virtual void closeWindow(void *) = 0; // Closes given window
    virtual void duplicateMainView() = 0;
    virtual void popOutMainView() = 0;
    virtual void closeMainView() = 0;
    virtual std::string activeLibFullPath() = 0;
};

//IUIManager::~IUIManager() = default;
#endif // IUIMANAGER_H
