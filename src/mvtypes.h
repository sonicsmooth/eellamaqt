#ifndef MVTYPES_H
#define MVTYPES_H

#include <vector>
#include <list>


enum class ViewType {INVALID, LIBTREEVIEW, LIBTABLEVIEW, LIBLISTVIEW, LIBSYMBOLVIEW, LIBFOOTPRINTVIEW};
//enum class ModelType {INVALID, SQLTABLEMODEL, SQLTREEMODEL, EXAMPLETREEMODEL};

//this is getting ugly
static std::vector<ViewType> SubViewTypes({ViewType::LIBTREEVIEW, ViewType::LIBTABLEVIEW, ViewType::LIBLISTVIEW});
static std::vector<ViewType> DefaultViewTypes({ViewType::LIBSYMBOLVIEW, ViewType::LIBTREEVIEW, ViewType::LIBTABLEVIEW});

#endif // MVTYPES_H
