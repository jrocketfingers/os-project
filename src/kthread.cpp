#include <iostream.h>
#include <kernel.h>
#include <kthread.h>
#include <dos.h>
#include <types.h>

extern PCB *running;
extern ffvector<PCB*>* PCBs;

void newThread(ThreadData* data) {
    cout << "Creating a new thread..." << endl;

    PCB* newPCB = new PCB(data->timeSlice);

    /* create a stack for the thread t
     * thread needs to be listed because
     * of the this pointer */
    newPCB->createStack(data->_this, data->_run, data->stackSize);

    /* enlist the thread in the available PCBs vector
     * not all PCBs need to be enlisted - hence this isn't in the constructor.
     * kernel PCB remains unlisted and is switched to manually */
    data->tid = PCBs->append(newPCB);

    cout << "New thread created." << endl;
}

void dispatchSyscall(unsigned callID, void *data) {
    cout << "CallID: " << callID << endl;
    switch(callID) {
        case 102:
            newThread((ThreadData*)data);
            break;
        default:
            cout << "Inconsistent syscall!";
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
    pcb->bp = FP_OFF(pcb->stack + stackSize);
}

void KThread::takeOver(unsigned callID, unsigned data_seg, unsigned data_off) {
    cout << "CallID: " << callID << endl;

    void *top = pcb->stack + this->stackSize;
    _SP = FP_OFF(top);
    _SS = FP_SEG(top);

    asm {
        mov ax, data_seg
        push ax

        mov ax, data_off
        push ax


        mov ax, callID
        push ax

        sub sp, 4   // make empty cs and ip for (this is the previous fn on stack)
    }

    /* I do not know how to write the scope resolution operator in assembly */
    _AX = FP_SEG(dispatchSyscall);
    asm push ax;

    _AX = FP_OFF(dispatchSyscall);
    asm push ax;

    asm ret /* exit point */
}