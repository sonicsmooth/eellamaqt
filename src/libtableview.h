#ifndef LIBTABLEVIEW_H
#define LIBTABLEVIEW_H

#include "libclient.h"
#include <QTableView>
#include <QAbstractItemModel>
#include <string>

class LibTableView : public QTableView, public LibClient
{
public:
    LibTableView(QWidget * = nullptr);
    LibTableView(QWidget * = nullptr, QAbstractItemModel * = nullptr);
    LibTableView(QWidget * = nullptr, QAbstractItemModel * = nullptr, LibCore * = nullptr, ILogger * = nullptr, std::string = std::string());
};


#endif // LIBTABLEVIEW_H
