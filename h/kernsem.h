#ifndef __H_KERNSEM__
#define __H_KERNSEM__

#include <queue.h>

class KernSem {
public:
    KernSem(int sem);
    ~KernSem();

    void signal();
    void wait();

    int value;
    Queue<PCB*> blocked;
};

#endif
