#ifndef __PCB_H__
#define __PCB_H__

/* data structures */
#include <queue.h>

/* public apis */
#include <api/types.h>

enum PCBState {
    STATE_new           =1,
    STATE_ready,
    STATE_running,
    STATE_stopped,
    STATE_blocked,
    STATE_idle
};

#ifdef DEBUG__THREADS
const char PCBStateName[][30] = {
    "new",
    "ready",
    "running",
    "stopped",
    "blocked",
    "idle"
};
#endif

class PCB {                      // Kernel's implementation of a user's thread
public:
    PCB(Time timeSlice) {
        this->timeSlice = timeSlice;
        this->state = STATE_new;
    }

    void createStack(void* t, void* run, StackSize stack_size);
    void deleteStack();

    void start();
    void stop();
    void block();
    void unblock();

    void schedule();
    void dispatch();

    word sp, ss;

    PCBState state;

    Time timeSlice;
    unsigned int id;

    Queue<PCB*> waitingOn;

    unsigned int *stack;
};

#endif