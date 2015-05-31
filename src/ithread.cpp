#include <debug.h>

#include <ithread.h>
#include <kernel.h>
#include <pcb.h>

/* kernel.cpp */
extern unsigned int tick;

void idle() {
    while(1);
}

IThread::IThread() {
    pcb = new PCB(1); /* using the smallest tick unit */
    pcb->id = -1;
    pcb->state = STATE_idle;

    pcb->createStack(0, idle, 1024);
}

IThread::~IThread() {
    delete pcb;
}

void IThread::takeOver() {
    asm cli;

    Kernel::idle();
    Kernel::running = this->pcb;

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
