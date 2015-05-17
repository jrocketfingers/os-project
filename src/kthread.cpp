#include <kthread.h>
#include <dos.h>

KThread::KThread() {
    /* prepare the kernel thread */
    pcb = new PCB(0);
    pcb->stack = new unsigned[65536];
    unsigned *kPtr = this->stack + 65536;
    *(--kPtr) = 0x200;
    *(--kPtr) = FP_SEG(Kernel::syscall);
    *(--kPtr) = FP_OFF(Kernel::syscall);

    kPtr -= 9;

    pcb->sp = FP_OFF(kPtr);
    pcb->ss = FP_SEG(this->stack);
    pcb->bp = FP_OFF(kPtr);
}

KThread::takeOver(unsigned callID, unsigned data_seg, unsigned data_off) {
    void *top = pcb->stack + pcb->stackSize;
    _SP = FP_OFF(top);
    _SS = FP_SEG(top);

    asm {
        mov ax, data_seg
        push ax

        mov ax, data_off
        push ax


        mov ax, callID
        push ax


        mov ax, seg this
        push ax

        mov ax, offset this
        push ax


        mov ax, seg KThread::dispatchSyscall
        push ax

        mov ax, offset KThread::dispatchSyscall
        push ax

        ret /* exit point */
    }
}

KThread::dispatchSyscall(unsigned callID, void *data) {
    switch(callID) {
        case 102:
            KThread::newThread((ThreadData*)data);
            break;
        case default:
            cout << "Inconsistent syscall!";
            break;
    }

    _SP = running->sp;
    _SS = running->ss;

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
        iret
    }
}

KThread::newThread(ThreadData* data) {

    cout << "New thread starting. Syscall: " << kThread->ax << endl;

    PCB* newPCB = new PCB(data->timeSlice);

    /* create a stack for the thread t
     * thread needs to be listed because
     * of the this pointer */
    newPCB->createStack(data->_this, data->_run, data->stack_size);

    /* enlist the thread in the available PCBs vector
     * not all PCBs need to be enlisted - hence this isn't in the constructor.
     * kernel PCB remains unlisted and is switched to manually */
    data->tid = PCBs->append(newPCB);

    cout << "New PCB created and enlisted." << endl;
}