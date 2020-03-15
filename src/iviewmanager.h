#ifndef IVIEWMANAGER_H
#define IVIEWMANAGER_H

#include <any>


class IViewManager
{
public:
    virtual ~IViewManager() {}
    virtual std::any createTreeView(std::any model) = 0; // takes a model and returns a view
    virtual std::any createTableView(std::any model) = 0;

};

#endif // IVIEWMANAGER_H
