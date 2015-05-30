#include <sleepq.h>

#include <api/syscalls.h>
#include <schedule.h>
#include <kernel.h>

#include <debug.h>


void SleepQ::put(PCB* sleeper, Time sleep) {
    sleeper->block();

    Elem* newSleeper = new Elem(sleeper, sleep);

    if(head) {
        Elem *cursor = head;
        Elem *prev = 0;
        while(cursor && cursor->ticks <= sleep) {
            prev = cursor;
            cursor = cursor->next;
            newSleeper->ticks -= prev->ticks;
        }

        /* if the thread should be first */
        if(cursor == head) {
            newSleeper->next = head;
            head = newSleeper;
        } else
            prev->next = newSleeper;

        while(cursor) {
            cursor->ticks -= newSleeper->ticks;
            cursor = cursor->next;
        }
    }
    else
        head = newSleeper;

#ifdef DEBUG__THREADS
    /* diagnostics */
    cout << "Put to sleep: " << sleeper->id << "; for " << sleep << " ticks." << endl;
#endif
}


void SleepQ::tick() {
    if(head) {
        /* in case there is more than one sleeping thread with the same tick */
        head->ticks--;

        while(head && head->ticks == 0) {
            Elem *old;
            Scheduler::put(head->pcb);
            head->pcb->unblock();
            old = head;

            head = head->next;

            delete old;
        }
    }

    /* diagnostics */
    //Elem *cursor = head;
    //while(cursor) {
        //cout << "[ID " << cursor->pcb->id << "; t " << cursor->ticks << "]--";
        //cursor = cursor->next;
    //}

    //cout << endl;
}
