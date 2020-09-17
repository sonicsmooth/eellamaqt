#ifndef CONNVIEWS_H
#define CONNVIEWS_H

#include "mvtypes.h"
#include "ilogger.h"

#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QMainWindow>
#include <QWidget>

#include <string>
#include <list>
#include <optional>
#include <functional>
#include <filesystem>

typedef struct connview {
    std::string fullpath;
    ViewType viewType;
    QAbstractItemModel *model;
    QAbstractItemView *view;
    QWidget *subWidget; // could be QMdiSubWindow or QDockWidget
    QMainWindow *mainWindow;
    // operator== is used for removing a struct from m_connViews
    bool operator==(const struct connview& a) const  {
         return ( a.fullpath == this->fullpath && 
                  a.viewType == this->viewType &&
                  a.model == this->model &&
                  a.view  == this->view &&
                  a.subWidget == this->subWidget &&
                  a.mainWindow == this->mainWindow);
    }
    bool operator!=(const struct connview& a) const  {
         return ( a.fullpath != this->fullpath ||
                  a.viewType != this->viewType ||
                  a.model != this->model ||
                  a.view  != this->view ||
                  a.subWidget != this->subWidget ||
                  a.mainWindow != this->mainWindow);
    }
} ConnView;

typedef std::list<ConnView> ConnViewList;
typedef const std::function<bool (const ConnView &)> CVPredFn;

class ConnViews {
private:
    ConnViewList m_cvl;

    // Base case for recursion
    template<typename Arg>
    ConnViews _selectWhere(const ConnViewList & cvl, Arg arg) {
    (void) cvl;
    (void) arg;
    throw("Wrong function call");
    }
    template<> ConnViews _selectWhere(const ConnViewList & cvl, std::string arg) {
        ConnViews retval;
        for (auto cv: cvl)
            if(cv.fullpath == arg)
                retval.push_back(cv);
        return retval;
    }
    template<> ConnViews _selectWhere(const ConnViewList & cvl, const ViewType & arg) {
        ConnViews retval;
        for (auto cv: cvl)
            if(cv.viewType == arg)
                retval.push_back(cv);
        return retval;
    }
    template<> ConnViews _selectWhere(const ConnViewList & cvl, const std::vector<ViewType> & arg) {
        ConnViews retval;
        for (auto cv: cvl)
            if(findViewType(arg, cv.viewType))
                retval.push_back(cv);
        return retval;
    }
    template<> ConnViews _selectWhere(const ConnViewList & cvl, const QAbstractItemModel *arg) {
        ConnViews retval;
        for (auto cv: cvl)
            if(cv.model == arg)
                retval.push_back(cv);
        return retval;
    }
    template<> ConnViews _selectWhere(const ConnViewList & cvl, const QAbstractItemView *arg) {
        ConnViews retval;
        for (auto cv: cvl)
            if(cv.view == arg)
                retval.push_back(cv);
        return retval;
    }
    template<> ConnViews _selectWhere(const ConnViewList & cvl, const QWidget *arg) {
        ConnViews retval;
        for (auto cv: cvl)
            if(cv.subWidget == arg)
                retval.push_back(cv);
        return retval;
    }
    template<> ConnViews _selectWhere(const ConnViewList & cvl, const QMainWindow *arg) {
        ConnViews retval;
        for (auto cv: cvl)
            if(cv.mainWindow == arg)
                retval.push_back(cv);
        return retval;
    }
    template<> ConnViews _selectWhere(const ConnViewList & cvl, int arg) {
        (void) cvl;
        (void) arg;
        return ConnViews();
    }

    // Internal recursive case
    template <typename Arg, typename... Args>
    ConnViews _selectWhere(const ConnViewList & cvl, Arg firstarg, Args... restargs) {
        ConnViews cvs2(_selectWhere(cvl, firstarg));
        if (cvs2.size())
            return _selectWhere(cvs2.coll(), restargs...);
        else
            return cvs2;
    }

public:
    void log(ILogger *);
    const ConnViewList & coll() {return m_cvl;}
    ConnViewList::reference front() {return m_cvl.front();}
    ConnViewList::reference back() {return m_cvl.back();}
    ConnViewList::iterator begin() {return m_cvl.begin();}
    ConnViewList::iterator end() {return m_cvl.end();}
    size_t size() const {return m_cvl.size();}
    void remove(const ConnView & cv) {m_cvl.remove(cv);}
    void push_back(const ConnView & cv) {m_cvl.push_back(cv);}

    // Called from client code for single searches
    template <typename... Args> std::optional<ConnView> selectWhere(Args... allargs) {
        // Returns first item found
        // Does NOT assert that exactly one item was found
        //ConnViews cvs(_selectWhere(m_cvl, allargs...));
        ConnViews cvs;
        _selectWhere(m_cvl, allargs...);
        if (cvs.size()) {
            ConnView cv;
            cv = cvs.front();
            return std::move(cv);
        }
        else
            return std::nullopt;
    }
    template<>                  std::optional<ConnView> selectWhere(CVPredFn & fn) {
        // Return first entry where predicate is true, else nullopt
        for (auto cv : m_cvl)
            if (fn(cv))
                return std::move(cv);
        return std::nullopt;
    }

    // Called from client code for list searches
    template <typename... Args> ConnViews selectWheres(Args... allargs) {
        return _selectWhere(m_cvl, allargs...);
    }
    template<>                  ConnViews selectWheres(CVPredFn & fn) {
        ConnViews retval;
        for (auto cv: m_cvl)
            if(fn(cv))
                retval.push_back(cv);
        return retval;
    }

    // Test declarations
    template <typename Arg>                   int fake(Arg);
    template <typename Arg, typename... Args> int fake(Arg, Args...);


};









#endif
