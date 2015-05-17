#include <types.h>
#include <thread.h>
#include <tdata.h>
#include <schedule.h>
#include <kernel.h>
#include <util.h>

Thread::Thread (StackSize stackSize, Time timeSlice) {
    ThreadData* td = new ThreadData();

    td->_this = this;
    td->_run = call;
    td->stackSize = stackSize;
    td->timeSlice = timeSlice;

    _AX = 102;
    _BX = FP_SEG(td);
    _CX = FP_OFF(td);

    asm int 61h;
}


void Thread::start() {
    /* make a syscall with the schedule call (103) */
    _AX = 103;
    _BX = this->tid;

    asm int 61h;
}

Thread::~Thread() {}


void Thread::waitToComplete() {

}


/* Static method */
void Thread::sleep(Time timeToSleep) {

}