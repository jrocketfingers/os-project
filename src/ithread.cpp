#include <iostream.h>
#include <ithread.h>

/* kernel.cpp */
extern unsigned int tick;
extern bool kernel_mode;
extern bool idling;

void idle() {
    while(1);
}

IThread::IThread() {
    pcb = new PCB(1); /* using the smallest tick unit */

    pcb->createStack(0, idle, 1024);
}

IThread::~IThread() {
    delete pcb;
}

void IThread::takeOver() {

    tick = pcb->timeSlice;

    kernel_mode = 0;
    idling = 1;

    _SP = pcb->sp;
    _SS = pcb->ss;

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
