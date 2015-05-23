#include <iostream.h> /* debugging purposes */

#include <scheduler.h>
#include <kernsem.h>

extern PCB* running;

KernSem::KernSem(int val) {
    this->value = val;
}

KernSem::~KernSem() {
    PCB* pcb;
    while(pcb = blocked.get()) {
        Scheduler::put(pcb);
    }
}

void KernSem::signal() {
    if(value++ < 0) {
        Scheduler::put(blocked.get());
    }
}

void KernSem::wait() {
    if(--value < 0) {
        blocked.put(running);
        running = Scheduler::get();
    }
}
