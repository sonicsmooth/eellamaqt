#ifndef LIBCLIENT_H
#define LIBCLIENT_H

#include "coreable.h"
#include "loggable.h"
#include "connable.h"
#include "libcore.h"
#include "ilogger.h"
#include <string>
#include <QAbstractItemView>


// The purpose of this class is to bring together the below three
// base classes

class LibClient : public Coreable, public Loggable, public Connable
{
public:
    LibClient(LibCore *pc = nullptr, ILogger *pl = nullptr, std::string connpath = std::string()) :
        Coreable(pc), Loggable(pl), Connable(connpath) {}
};


#endif // LIBCLIENT_H
