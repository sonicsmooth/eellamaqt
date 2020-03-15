#ifndef QVIEWMANAGER_H
#define QVIEWMANAGER_H

#include "iviewmanager.h"
#include <any>


class QViewManager : public IViewManager
{
public:
    QViewManager();
    std::any createTreeView(std::any model) override;
    std::any createTableView(std::any model) override;
};

#endif // QVIEWMANAGER_H
