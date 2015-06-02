#include <scheduler.h>
#include <kernsem.h>
#include <pcb.h>

#include <debug.h>

extern void _dispatch();

KernSem::KernSem(int val) {
    this->value = val;
}


KernSem::~KernSem() {
    PCB* pcb;
    while(!blocked.empty()) {
        pcb = blocked.get();
        Scheduler::put(pcb);
        pcb->unblock();
    }
}


void KernSem::signal() {
    if(value++ < 0) {
        PCB* pcb;
        pcb = blocked.get();
        Scheduler::put(pcb);
        pcb->unblock();
    }

    #ifdef DEBUG__SEMAPHORES
    cout << "[" << sid << "] Semaphore value after signal: " << value << endl << flush;
    #endif
}


void KernSem::wait() {
    if(--value < 0) {
        blocked.put(Kernel::running);
        Kernel::running->block();
        _dispatch();
    }

    #ifdef DEBUG__SEMAPHORES
    cout << "[" << sid << "] Semaphore value after wait: " << value << endl << flush;
    #endif
}
