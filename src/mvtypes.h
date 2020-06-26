#ifndef MVTYPES_H
#define MVTYPES_H

#include <vector>

enum class ViewType {INVALID, LIBTREEVIEW, LIBTABLEVIEW, LIBLISTVIEW, LIBSYMBOLVIEW, LIBFOOTPRINTVIEW};
//enum class ModelType {INVALID, SQLTABLEMODEL, SQLTREEMODEL, EXAMPLETREEMODEL};

//this is getting ugly
static std::vector<ViewType> MainViewTypes({ViewType::LIBSYMBOLVIEW, ViewType::LIBFOOTPRINTVIEW});
static std::vector<ViewType> SubViewTypes({ViewType::LIBTREEVIEW, ViewType::LIBTABLEVIEW, ViewType::LIBLISTVIEW});
static std::vector<ViewType> DefaultViewTypes({ViewType::LIBSYMBOLVIEW, ViewType::LIBTREEVIEW, ViewType::LIBTABLEVIEW});

template <typename COLL, typename ELEM>
static bool findViewType(COLL coll, ELEM thing) {
    return std::find(coll.begin(), coll.end(), thing) != coll.end();
}

#endif // MVTYPES_H
