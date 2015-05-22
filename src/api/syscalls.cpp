/* declaration header */
#include <api/syscalls.h>

/* dos dependencies */
#include <dos.h>

/* system interface */
//#include <context.h>        [> context locking primitives <]
#include <tdata.h>
#include <sdata.h>
#include <syscalls.h>

int sys_newthread(void *_this, void *_run, StackSize stackSize, Time timeSlice) {
    /* save used registers */
    asm {
        push ax
        push bx
        push cx
    }

    // lock here somehow
    ThreadData* td = new ThreadData();

    td->_run = _run;
    td->_this = _this;
    td->stackSize = stackSize;
    td->timeSlice = timeSlice;

    _AX = SYS_newthread;
    _BX = FP_SEG(td);
    _CX = FP_OFF(td);

    asm int 61h;

    unsigned tid = td->tid;

    delete td;

    /* restore used registers */
    asm {
        pop cx
        pop bx
        pop ax
    }

    // unlock somehow

    return tid;
}

void sys_startthread(unsigned tid) {
    // lock somehow

    ThreadData *td = new ThreadData();

    td->tid = tid;

    /* make a syscall with the schedule call (103) */
    _AX = SYS_startthread;
    _BX = FP_SEG(td);
    _CX = FP_OFF(td);

    asm int 61h;

    delete td;

    // unlock somehow
}

void sys_endthread(unsigned tid) {
    ThreadData *td = new ThreadData();
    td->tid = tid;

    _AX = SYS_endthread;
    _BX = FP_SEG(td);
    _CX = FP_OFF(td);

    asm int 61h;

    delete td;
}
