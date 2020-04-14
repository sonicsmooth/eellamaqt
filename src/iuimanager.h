#ifndef IUIMANAGER_H
#define IUIMANAGER_H

#include "idbif.h"

//#include <any>
#include <string>

// These functions are called by the core to do bare minimal stuff to the UI.

enum class UIType {LIBTREEVIEW, LIBTABLEVIEW, LIBSYMBOLEDITORVIEW, LIBFOOTPRINTEDITORVIEW};

class IUIManager
{
public:
    virtual ~IUIManager() {}
    // Tell UIManager that a db has been opened/closed/renamed
    virtual void notifyDbOpen(IDbIf *, std::string) = 0;
    virtual void notifyDbClose(IDbIf *, std::string) = 0;
    virtual void notifyDbRename(IDbIf *, std::string, std::string) = 0;

};

//IUIManager::~IUIManager() = default;
#endif // IUIMANAGER_H
