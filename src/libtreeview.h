#ifndef LIBTREEVIEW_H
#define LIBTREEVIEW_H

#include "libclient.h"
#include <QTreeView>
#include <QAbstractItemModel>
#include <string>

class LibTreeView : public QTreeView, public LibClient
{
public:
    LibTreeView(QWidget * = nullptr);
    LibTreeView(QWidget * = nullptr, QAbstractItemModel * = nullptr);
    LibTreeView(QWidget * = nullptr, QAbstractItemModel * = nullptr, LibCore * = nullptr, ILogger * = nullptr, std::string = std::string());
};


#endif // LIBTABLEVIEW_H
