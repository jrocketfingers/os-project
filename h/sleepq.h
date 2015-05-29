#ifndef __H_SLEEPQ__
#define __H_SLEEPQ__

#include <api/types.h>
#include <pcb.h>

class SleepQ {
public:
    SleepQ() {
        head = 0;
    }

    struct Elem {
        Elem(PCB *sleeper, Time sleep) { pcb = sleeper; next = 0; ticks = sleep; }

        Elem* next;
        PCB* pcb;
        Time ticks;
    };

    Elem *head;

    void put(PCB* sleeper, Time sleep);

    void tick();
};

#endif
