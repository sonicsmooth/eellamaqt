#ifndef IMODELMANAGER_H
#define IMODELMANAGER_H

#include <any>
#include <string>

class IModelManager
{
public:
    virtual ~IModelManager() {}
    virtual std::any createSqlTableModel(std::string) = 0;
    virtual std::any createSqlTreeModel(std::string) = 0;
};

#endif // IMODELMANAGER_H
