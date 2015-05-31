#ifndef __H_ITHREAD__
#define __H_ITHREAD__

/* forward declaration */
class PCB;

class IThread {
public:
    IThread();
    ~IThread();

    void takeOver();

    PCB* pcb;
};

#endif
