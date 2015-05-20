#include <types.h>
#include <tdata.h>
#include <kernel.h>
#include <thread.h>
#include <syscalls.h>

Thread::Thread (StackSize stackSize, Time timeSlice) {
    ThreadData* td = new ThreadData();

    td->_this = this;
    td->_run = call;
    td->stackSize = stackSize;
    td->timeSlice = timeSlice;

    /* save used registers */
    asm {
        push ax
        push bx
        push cx
    }

    _AX = SYS_newthread;
    _BX = FP_SEG(td);
    _CX = FP_OFF(td);

    asm int 61h;

    tid = td->tid;

    delete td;

    /* restore used registers */
    asm {
        pop cx
        pop bx
        pop ax
    }
}


void Thread::start() {
    ThreadData *td = new ThreadData();

    /* make a syscall with the schedule call (103) */
    _AX = SYS_startthread;
    _BX = FP_SEG(td);
    _CX = FP_OFF(td);

    asm int 61h;

    delete td;
}

Thread::~Thread() {}


void Thread::waitToComplete() {

}


/* Static method */
void Thread::sleep(Time timeToSleep) {

}

void Thread::call(Thread* t) {
    t->run();

    ThreadData *td = new ThreadData();
    td->tid = t->tid;

    _AX = SYS_endthread;
    _BX = FP_SEG(td);
    _CX = FP_OFF(td);

    asm int 61h;

    delete td;
}