#ifndef __H_KTHREAD__
#define __H_KTHREAD__

#include <pcb.h>

void interrupt syscall(unsigned p_bp, unsigned p_di, unsigned p_si, unsigned p_ds,
                       unsigned p_es, unsigned p_dx, unsigned p_cx, unsigned p_bx,
                       unsigned p_ax, unsigned p_ip, unsigned p_cs, unsigned flags);

class KThread {
public:
    KThread();

private:
    void syscall_dispatch();

    PCB* pcb;
    unsigned callID;
    void* dataPtr;
}

#endif
