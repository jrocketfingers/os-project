#include <kernev.h>
#include <schedule.h>

#include <debug.h>

extern void _dispatch();

KernEv::KernEv() {
    /* mark who created the event */
    this->creator = Kernel::running;

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
        Scheduler::put(creator);
        creator->unblock();
        _dispatch();
    }
}

void KernEv::wait() {
    val--;

    /* blocking case */
    if(val < 0) {
        creator->block();
        _dispatch();
    }

#ifdef DEBUG__EVENTS
    /* diagnostics */
    if(val < -1)
        cout << "Event has waited more than once, error!" << endl << flush;
#endif
}
