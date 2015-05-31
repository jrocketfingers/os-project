#ifndef __H_KTHREAD__
#define __H_KTHREAD__

#include <tdata.h>


void interrupt syscall(unsigned p_bp, unsigned p_di, unsigned p_si, unsigned p_ds,
                       unsigned p_es, unsigned p_dx, unsigned p_cx, unsigned p_bx,
                       unsigned p_ax, unsigned p_ip, unsigned p_cs, unsigned flags);

class PCB;

class KThread {
public:
    KThread();

    void takeOver(unsigned callID, unsigned data_seg, unsigned data_off);
    //void dispatchSyscall(unsigned callID, void *data);
    //void newThread(ThreadData* data);

private:
    void syscall_dispatch();

    PCB* pcb;
    unsigned callID;
    void* dataPtr;

    StackSize stackSize;
};

#endif
