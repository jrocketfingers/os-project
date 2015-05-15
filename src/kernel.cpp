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

/* temp registers */
word tax, tbx, tcx, tdx, tes;

/* flags */
bool change_thread_on_ctx_switch = 0;

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

    if(change_thread_on_ctx_switch) {
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
    change_thread_on_ctx_switch = 0;

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

    asm mov bp, sp;
    asm mov ax, [bp];
    cout << "PCB enlisted 'n' stuff. Ready to switch! Should be 512: " << _AX << endl;
}


void sys_schedule() {
    /* kThread->bx is actually the thread->tid */
    PCB *threadToSchedule = (*PCBs)[kThread->bx];
    Scheduler::put(threadToSchedule);
}


void sys_dispatch() {
    sys_ctx_switch();
}


void interrupt switch_to_syscall(unsigned p_bp, unsigned p_di, unsigned p_si, unsigned p_ds,
                       unsigned p_es, unsigned p_dx, unsigned p_cx, unsigned p_bx,
                       unsigned p_ax, unsigned p_ip, unsigned p_cs, unsigned flags) {

    cout << "System call. AX: " << p_ax << endl;

    /* transfer the parameters to the kthread */
    kThread->ax = p_ax;
    kThread->bx = p_bx;
    kThread->cx = p_cx;
    kThread->dx = p_dx;
    kThread->es = p_es;

    /* clean up interrupt parameters.
     * the return sequence will be executed by sys_ctx_switch,
     * which is a ---regular function call--- (cdecl). */
    /* NOTE: do not mess with the bp value, as it is used to access all the
     * interrupt parameters, they still DO exist on the running stack */
    tax = _AX;
    tdx = _DX;
    tes = _ES;
    asm {
        /* TO DO: <<<<<<<<<<<<<<<<<<<<<<<<<<
         * Move up CS and IP because there's the flags register just above them in
         * an interrupt call. A normal return will clean from the register
         * only the CS and IP, ignoring the flags, and ruining the stack! */
        mov sp, bp      // remove the registers stack (there's an extra sp -= 10)
        add sp, 18
        pop dx          // take ip
        pop es          // take cs
        add sp, 1       // skip the flags

        push es
        push dx
        push p_bp       // push the original bp
    }
    _AX = tax;
    _DX = tdx;
    _ES = tes;

    /* save stack */
    save_ctx();

    /* switch to kernel stack */
    use_ctx(kThread);

    /* add the context switch on top of the stack */

    /* use the original ax value based on bp */
    /* stack has been changed p_ax unavailable */
    switch(kThread->ax) {
        case 101:
            cout << "Dispatching." << endl;
            asm mov ax, 0200h;
            asm push ax;
            PUSHF(sys_dispatch)
            break;
        case 102:
            cout << "Making a new thread." << endl;
            PUSHF(sys_ctx_resume);

            /* setting up the flags */
            asm mov ax, 0200h;
            asm push ax;
            /* pushing the handler */
            PUSHF(sys_newthread)
            break;
        default:
            cout << "default choice." << endl;
            break;
    }

    asm {
        sub sp, 18
        mov bp, sp      // prevent from old bp overtaking sp value

        /* this segment is needed cause at the end of a call there's
         * mov sp, bp; this effectively erases local variables off the stack
         * unless this is configured properly, the stack blows */
        mov ax, bp
        add ax, 22      // calculate the old bp location
        mov [bp], ax    // set the old bp
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


void Kernel::syscall() {
    while(1) {


        sys_ctx_switch();
    }
}


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
