#ifndef __H_TDATA__
#define __H_TDATA__

#include <api/types.h>

struct ThreadData {
    void *_this;
    void *_run;

    StackSize stackSize;
    Time timeSlice;

    unsigned tid;
};

#endif
