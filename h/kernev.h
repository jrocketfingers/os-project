#ifndef __H_KERNEV__
#define __H_KERNEV__

#include <api_type.h>
#include <pcb.h>

class KernEv {
public:
    KernEv();
    ~KernEv();

    void signal();
    void wait();

    int val;
    unsigned eid;

    PCB* creator;
};

#endif
