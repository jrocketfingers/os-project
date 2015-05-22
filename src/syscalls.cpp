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

extern PCB *running;
extern ffvector<PCB*>* PCBs;
extern unsigned int tick;

void newThread(ThreadData* data) {
    cout << "Creating a new thread..." << endl;

    PCB* newPCB = new PCB(data->timeSlice);

    newPCB->createStack(data->_this, data->_run, data->stackSize);

    /* enlist the thread in the available PCBs vector
     * not all PCBs need to be enlisted - hence this isn't in the constructor.
     * kernel PCB remains unlisted and is switched to manually */
    data->tid = PCBs->append(newPCB);
    newPCB->tid = data->tid;

    cout << "New thread created. TID: " << data->tid << endl;
}


void dispatch() {
    if(!running->done)
        Scheduler::put(running);

    do {
        running = Scheduler::get();
    } while(running->done); /* if the newly fetched thread is marked done
                              (terminated), pop it, and find another */

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


void startThread(ThreadData *data) {
    Scheduler::put((*PCBs)[data->tid]);

    cout << "Thread scheduled." << endl;
}


void endThread(ThreadData *data) {
    (*PCBs)[data->tid]->done = 1;
}


void wait(SemaphoreData *data) {

}


void signal(SemaphoreData *data) {

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
        case SYS_signal:
            signal((SemaphoreData*)data);
            break;
        case SYS_wait:
            wait((SemaphoreData*)data);
            break;
        default:
            cout << "Inconsistent syscall! " << callID << endl;
            break;
    }

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
        iret
    }
}
