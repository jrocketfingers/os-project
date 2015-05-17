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


void inline save_ctx(PCB* pcb) {
    pcb->sp = _SP;
    pcb->ss = _SS;
    pcb->bp = _BP;

    pcb->ax = _AX;
    pcb->bx = _BX;
    pcb->cx = _CX;
    pcb->dx = _DX;
    pcb->es = _ES;
}


void inline use_ctx(PCB* pcb) {
    _SP = pcb->sp;
    _SS = pcb->ss;
    _BP = pcb->bp;

    _AX = pcb->ax;
    _BX = pcb->bx;
    _CX = pcb->cx;
    _DX = pcb->dx;
    _ES = pcb->es;

    //running = pcb;
    /* We're not gonna set running. Kernel thread can take over, without
     * marking itself as the running thread. Why? We're pretty sure we're inside
     * the kernel when we are even without checking running == kThread;
     * What does this allow us? We switch back to the original thread by
     * writting use_ctx(running). Oh and since it is impossible to save the
     * kernel stack pointer and stuff, it'll ALWAYS start by using an empty
     * stack. Awesome, a commented line does all this. */
    /* Also, now running holds a reference to the PCB of the thread that
     * initiated the syscall 'n' stuff. Super-useful. */

    /* Why was all this written:
     * DO NOT FORGET TO WRITE running = Scheduler::get(); in context switches! */
}


/* not-really-an-interrupt */
void interrupt sys_ctx_switch() {
    /* basically this should be started only from the kernel thread,
     * so there's no need to save the stack (kernel always starts with an empty)
     * stack. */

    if(flag_change_thread_on_ctx_switch) {
        cout << "Switching the context!" << endl;
        Scheduler::put(running);
        /* run the CPU time lottery
         * fairest lottery ever -
         * even the house (OS) doesn't know how's it selected */
        running = Scheduler::get();
        /* Oh wait, we've already written this. We're safe. (you can almost ignore
         * the warning written in use_ctx */
    }

    tick = running->timeSlice;
    flag_change_thread_on_ctx_switch = 1;

    /* running ctx might not have been changed depending on the switch flag */
    use_ctx(running);

    cout << "Resuming the thread..." << endl;
}


void interrupt systick() {
    /* do not tick if the time slice is set to 0
     * unlimited runtime thread */
    if(running->timeSlice) tick--;

    if(tick <= 0 && running->timeSlice) {
        sys_ctx_switch();
    }

    asm int 60h;
}


void sys_newthread() {
    word *tSeg = (word*)kThread->pcb->bx;
    word *tOff = (word*)kThread->pcb->cx;
    StackSize stack_size = kThread->pcb->dx;
    Time timeSlice = kThread->pcb->es;

    cout << "New thread starting. Syscall: " << kThread->ax << endl;

    /* reconstruct the thread pointer */
    Thread* t = (Thread*)MK_FP(tSeg, tOff);

    PCB* newPCB = new PCB(timeSlice);

    /* create a stack for the thread t
     * thread needs to be listed because
     * of the this pointer */
    newPCB->createStack(t, stack_size);

    /* enlist the thread in the available PCBs vector
     * not all PCBs need to be enlisted - hence this isn't in the constructor.
     * kernel PCB remains unlisted and is switched to manually */
    newPCB->enlist(t);

    asm mov bp, sp;
    asm mov ax, [bp];
    cout << "PCB enlisted 'n' stuff. Ready to switch! Should be 512: " << _AX << endl;
}


void sys_schedule() {
    /* kThread->bx is actually the thread->tid */
    PCB *threadToSchedule = (*PCBs)[kThread->bx];
    Scheduler::put(threadToSchedule);
}


void interrupt syscall(unsigned p_bp, unsigned p_di, unsigned p_si, unsigned p_ds,
                       unsigned p_es, unsigned p_dx, unsigned p_cx, unsigned p_bx,
                       unsigned p_ax, unsigned p_ip, unsigned p_cs, unsigned flags) {

    cout << "System call. AX: " << p_ax << endl;

    /* transfer the parameters to the kthread */
    kThread->ax = p_ax;
    kThread->bx = p_bx;
    kThread->cx = p_cx;
    kThread->dx = p_dx;
    kThread->es = p_es;

    /* save stack */
    save_ctx(running);

    /* switch to kernel stack */
    use_ctx(kThread);
    cout << "Switched to kernel thread. far ptr: " << _SS << ":" << _SP << ":" << _BP << endl;
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
        mov word ptr es:0184h, offset switch_to_syscall
        mov word ptr es:0186h, seg switch_to_syscall

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


KThread::KThread() {
    /* prepare the kernel thread */
    pcb = new PCB(0);
    pcb->stack = new unsigned[65536];
    unsigned *kPtr = kThread->stack + 65536;
    *(--kPtr) = 0x200;
    *(--kPtr) = FP_SEG(Kernel::syscall);
    *(--kPtr) = FP_OFF(Kernel::syscall);

    kPtr -= 9;

    //save_ctx(kThread);
    pcb->sp = FP_OFF(kPtr);
    pcb->ss = FP_SEG(kThread->stack);
    pcb->bp = FP_OFF(kPtr);
}


void Kernel::stop() {}


void Kernel::syscall() {
    while(1) {
        switch(kThread->ax) {
            case 101:
                cout << "Dispatching." << endl;
            case 102:
                cout << "Making a new thread." << endl;
                sys_newthread();
                flag_change_thread_on_ctx_switch = 0;
                break;
            default:
                cout << "Wrong system call!" << endl;
                flag_change_thread_on_ctx_switch = 0;
                break;
        }

        sys_ctx_switch();
    }
}


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
