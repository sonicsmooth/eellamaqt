#ifndef __CONNVIEWS_H__
#define __CONNVIEWS_H__

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
    ConnViewList m_cvs;

    // Base case for recursion
    template <typename Arg>
    ConnViews selectWhere_base(const ConnViews &, Arg);

    // Internal recursive case
    template <typename Arg, typename... Args>
    ConnViews selectWhere_int(const ConnViews &, Arg, Args... );

    public:
    void log(ILogger *);
    ConnViewList::reference front() {return m_cvs.front();}
    ConnViewList::reference back() {return m_cvs.back();}
    ConnViewList::iterator begin() {return m_cvs.begin();}
    ConnViewList::iterator end() {return m_cvs.end();}
    size_t size() const {return m_cvs.size();}
    void remove(const ConnView & cv) {m_cvs.remove(cv);}
    void push_back(const ConnView & cv) {m_cvs.push_back(cv);}

    // Called from client code for single searches
    template <typename... Args>
    std::optional<ConnView> selectWhere(Args... );

    // Called from client code for list searches
    template <typename... Args>
    ConnViews selectWheres(Args... );

    

};


#endif