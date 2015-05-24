#include <iostream.h>
#include <dos.h>

#include <kernel.h>
#include <syscalls.h>
#include <kthread.h>

#include <scheduler.h> /* dispatch needs it */

/* syscalls.cpp */
void dispatchSyscall(unsigned callID, void *data);

/* kernel.cpp */
extern bool kernel_mode;


KThread::KThread() {
    /* prepare the kernel thread */
    stackSize = 4096;
    pcb = new PCB(0);
    pcb->stack = new unsigned[stackSize];

    pcb->sp = FP_OFF(pcb->stack + stackSize);
    pcb->ss = FP_SEG(pcb->stack);
}


void KThread::takeOver(unsigned callID, unsigned data_seg, unsigned data_off) {
    kernel_mode = 1;

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