#ifndef __KERNEL_H__
#define __KERNEL_H__

#include <types.h>
#include <ffvector.h>

#define lock asm cli
#define unlock asm sti



class PCB {                      // Kernel's implementation of a user's thread
public:
    PCB(unsigned int stackSize, const char *name) : name(name)
    {
        stack = new unsigned int[stackSize];
    }

    word sp;
    word ss;
    word bp;
    word cs;
    word pc;
    bool done;
    int timeSlice;
    unsigned int *stack;
    const char *name;
};



class Kernel {
public:
    static void init();
    static void waitFor(int tid);
    static void sleep(Time time);
    static void dispatch();
    static void yield();
    static void schedule(unsigned int tid);
    static void stop();

    static tid_t enlistPCB(PCB *newPCB);
private:
    static PCB *running;
    static ffvector<PCB*>* PCBs;
};

#endif