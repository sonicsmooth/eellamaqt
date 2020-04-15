#ifndef IUIMANAGER_H
#define IUIMANAGER_H

#include "imodelmanager.h"
#include "iviewmanager.h"
#include "idbif.h"

#include <string>

// These functions are called by the core to do bare minimal stuff to the UI.

class IUIManager
{
public:
    virtual ~IUIManager() {}
    virtual void notifyDbOpen(IDbIf *, std::string) = 0;
    virtual void notifyDbClose(IDbIf *, std::string) = 0;
    virtual void notifyDbRename(IDbIf *, std::string, std::string) = 0;

};

//IUIManager::~IUIManager() = default;
#endif // IUIMANAGER_H
