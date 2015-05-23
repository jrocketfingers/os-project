#include <iostream.h> /* debugging purposes */

#include <scheduler.h>
#include <kernsem.h>
#include <pcb.h>

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
    cout << "Running thread " << running->id << " trying to wait on SID: " << sid << endl;
    if(--value < 0) {
        cout << "Running thread " << running->id << " blocked on SID: " << sid << endl;
        blocked.put(running);
        running = Scheduler::get(); /* just a fake get - puts back a valid value */
    }
}
