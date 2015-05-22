/* default includes */
#include <dos.h>        /* FP_OFF, FP_SEG, MK_FP macros */

#include <pcb.h>        /* handles PCBs */
#include <ffvector.h>   /* handles PCB storage */

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


void PCB::enlist(Thread* t) {
    t->tid = PCBs->append(this);
}
