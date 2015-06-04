#ifndef __H_TDATA__
#define __H_TDATA__

#include <api_type.h>

struct ThreadData {
    void *_this;
    void *_run;

    StackSize stackSize;
    Time timeSlice;

    unsigned tid;
};

#endif
