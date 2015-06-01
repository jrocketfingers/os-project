#include <sleepq.h>

#include <api/syscalls.h>
#include <schedule.h>
#include <kernel.h>
#include <pcb.h>

#include <debug.h>


void SleepQ::put(PCB* sleeper, Time sleep) {
    sleeper->block();

    Elem* newSleeper = new Elem(sleeper, sleep);

    if(head) {
        Elem *cursor = head;
        Elem *prev = 0;
        while(cursor && cursor->ticks <= newSleeper->ticks) {
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

        Time rest = newSleeper->ticks;
        Time decr = newSleeper->ticks;
        /* decaying decrementing the differential queue
         * wow, I just made up a buncha CS names */
        while(cursor) {
            decr = cursor->ticks;
            cursor->ticks -= rest;
            if(decr > rest) break;
            rest -= decr;
            cursor = cursor->next;
        }
    }
    else
        head = newSleeper;

    #ifdef DEBUG__THREADS
    /* diagnostics */
    cout << "Put to sleep: " << sleeper->id << "; for " << sleep << " ticks." << endl << flush;
    #endif

    #ifdef DEBUG__SLEEP
    /* diagnostics */
    Elem *cursor = head;
    while(cursor) {
        cout << "[ID " << cursor->pcb->id << "; t " << cursor->ticks << "]--";
        cursor = cursor->next;
    }

    cout << endl << flush;
    #endif
}


void SleepQ::tick() {
    if(head) {
        /* in case there is more than one sleeping thread with the same tick */
        head->ticks--;

        while(head && head->ticks == 0) {
            Elem *old;

            head->pcb->unblock();
            #ifdef DEBUG__VERBOSE
            cout << "[SLEEPQ] Putting thread id " << head->pcb->id << " into scheduler." << endl << flush;
            cout << "[SLEEPQ] Thread's state is: " << PCBStateName[head->pcb->state] << endl << flush;
            #endif
            Scheduler::put(head->pcb);

            Kernel::state = STATE_wakeup;

            old = head;

            head = head->next;

            delete old;
        }
    }

    #ifdef DEBUG__SLEEP
    /* diagnostics */
    Elem *cursor = head;
    while(cursor) {
        cout << "[ID " << cursor->pcb->id << "; t " << cursor->ticks << "]--";
        cursor = cursor->next;
    }

    cout << endl << flush;
    #endif
}
