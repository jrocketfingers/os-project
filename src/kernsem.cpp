#include <scheduler.h>
#include <kernsem.h>
#include <pcb.h>

#include <debug.h>

/* kernel.cpp */
extern PCB* running;
extern char dont_schedule;


KernSem::KernSem(int val) {
    this->value = val;
}


KernSem::~KernSem() {
    PCB* pcb;
    while(pcb = blocked.get()) {
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
    cout << "[" << sid << "] Semaphore value after signal: " << value << endl;
#endif
}


void KernSem::wait() {
    if(--value < 0) {
        blocked.put(running);
        running->block();
    }

#ifdef DEBUG__SEMAPHORES
    cout << "[" << sid << "] Semaphore value after wait: " << value << endl;
#endif
}
