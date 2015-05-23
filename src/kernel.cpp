#include <iostream.h>
#include <dos.h>

#include <syscalls.h>
#include <vector.h>
#include <kthread.h>
#include <thread.h>
#include <pcb.h>
#include <kernel.h>
#include <kernsem.h>

#include <schedule.h>

#define KERNEL_STACK_SIZE 1024

#define PUSHF(func) asm mov ax, seg func; \
    asm push ax; \
    asm mov ax, offset func; \
    asm push ax;

/* system threads */
KThread *kThread;
PCB *running = 0;

/* original timer interrupt handler */
unsigned int oldTimerSEG;
unsigned int oldTimerOFF;

/* ticker */
unsigned int tick;

/* list of available threads */
ffvector<PCB*>* PCBs = 0;
ffvector<KernSem*>* KernSems = 0;


void interrupt systick() {
    /* do not tick if the time slice is set to 0
     * unlimited runtime thread */
    if(running->timeSlice) tick--;

    asm int 60h; /* timer routine that we switched out */

    if(tick <= 0 && running->timeSlice) {
        running->sp = _SP;
        running->ss = _SS;

        /* syscall dispatch */
        _AX = SYS_dispatch;
        asm int 61h;
    }
}


void interrupt syscall(unsigned p_bp, unsigned p_di, unsigned p_si, unsigned p_ds,
                       unsigned p_es, unsigned p_dx, unsigned p_cx, unsigned p_bx,
                       unsigned p_ax, unsigned p_ip, unsigned p_cs, unsigned flags) {
    /* save stack */
    running->sp = _SP;
    running->ss = _SS;

    /* switch to kernel stack */
    kThread->takeOver(p_ax, p_bx, p_cx);
}


void Kernel::init() {
    /* prepare the initial thread information */
    PCB* userMain = new PCB(2); /* time slice = 2 */
    /* stackless thread; it uses the original stack*/

    kThread = new KThread();

    PCBs = new ffvector<PCB*>(10);
    userMain->id = PCBs->append(userMain);

    KernSems = new ffvector<KernSem*>(10);

    /* prepare IVT */
    asm {
        push es
        push ax
        mov ax, 0
        mov es, ax

        /* put syscall at the 61h entry */
        mov word ptr es:0184h, offset syscall
        mov word ptr es:0186h, seg syscall

        mov ax, word ptr es:0020h
        mov oldTimerOFF, ax
        mov ax, word ptr es:0022h
        mov oldTimerSEG, ax

        mov word ptr es:0020h, offset systick
        mov word ptr es:0022h, seg systick

        /* put the original system timer at 60h */
        mov ax, oldTimerOFF
        mov word ptr es:0180h, ax
        mov ax, oldTimerSEG
        mov word ptr es:0182h, ax

        pop ax
        pop es
    }

    /* mark the userMain as the running thread */
    running = userMain;
    tick = 2;

    cout << "Kernel initialization finished." << endl;
}


void Kernel::stop() {}
