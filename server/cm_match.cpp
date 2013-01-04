#include "cm_match.h"

namespace {
    DBID _idmax = 0;
}

void initCmatch() {
    //todo: load _idmax
    _idmax = 1;
}

DBID genDBID() {
    return ++_idmax;
}