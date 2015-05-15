#include <kernel.h>
#include <pcb.h>
#include <thread.h>
#include <schedule.h>
#include <iostream.h>
#include <dos.h>
#include <vector.h>

#define KERNEL_STACK_SIZE 1024

#define PUSHF(func) asm mov ax, seg func; \
    asm push ax; \
    asm mov ax, offset func; \
    asm push ax;

/* system threads */
PCB *kThread;
PCB *running = 0;

/* original timer interrupt handler */
unsigned int oldTimerSEG;
unsigned int oldTimerOFF;

/* ticker */
unsigned int tick;

/* list of available threads */
ffvector<PCB*>* PCBs = 0;


void inline save_ctx() {
    if(running) {
        running->sp = _SP;
        running->ss = _SS;
        running->bp = _BP;

        running->ax = _AX;
        running->bx = _BX;
        running->cx = _CX;
        running->es = _ES;
    }
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

    running = pcb;
}


void sys_ctx_switch() {
    save_ctx();

    running = Scheduler::get();

    use_ctx(running);

    tick = running->timeSlice;
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
    word *tSeg = (word*)kThread->bx;
    word *tOff = (word*)kThread->cx;
    StackSize stack_size = kThread->dx;
    Time timeSlice = kThread->es;

    cout << "New thread starting. Syscall: " << kThread->ax << endl;

    /* reconstruct the thread pointer */
    Thread* t = (Thread*)MK_FP(tSeg, tOff);

    PCB* newPCB = new PCB(stack_size, timeSlice);

    /* create a stack for the thread t
     * thread needs to be listed because
     * of the this pointer */
    newPCB->createStack(t, stack_size);

    /* enlist the thread in the available PCBs vector
     * not all PCBs need to be enlisted - hence this isn't in the constructor.
     * kernel PCB remains unlisted and is switched to manually */
    newPCB->enlist(t);
}


void sys_schedule() {
    /* kThread->bx is actually the thread->tid */
    PCB *threadToSchedule = (*PCBs)[kThread->bx];
    Scheduler::put(threadToSchedule);
}


void sys_dispatch() {
    sys_ctx_switch();
}


void interrupt syscall(unsigned p_bp, unsigned p_di, unsigned p_si, unsigned p_ds,
                       unsigned p_es, unsigned p_dx, unsigned p_cx, unsigned p_bx,
                       unsigned p_ax, unsigned p_ip, unsigned p_cs) {

    cout << "System call." << endl;

    /* clean up interrupt parameters.
     * the return sequence will be executed by sys_ctx_switch,
     * which is a ---regular function call--- (cdecl). */
    /* NOTE: do not mess with the bp value, as it is used to access all the
     * interrupt parameters, they still DO exist on the running stack */
    asm {
        add sp, 18      // remove the registers stack
        pop dx          // take cs
        pop es          // take ip
        push p_bp       // push the original bp
        push es
        push dx
    }
    /* the running stack is now prepared for beign stored in the scheduler */

    /* save stack */
    save_ctx();

    kThread->ax = p_ax;
    kThread->bx = p_bx;
    kThread->cx = p_cx;
    kThread->dx = p_dx;
    kThread->es = p_es;

    /* switch to kernel stack */
    use_ctx(kThread);

    /* use the original ax value based on bp */
    /* stack has been changed p_ax unavailable */
    switch(kThread->ax) {
        case 101:
            cout << "Dispatching." << endl;
            PUSHF(sys_dispatch)
            break;
        case 102:
            cout << "Making a new thread." << endl;
            PUSHF(sys_newthread)
            break;
        default:
            cout << "default choice." << endl;
            break;
    }
}

void Kernel::init() {
    asm cli

    kThread = new PCB(65536, 0);
    PCBs = new ffvector<PCB*>(10);

    kThread->sp = FP_OFF(kThread->stack+63999);
    kThread->ss = FP_SEG(kThread->stack+63999);
    kThread->bp = FP_OFF(kThread->stack+63999);

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

    cout << "Kernel initialization finished." << endl;

    asm sti
}


void Kernel::stop() {}


void PCB::createStack(Thread* t, StackSize stackSize) {
    int sp_i;

    stack = new unsigned int[stackSize];

    sp = FP_OFF(stack+stackSize);
    ss = FP_SEG(stack+stackSize);

    /* Knit the thread back into the kernel
     * by placing the stop call on top of the thread*/
    this->stack[sp_i] = FP_SEG(Kernel::stop);
    this->stack[sp_i - 1] = FP_OFF(Kernel::stop);

    /* Since it's as if stop has called the call,
     * which it isn't, it was dispatched this way,
     * we need to free up two slots for the stop's bp and ds.
     *
     * DANGER: - future suspect code ahead -
     * Stop is going to use only static variables, so we can
     * take a risk with leaving that as garbage, for now.*/

    this->stack[sp_i - 4] = FP_SEG(t);
    this->stack[sp_i - 5] = FP_OFF(t);

    this->stack[sp_i - 6] = FP_SEG(call);
    this->stack[sp_i - 7] = FP_OFF(call);

    /* Make sure SP points to the right place */
    sp_i -= 7;
    this->sp = FP_OFF(this->stack + sp_i);
    this->ss = FP_SEG(this->stack + sp_i);
    this->bp = FP_OFF(this->stack + sp_i);
}


void PCB::enlist(Thread* t) {
    t->tid = PCBs->append(this);
}

void PCB::call(Thread* t) {
    t->run();
    Kernel::stop();
}
