#include <iostream.h>
#include <dos.h>

#include <kernel.h>
#include <types.h>
#include <syscalls.h>
#include <kthread.h>

#include <scheduler.h> /* dispatch */

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

    cout << "New thread created." << endl;
}

void dispatch() {
    Scheduler::put(running);
    running = Scheduler::get();

    tick = running->timeSlice;

    cout << "Tick set at: " << tick << endl;

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


KThread::KThread() {
    /* prepare the kernel thread */
    stackSize = 4096;
    pcb = new PCB(0);
    pcb->stack = new unsigned[stackSize];

    pcb->sp = FP_OFF(pcb->stack + stackSize);
    pcb->ss = FP_SEG(pcb->stack);
}


void KThread::takeOver(unsigned callID, unsigned data_seg, unsigned data_off) {
    unsigned *top = pcb->stack + this->stackSize;

    *(--top) = data_seg;
    *(--top) = data_off;
    *(--top) = callID;
    top -= 2;
    *(--top) = 0x200;
    *(--top) = FP_SEG(dispatchSyscall);
    *(--top) = FP_OFF(dispatchSyscall);


    _SP = FP_OFF(top);
    _SS = FP_SEG(top);

    asm iret; /* exit point */
}