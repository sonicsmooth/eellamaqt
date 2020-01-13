#ifndef IUIMANAGER_H
#define IUIMANAGER_H

#include <any>
#include <string>

// These functions are called by the core to do bare minimal stuff to the UI.

enum class UIType {LIBVIEW};

class IUIManager
{
public:
    virtual ~IUIManager() {}
    virtual std::any openUI(UIType, std::string) = 0;
    virtual void closeUI(std::string) = 0;
    virtual void retargetUI(std::string, std::string) = 0;

};

//IUIManager::~IUIManager() = default;
#endif // IUIMANAGER_H