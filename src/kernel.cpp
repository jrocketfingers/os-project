#include <kernel.h>
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
PCB *running;

/* original timer interrupt handler */
void (*old_timer_handler)();
unsigned int oldTimerSEG;
unsigned int oldTimerOFF;

/* list of available threads */
ffvector<PCB*>* Kernel::PCBs = 0;

void inline save_ctx() {
    running->sp = _SP;
    running->ss = _SS;
}

void inline use_ctx(PCB* pcb) {
    _SP = pcb->sp;
    _SS = pcb->ss;

    running = pcb;
}

void interrupt systick() {
    asm int 60h;
}


void sys_yield() {
    cout << "Yielding!" << endl;
}


void sys_newthread(p_bp, p_di, p_si, p_ds, p_es, p_dx, p_cx, p_bx, p_ax, p_ip, p_cs) {
    cout << "New thread." << p_ax << endl;
}


void Kernel::yield() {
}

void Kernel::dispatch() {
    //Kernel::ctx_lock = 1;
}

tid_t Kernel::enlistPCB(PCB* newPCB) {
    return PCBs->append(newPCB);
}

void Kernel::schedule(unsigned int tid) {
    Scheduler::put((*PCBs)[tid]);

    /* set up for a dispatch system call */
    _AX = 0;

    asm int 60h;
}

void interrupt syscall(unsigned p_bp, unsigned p_di, unsigned p_si, unsigned p_ds,
                       unsigned p_es, unsigned p_dx, unsigned p_cx, unsigned p_bx,
                       unsigned p_ax, unsigned p_ip, unsigned p_cs) {
    cout << "System call. AX:" << p_ax << endl;

    /* save stack */
    save_ctx();

    /* use kernel stack */
    use_ctx(kThread);

    /* DELETE THIS */
    /* put the return from kernel thread to the caller, temp solution */
    asm {
        push p_cs
        push p_ip
        push p_bp
    }
    /* UP TO HERE */

    switch(p_ax) {
        case 101:
            cout << "setting up sys_yield." << endl;
            PUSHF(sys_yield)
            //asm {
                //mov ax, seg sys_yield
                //push ax
                //mov ax, offset sys_yield
                //push ax
            //}

            break;
        case 102:
            PUSHF(sys_newthread)
            break;
        default:
            cout << "default choice." << endl;
            PUSHF(sys_newthread)
            break;
    }

    asm {
        push p_ax
        push p_bx
        push p_cx
        push p_dx
        push p_es
        push p_ds
        push p_si
        push p_di
        push p_bp

        mov bp, sp
    }
}

void Kernel::init() {
    asm cli

    kThread = new PCB(64000, "Kernel");
    PCBs = new ffvector<PCB*>(10);

    kThread->sp = FP_OFF(kThread->stack+63999);
    kThread->ss = FP_SEG(kThread->stack+63999);
    kThread->bp = FP_OFF(kThread->stack+63999);

    asm {
        push es
        push ax
        mov ax, 0
        mov es, ax

        mov word ptr es:0184h, offset syscall
        mov word ptr es:0186h, seg syscall

        mov ax, word ptr es:0020h
        mov oldTimerOFF, ax
        mov ax, word ptr es:0022h
        mov oldTimerSEG, ax

        mov word ptr es:0020h, offset systick
        mov word ptr es:0022h, seg systick

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
