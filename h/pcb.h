#ifndef __PCB_H__
#define __PCB_H__

/* data structures */
#include <queue.h>

/* public apis */
#include <api/types.h>

class PCB {                      // Kernel's implementation of a user's thread
public:
    PCB(Time timeSlice) {
        this->timeSlice = timeSlice;
    }

    void createStack(void* t, void* run, StackSize stack_size);


    word sp, ss;

    bool done;
    Time timeSlice;
    unsigned int id;

    Queue<PCB*> waitingOn;

    unsigned int *stack;
};

#endif