#ifndef ICOREABLE_H
#define ICOREABLE_H

#include "libcore.h"

class Coreable {
    public:
        LibCore *m_pCore;
        Coreable() : m_pCore(nullptr) {};
        void setCore(LibCore *c) {m_pCore = c;};
        LibCore *core() const {return m_pCore;};
};


#endif
