/* default includes */
#include <dos.h>        /* FP_OFF, FP_SEG, MK_FP macros */
#include <stdlib.h>     /* exit */

#include <pcb.h>        /* handles PCBs */
#include <ffvector.h>   /* handles PCB storage */

#include <debug.h>

extern ffvector<PCB*>* PCBs;

void PCB::createStack(void* _this, void* _run, StackSize stackSize) {
    unsigned *sPtr;

    stack = new unsigned int[stackSize];
    sPtr = stack + stackSize;

    sp = FP_OFF(stack+stackSize);
    ss = FP_SEG(stack+stackSize);

    *(--sPtr) = FP_SEG(_this); // this for the static thread call wrapper
    *(--sPtr) = FP_OFF(_this);

    sPtr -= 2; // skip first call's cs and ip

    *(--sPtr) = 0x200; // set the enabled interrupt flag
    *(--sPtr) = FP_SEG(_run); // add the return path from the interrupt
    *(--sPtr) = FP_OFF(_run);

    /* Add space for register data (is not clean!) */
    sPtr -= 9;

    //*(--sPtr) = FP_OFF(sPtr - 11); [> initially old BP of the interrupt call
                                      //should refer to the wrappers stack frame */

    this->sp = FP_OFF(sPtr);
    this->ss = FP_SEG(sPtr);
}

void PCB::start() {
    if(state == STATE_new) {
        state = STATE_ready;
        Kernel::active_threads++;
    }
    else {
#ifdef DEBUG__THREADS
        cout << "PCB::start ERROR: Old thread trying to start!" << endl;
        cout << "PCBs state was: " << PCBStateName[state] << endl;
        cout << "PCBs ID was: " << id << endl;
#endif
        exit(1);
    }
}

void PCB::schedule() {
    if(state == STATE_running) {
        state = STATE_ready;
    }
    else {
#ifdef DEBUG__THREADS
        cout << "PCB::schedule ERROR: Non-running thread scheduling!" << endl;
        cout << "PCBs state was: " << PCBStateName[state] << endl;
        cout << "PCBs ID was: " << id << endl;
#endif
        exit(1);
    }
}

void PCB::stop() {
    state = STATE_stopped;
    Kernel::active_threads--;
}

void PCB::dispatch() {
    if(state == STATE_ready) {
        state = STATE_running;
    }
    else {
#ifdef DEBUG__THREADS
        cout << "PCB::dispatch ERROR: Thread is not ready (blocked, new or stopped)!" << endl;
        cout << "PCBs state was: " << PCBStateName[state] << endl;
        cout << "PCBs ID was: " << id << endl;
#endif
        exit(1);
    }
}

void PCB::block() {
    if(state == STATE_running) {
        state = STATE_blocked;
        Kernel::active_threads--;
        Kernel::blocked_threads++;
    }
    else {
#ifdef DEBUG__THREADS
        cout << "PCB::block ERROR: Non-running thread blocking!" << endl;
        cout << "PCBs state was: " << PCBStateName[state] << endl;
        cout << "PCBs ID was: " << id << endl;
#endif
        exit(1);
    }
}

void PCB::unblock() {
    if(state == STATE_blocked) {
        state = STATE_ready;
        Kernel::active_threads++;
        Kernel::blocked_threads--;
    }
    else {
#ifdef DEBUG__THREADS
        cout << "PCB::unblock ERROR: Only a blocked thread can be unblocked!" << endl;
        cout << "PCBs state was: " << PCBStateName[state] << endl;
        cout << "PCBs ID was: " << id << endl;
#endif
        exit(1);
    }
}

void PCB::sleep(Time timeSlice) {
    this->block();
    Kernel::sleeping_threads++;
    Kernel::active_threads--;

    Kernel::sleeping.put(Kernel::running, timeSlice); /* time slice - sleeping time */
}
