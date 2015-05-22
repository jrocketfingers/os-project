#ifndef __H_SDATA__
#define __H_SDATA__

struct SemaphoreData {
    bool wait;
    unsigned int sid;
    int val;
};

#endif