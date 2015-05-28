/* system libraries */
#include <iostream.h>

#include <kernev.h>
#include <schedule.h>

/* kernel.cpp */
extern PCB* running;
extern char dont_schedule;

KernEv::KernEv() {
    /* mark who created the event */
    this->creator = running;

    /* value is initially set to 0, since it is suposed to block at first */
    val = 0;
}

KernEv::~KernEv() {
    if(val < 0)
        Scheduler::put(this->creator);
}

void KernEv::signal() {
    val++;

    /* unblocking case */
    if(val >= 0) {
        Scheduler::put(this->creator);
    }
}

void KernEv::wait() {
    val--;

    /* blocking case */
    if(val < 0)
        dont_schedule = 1;

    /* diagnostics */
    if(val < -1)
        cout << "Event has waited more than once, error!" << endl;
}
