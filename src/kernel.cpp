#include <kernel.h>
#include <pcb.h>
#include <thread.h>
#include <kthread.h>
#include <schedule.h>
#include <iostream.h>
#include <dos.h>
#include <vector.h>
#include <dos.h>

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

/* temp registers */
word tax, tbx, tcx, tdx, tes;

/* flags */
bool flag_change_thread_on_ctx_switch = 0;

/* list of available threads */
ffvector<PCB*>* PCBs = 0;


void interrupt systick() {
    /* do not tick if the time slice is set to 0
     * unlimited runtime thread */
    if(running->timeSlice) tick--;

    if(tick <= 0 && running->timeSlice) { /* do a context switch */ }

    asm int 60h;
}


void sys_newthread() {
}


void interrupt syscall(unsigned p_bp, unsigned p_di, unsigned p_si, unsigned p_ds,
                       unsigned p_es, unsigned p_dx, unsigned p_cx, unsigned p_bx,
                       unsigned p_ax, unsigned p_ip, unsigned p_cs, unsigned flags) {
    /* save stack */
    running->sp = _SP;
    running->ss = _SS;

    /* switch to kernel stack */
    kThread->takeOver(p_ax, p_bx, p_cx);

    cout << "Switched to a kernel thread." << endl;
}

void Kernel::init() {
    /* prepare the initial thread information */
    PCB* userMain = new PCB(2);
    /* stackless thread; it uses the original stack*/

    kThread = new KThread();

    PCBs = new ffvector<PCB*>(10);
    PCBs->append(userMain);


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

    cout << "Kernel initialization finished." << endl;
}


void Kernel::stop() {}


void PCB::createStack(Thread* t, StackSize stackSize) {
    unsigned *sPtr;

    stack = new unsigned int[stackSize];
    sPtr = stack + stackSize;

    sp = FP_OFF(stack+stackSize);
    ss = FP_SEG(stack+stackSize);

    *(--sPtr) = FP_SEG(t); // this stands as a parameter to the PCB::call
    *(--sPtr) = FP_OFF(t);

    sPtr -= 3; // skip first call's cs, ip and bp

    *(--sPtr) = FP_SEG(call); // add the return path from the interrupt
    *(--sPtr) = FP_OFF(call);

    /* Make sure SP points to the right place */
    sPtr -= 8;

    *(--sPtr) = FP_OFF(sPtr - 11);

    this->sp = FP_OFF(sPtr);
    this->ss = FP_SEG(sPtr);
    this->bp = FP_OFF(sPtr);
}


void PCB::enlist(Thread* t) {
    t->tid = PCBs->append(this);
}


void PCB::call(Thread* t) {
    t->run();
    Kernel::stop();
}
