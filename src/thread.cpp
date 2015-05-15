#include <types.h>
#include <thread.h>
#include <schedule.h>
#include <kernel.h>
#include <util.h>

Thread::Thread (StackSize stackSize, Time timeSlice) {
    _AX = 102;
    _BX = FP_SEG(this);
    _CX = FP_OFF(this);
    _DX = stackSize;
    _ES = timeSlice;

    asm int 61h;
}


void Thread::start() {
    /* make a syscall with the schedule call (103) */
    _AX = 103;
    _BX = this->tid;

    asm int 61h;
}


//void Thread::waitToComplete() {
    //Kernel::waitFor(this->tid);
//}


/* Static method */
//void Thread::sleep(Time timeToSleep) {
    //Kernel::sleep(timeToSleep); // hence it does not have a this parameter to assign
                            //// the sleep time to
//}