#ifndef __H_ITHREAD__
#define __H_ITHREAD__

#include <pcb.h>

class IThread {
public:
    IThread();
    ~IThread();

    void takeOver();

    PCB* pcb;
};

#endif
