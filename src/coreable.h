#ifndef ICOREABLE_H
#define ICOREABLE_H

#include "libcore.h"

class Coreable {
    protected:
        LibCore *m_pCore;

    public:
        Coreable(LibCore *pc = nullptr) : m_pCore(pc) {}
        void setCore(LibCore *c) {m_pCore = c;}
        LibCore *core() const {return m_pCore;}
};


#endif
