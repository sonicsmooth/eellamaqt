#include <list>
#include "mvtypes.h"


class CJunk {
public:
    template<typename T, typename U>
    int fun1(T, U);
    template<typename... T>
    int fun2(T...);
};

template<typename T, typename U>
int CJunk::fun1(T arg1, U arg2) {
    (void) arg1;
    (void) arg2;
    return 0;
}


template<typename... T>
int CJunk::fun2(T... arg) {
    fun1(arg...);
    return 0;
}

void mainxx() {
    CJunk cj;
    cj.fun2(10, MainViewTypes);
}

