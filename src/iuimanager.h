#ifndef IUIMANAGER_H
#define IUIMANAGER_H

#include <any>
#include <string>

enum class UITYPE {LIBVIEW};

class IUIManager
{
public:
    virtual ~IUIManager() {}
    virtual std::any OpenUI(UITYPE, std::string) = 0;
    virtual void CloseUI(std::string) = 0;
};

//IUIManager::~IUIManager() = default;
#endif // IUIMANAGER_H
