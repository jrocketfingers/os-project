#ifndef __H_SLEEPQ__
#define __H_SLEEPQ__

#include <api/types.h>
#include <scheduler.h>

/* debugging purposes */
#include <iostream.h>

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

    void put(PCB* sleeper, Time sleep) {
        Elem* newSleeper = new Elem(sleeper, sleep);

        if(head) {
            Elem *cursor = head;
            Elem *prev = 0;
            while(cursor && cursor->ticks < sleep) {
                prev = cursor;
                cursor = cursor->next;
                newSleeper->ticks -= prev->ticks;
            }

            prev->next = newSleeper;

            while(cursor) {
                cursor->ticks -= newSleeper->ticks;
                cursor = cursor->next;
            }
        }
        else
            head = newSleeper;
    }

    void tick() {
        if(head) {
            /* in case there is more than one sleeping thread with the same tick */
            head->ticks--;
            while(head && head->ticks == 0) {
                Elem *old;
                Scheduler::put(head->pcb);
                old = head;

                head = head->next;

                delete old;
            }
        }
    }
};

#endif
