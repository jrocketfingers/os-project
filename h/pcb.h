#ifndef __PCB_H__
#define __PCB_H__

#include <thread.h>
#include <types.h>

class PCB {                      // Kernel's implementation of a user's thread
public:
    PCB(Time timeSlice)
    {
        this->timeSlice = timeSlice;
    }

    void createStack(void* t, void* run, StackSize stack_size);
    void enlist(Thread* t);

    unsigned int id;

    word sp, ss;

    bool done;
    Time timeSlice;

    unsigned int *stack;
};

#endif