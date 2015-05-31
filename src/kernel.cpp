#include <dos.h>

#include <api/syscalls.h>
#include <syscalls.h>
#include <vector.h>
#include <thread.h>
#include <pcb.h>
#include <kernel.h>
#include <kernsem.h>
#include <kernev.h>
#include <schedule.h>
#include <sleepq.h>
#include <kthread.h>
#include <ithread.h>

#include <debug.h>

#define KERNEL_STACK_SIZE 1024

#define PUSHF(func) asm mov ax, seg func; \
    asm push ax; \
    asm mov ax, offset func; \
    asm push ax;

/* system threads */
KThread Kernel::kThread;
IThread Kernel::iThread;
PCB*    Kernel::running = 0;

KernelState Kernel::state;

int Kernel::active_threads      = 0;
int Kernel::blocked_threads     = 0;
int Kernel::sleeping_threads    = 0;

/* original timer interrupt handler */
unsigned int oldTimerSEG;
unsigned int oldTimerOFF;

/* ticker */
unsigned int tick;

/* sleeping queue */
SleepQ Kernel::sleeping;

/* list of available threads */
ffvector<PCB*>*     Kernel::PCBs        = 0;
ffvector<KernSem*>* Kernel::KernSems    = 0;
ffvector<KernEv*>*  Kernel::KernEvs     = 0;

void interrupt systick() {
    asm int 60h; /* timer routine that we switched out */

    /* do not tick if the time slice is set to 0
     * unlimited runtime thread */
    if(Kernel::running->timeSlice != 0) {
        tick++;
    }

    Kernel::sleeping.tick();

    /* if we're safe to preempt */
    if(Kernel::state == STATE_working && tick >= Kernel::running->timeSlice) {
        /* syscall */
        dispatch();
        tick = 0;
    }
}


void interrupt syscall(unsigned p_bp, unsigned p_di, unsigned p_si, unsigned p_ds,
                       unsigned p_es, unsigned p_dx, unsigned p_cx, unsigned p_bx,
                       unsigned p_ax, unsigned p_ip, unsigned p_cs, unsigned flags) {
    asm cli;

    /* save stack */
    if(Kernel::state == STATE_working) {
        Kernel::running->sp = _SP;
        Kernel::running->ss = _SS;
    }
    else if(Kernel::state == STATE_idle) {
        Kernel::iThread.pcb->sp = _SP;
        Kernel::iThread.pcb->ss = _SS;
    }
    #ifdef DEBUG
    else {
        cout << "Entering kernel mode from an illegal state." << endl;
        exit(1);
    }
    #endif

    Kernel::state = STATE_kmode;

    /* switch to kernel stack */
    Kernel::kThread.takeOver(p_ax, p_bx, p_cx);
}


void Kernel::init() {
    asm cli;

    /* prepare the initial thread information */
    PCB* userMain = new PCB(2); /* time slice = 2 */
    /* stackless thread; it uses the original stack*/

    /* make an available PCB listing, and add userMain */
    PCBs            = new ffvector<PCB*>(100);
    userMain->id    = PCBs->append(userMain);

    #ifdef DEBUG
    cout << "User main has ID: " << userMain->id << endl;
    #endif

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

    /* mark the userMain as the Kernel::running thread */
    Kernel::running         = userMain;
    Kernel::running->state  = STATE_running; /* manually sets usermain as the running thread */
    Kernel::state   = STATE_working;
    tick            = 0;

    #ifdef DEBUG
    cout << "Kernel initialization finished." << endl;
    #endif

    asm sti;
}


void Kernel::stop() {}
