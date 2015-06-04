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
    if(val < 0) {
        asm cli;
        Scheduler::put(this->creator);
        asm sti;
    }
}

void KernEv::signal() {
    if(val++ < 0) {
        #ifdef DEBUG__EVENTS
        cout << "Event " << ivtno << " firing." << endl;
        #endif
        asm cli;
        Scheduler::put(creator);
        asm sti;
        creator->unblock();
    }

    //if(val > 1) val = 1;
}

void KernEv::wait() {
    if(Kernel::running != creator) return;

    /* blocking case */
    if(--val < 0) {
        #ifdef DEBUG__EVENTS
        cout << "Event " << ivtno << " waiting." << endl;
        #endif
        creator->block();
        _dispatch();
    }

    #ifdef DEBUG__EVENTS
    /* diagnostics */
    if(val < -1)
        cout << "Event has waited more than once, error!" << endl << flush;
    #endif
}
