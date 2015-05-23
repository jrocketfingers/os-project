/* default includes */
#include <iostream.h>
#include <dos.h>

/* dependincies */
#include <pcb.h>            /* handles PCBs */
#include <ffvector.h>       /* handles PCB storing */
#include <tdata.h>          /* thread transient data */
#include <sdata.h>          /* semaphore transient data */
#include <syscalls.h>       /* enumerated list of syscalls */
#include <scheduler.h>      /* scheduler implementation */
#include <kernsem.h>        /* handles kernel */

extern PCB *running;
extern ffvector<PCB*>* PCBs;
extern ffvector<KernSem*>* KernSems;
extern unsigned int tick;

void newThread(ThreadData* data) {
    PCB* newPCB = new PCB(data->timeSlice);

    newPCB->createStack(data->_this, data->_run, data->stackSize);

    /* enlist the thread in the available PCBs vector
     * not all PCBs need to be enlisted - hence this isn't in the constructor.
     * kernel PCB remains unlisted and is switched to manually */
    data->tid = PCBs->append(newPCB);
    newPCB->id = data->tid;
}


void dispatch() {
    if(!running->done)
        Scheduler::put(running);

    do {
        running = Scheduler::get();
    } while(running->done); /* if the newly fetched thread is marked done
                              (terminated), pop it, and find another */

    cout << "Switch " << running->id << endl;
}


void startThread(ThreadData *data) {
    Scheduler::put((*PCBs)[data->tid]);
}


void endThread(ThreadData *data) {
    (*PCBs)[data->tid]->done = 1;
}


void waitToComplete(ThreadData *data) {
    PCB* waitOnThread = (*PCBs)[data->tid];
    waitOnThread->waitingOn.put(running);
}


void newSemaphore(int *init) {
    KernSem *sem = new KernSem(*init);
    *init = KernSems->append(sem);   // init returns the SID; ugly, but faster
}


void deleteSemaphore(int *sid) {
    KernSem *sem = (*KernSems)[*sid];
    PCB *pcb;

    while(pcb = sem->blocked.get())
        Scheduler::put(pcb);

    KernSems->remove(*sid);

    delete sem;
}


void wait(int *sid) {
    (*KernSems)[*sid]->wait();
}


void signal(int *sid) {
    (*KernSems)[*sid]->signal();
}


void semval(int *sid) {
    *sid = (*KernSems)[*sid]->value;
}


void dispatchSyscall(unsigned callID, void *data) {
    switch(callID) {
        case SYS_dispatch:
            dispatch();
            break;
        case SYS_newthread:
            newThread((ThreadData*)data);
            break;
        case SYS_startthread:
            startThread((ThreadData*)data);
            break;
        case SYS_endthread:
            endThread((ThreadData*)data);
            break;
        case SYS_waittocomplete:
            waitToComplete((ThreadData*)data);

        case SYS_newsem:
            newSemaphore((int*)data);
            break;
        case SYS_deletesem:
            deleteSemaphore((int*)data);
            break;
        case SYS_signal:
            signal((int*)data);
            break;
        case SYS_wait:
            wait((int*)data);
            break;
        case SYS_semval:
            semval((int*)data);
            break;
        default:
            cout << "Inconsistent syscall! " << callID << endl;
            break;
    }

    dispatch(); /* change the active running */

    tick = running->timeSlice;

    _SP = running->sp;
    _SS = running->ss;


    asm {
        pop bp
        pop di
        pop si
        pop ds
        pop es
        pop dx
        pop cx
        pop bx
        pop ax

        iret /* exit point */
    }
}
