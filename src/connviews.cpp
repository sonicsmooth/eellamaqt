#include "mvtypes.h"
#include "connviews.h"

#include <filesystem>

// PRIVATE

// This is the generic base case, must be specialized
// By keeping this commented, we can force a linker error
// if the fn is called with an arg for which there is no
// specialization
//template<typename Arg>
//ConnViews UIManager::selectWhere_ext(Arg arg) {
//    (void) cvs;
//    (void) arg;
//    log("Single selectWhere_ext, shouldn't be called directly");
//    throw("Wrong function call");
//}
template<> // specialization
ConnViews ConnViews::selectWhere_base(const ConnViews & cvs, std::string arg) {
    ConnViews retval;
    for (auto cv: ((ConnViews &)cvs))
        if(cv.fullpath == arg)
            retval.push_back(cv);
    return retval;
}
template<> // specialization
ConnViews ConnViews::selectWhere_base(const ConnViews & cvs, const ViewType & arg) {
    ConnViews retval;
    for (auto cv: ((ConnViews &)cvs))
        if(cv.viewType == arg)
            retval.push_back(cv);
    return retval;
}
template<> // specialization
ConnViews ConnViews::selectWhere_base(const ConnViews & cvs, const std::vector<ViewType> & arg) {
    ConnViews retval;
    for (auto cv: ((ConnViews &)cvs))
        if(findViewType(arg, cv.viewType))
            retval.push_back(cv);
    return retval;
}


template<> // specialization
ConnViews ConnViews::selectWhere_base(const ConnViews & cvs, int arg) {
    return ConnViews();
}


template<> // specialization
ConnViews ConnViews::selectWhere_base(const ConnViews & cvs, const QAbstractItemModel *arg) {
    ConnViews retval;
    for (auto cv: ((ConnViews &)cvs))
        if(cv.model == arg)
            retval.push_back(cv);
    return retval;
}
template<> // specialization
ConnViews ConnViews::selectWhere_base(const ConnViews & cvs, const QAbstractItemView *arg) {
    ConnViews retval;
    for (auto cv: ((ConnViews &)cvs))
        if(cv.view == arg)
            retval.push_back(cv);
    return retval;
}
template<> // specialization
ConnViews ConnViews::selectWhere_base(const ConnViews & cvs, const QWidget *arg) {
    ConnViews retval;
    for (auto cv: ((ConnViews &)cvs))
        if(cv.subWidget == arg)
            retval.push_back(cv);
    return retval;
}
template<> // specialization
ConnViews ConnViews::selectWhere_base(const ConnViews & cvs, const QMainWindow *arg) {
    ConnViews retval;
    for (auto cv: ((ConnViews &)cvs))
        if(cv.mainWindow == arg)
            retval.push_back(cv);
    return retval;
}

template <typename Arg, typename... Args>
ConnViews ConnViews::selectWhere_int(const ConnViews & cvs, Arg firstarg, Args... restargs) {
    ConnViews cvs2(selectWhere_base(cvs, firstarg));
    if (cvs2.size())
        return selectWhere_int(cvs2, restargs...);
    else
        return cvs2;
}

// PUBLIC
void ConnViews::log(ILogger *lgr) {
    lgr->log("");
    lgr->log("%-19s  %-13s  %-10s  %-10s  %-10s  %-10s",
        "Fullpath","ViewType","Model","View","SubWidget","Mainwindow");
    for (auto cv : m_cvs) {
        std::string vts(cv.viewType == ViewType::INVALID          ? "INVALID"          :
                        cv.viewType == ViewType::LIBSYMBOLVIEW    ? "LibSymbolView"    :
                        cv.viewType == ViewType::LIBTREEVIEW      ? "LibTreeView"      :
                        cv.viewType == ViewType::LIBTABLEVIEW     ? "LibTableView"     :
                        cv.viewType == ViewType::LIBFOOTPRINTVIEW ? "LibFootprintView" : "really invalid");
    std::string title(std::filesystem::path(cv.fullpath).filename().string());
    lgr->log("%-19s  %-13s  0x%08x  0x%08x  0x%08x  0x%08x",
    title.c_str(), vts.c_str(), cv.model, cv.view, cv.subWidget, cv.mainWindow);

    }
}

// Called from client code for single searches
template <typename... Args>
std::optional<ConnView> ConnViews::selectWhere(Args... allargs) {
    // Returns first item found
    // Does NOT assert that exactly one item was found
    ConnViews cvs(selectWhere_int(m_connViews, allargs...));
    if (cvs.size()) {
        ConnView cv;
        cv = cvs.front();
        return std::move(cv);
    }
    else
        return std::nullopt;
}
template<> // specialization
std::optional<ConnView> ConnViews::selectWhere(CVPredFn & fn) {
    // Return first entry where predicate is true, else nullopt
    for (auto cv : m_cvs)
        if (fn(cv))
            return std::move(cv);
    return std::nullopt;
}

// Called from client code for list searches
template <typename... Args>
ConnViews ConnViews::selectWheres(Args... allargs) {
    return selectWhere_int(m_connViews, allargs...);
}
template<> // specialization
ConnViews ConnViews::selectWheres(CVPredFn & fn) {
    ConnViews retval;
    for (auto cv: m_cvs)
        if(fn(cv))
            retval.push_back(cv);
    return retval;
}

