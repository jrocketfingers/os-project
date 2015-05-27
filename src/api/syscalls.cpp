/* declaration header */
#include <api/syscalls.h>

/* dos dependencies */
#include <dos.h>
#include <iostream.h> /* debug dependency */

/* system interface */
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

    /* why stack allocated data?
     * it's on the callers stack, so it does make sense,
     * also, allocation is atomic, so there is only one lock
     * necessary, inside the system call itself */
    ThreadData td;
    ThreadData *ptd = &td;
    /* this above is just an ugly hack that prevents _BX = FP_SEG(&td) from
     * overwriting _AX in the process */

    td._run = _run;
    td._this = _this;
    td.stackSize = stackSize;
    td.timeSlice = timeSlice;

    _AX = SYS_newthread;
    _BX = FP_SEG(ptd);
    _CX = FP_OFF(ptd);

    asm int 61h;

    unsigned tid = td.tid;

    /* restore used registers */
    asm {
        pop cx
        pop bx
        pop ax
    }

    return tid;
}

void sys_startthread(unsigned tid) {
    ThreadData td;
    ThreadData *ptd = &td;

    td.tid = tid;

    /* make a syscall with the schedule call (103) */
    _AX = SYS_startthread;
    _BX = FP_SEG(ptd);
    _CX = FP_OFF(ptd);

    asm int 61h;
}

void sys_endthread(unsigned tid) {
    ThreadData td;
    ThreadData *ptd = &td;

    td.tid = tid;

    _AX = SYS_endthread;
    _BX = FP_SEG(ptd);
    _CX = FP_OFF(ptd);

    asm int 61h;
}

void sys_waittocomplete(unsigned tid) {
    ThreadData td;
    ThreadData *ptd = &td;

    td.tid = tid;

    _AX = SYS_waittocomplete;
    _BX = FP_SEG(ptd);
    _CX = FP_OFF(ptd);

    asm int 61h;
}

void sys_sleep(Time time) {
    ThreadData td;
    ThreadData *ptd = &td;

    td.timeSlice = time;

    _AX = SYS_sleep;
    _BX = FP_SEG(ptd);
    _CX = FP_OFF(ptd);

    asm int 61h;
}

unsigned sys_newsem(int init) {
    int* data = &init;

    _AX = SYS_newsem;
    _BX = FP_SEG(data);  // messy; init is both the initial value
    _CX = FP_OFF(data);  // and the returned SID

    asm int 61h;

    return init;        /* SID of the new semaphore */
}

void sys_deletesem(unsigned sid) {
    unsigned* data = &sid;

    _AX = SYS_deletesem;
    _BX = FP_SEG(data);
    _CX = FP_OFF(data);

    asm int 61h;
}

void sys_sigsem(unsigned sid) {
    unsigned* data = &sid;

    _AX = SYS_sigsem;
    _BX = FP_SEG(data);
    _CX = FP_OFF(data);

    asm int 61h;
}

void sys_waitsem(unsigned sid) {
    unsigned* data = &sid;

    _AX = SYS_waitsem;
    _BX = FP_SEG(data);
    _CX = FP_OFF(data);

    asm int 61h;
}

int sys_semval(unsigned sid) {
    unsigned* data = &sid;
    _AX = SYS_semval;
    _BX = FP_SEG(data);
    _CX = FP_OFF(data);

    asm int 61h;

    return sid;         // SID now contains semaphore value
}

unsigned sys_newev(unsigned ivtNo) {
    unsigned* data = &ivtNo;

    _AX = SYS_newevent;
    _BX = FP_SEG(data);
    _CX = FP_OFF(data);

    asm int 61h;

    return ivtNo;        /* EID of the new event */
}

void sys_deleteev(unsigned eid) {
    unsigned* data = &eid;

    _AX = SYS_deleteevent;
    _BX = FP_SEG(data);
    _CX = FP_OFF(data);

    asm int 61h;
}

void sys_sigev(unsigned eid) {
    unsigned* data = &eid;

    _AX = SYS_sigev;
    _BX = FP_SEG(data);
    _CX = FP_OFF(data);

    asm int 61h;
}

void sys_waitev(unsigned eid) {
    unsigned* data = &eid;

    _AX = SYS_waitev;
    _BX = FP_SEG(data);
    _CX = FP_OFF(data);

    asm int 61h;
}
