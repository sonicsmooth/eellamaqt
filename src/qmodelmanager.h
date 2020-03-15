#ifndef QMODELMANAGER_H
#define QMODELMANAGER_H

#include "imodelmanager.h"
#include <any>
#include <string>

class QModelManager : public IModelManager
{
public:
    QModelManager();
    std::any createSqlTableModel(std::string) override;
    std::any createSqlTreeModel(std::string) override;
};

#endif // QMODELMANAGER_H
