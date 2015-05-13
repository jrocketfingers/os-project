#include <types.h>
#include <thread.h>
#include <schedule.h>
#include <os.h>
#include <util.h>

Thread::Thread (StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice) {
    lock;
    PCB *newPCB = new PCB(stackSize, timeSlice);
    unlock;

    int sp_i = 1023;

    /* Knit the thread back into the kernel
     * by placing the stop call on top of the thread*/
    newPCB->stack[sp_i] = FP_SEG(OS::stop());
    newPCB->stack[sp_i - 1] = FP_OFF(OS::stop());

    /* Since it's as if stop has called the call,
     * which it isn't, it was dispatched this way,
     * we need to free up two slots for the stop's bp and ds.
     *
     * DANGER: - future suspect code ahead -
     * Stop is going to use only static variables, so we can
     * take a risk with leaving that as garbage, for now.*/

    newPCB->stack[sp_i - 4] = FP_SEG(this);
    newPCB->stack[sp_i - 5] = FP_OFF(this);

    newPCB->stack[sp_i - 6] = FP_SEG(Thread::call);
    newPCB->stack[sp_i - 7] = FP_OFF(Thread::call);

    /* Make sure SP points to the right place */
    sp_i -= 7;
    newPCB->sp = FP_OFF(newPCB->stack + sp_i);
    newPCB->ss = FP_SEG(newPCB->stack + sp_i);
    newPCB->bp = FP_OFF(newPCB->stack + sp_i);

    this->tid = OS::enlistPCB(newPCB);
}

void Thread::start() {
    OS::schedule(this->tid);
}

void Thread::waitToComplete() {
    OS::waitFor(this->tid);
}

/* Static method */
void Thread::sleep(Time timeToSleep) {
    OS::sleep(timeToSleep); // hence it does not have a this parameter to assign
                            // the sleep time to
}

/* Non-polymorphic run wrapper. */
void Thread::call() {
    this->run();
    OS::stop();
}