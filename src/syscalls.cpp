/* default includes */
#include <iostream.h>
#include <dos.h>

/* dependencies */
#include <pcb.h>            /* handles PCBs */
#include <ffvector.h>       /* handles PCB storing */
#include <tdata.h>          /* thread transient data */
#include <sdata.h>          /* semaphore transient data */
#include <syscalls.h>       /* enumerated list of syscalls */
#include <scheduler.h>      /* scheduler implementation */
#include <kernsem.h>        /* handles kernel */
#include <kernev.h>
#include <sleepq.h>
#include <ithread.h>


/* kerenl.cpp */
extern PCB *running;
extern ffvector<PCB*>* PCBs;
extern ffvector<KernSem*>* KernSems;
extern ffvector<KernEv*>* KernEvs;
extern unsigned int tick;
extern SleepQ sleeping;
extern IThread *iThread;
extern bool kernel_mode;

bool dont_schedule = 0;


void newThread(ThreadData* data) {
    PCB* newPCB = new PCB(data->timeSlice);

    newPCB->createStack(data->_this, data->_run, data->stackSize);

    /* enlist the thread in the available PCBs vector
     * not all PCBs need to be enlisted - hence this isn't in the constructor.
     * kernel PCB remains unlisted and is switched to manually */
    data->tid = PCBs->append(newPCB);
    newPCB->id = data->tid;
}


void dispatch() {
    //cout << "Switching from: " << running->id << endl;

    if(!running->done && dont_schedule == 0)
        Scheduler::put(running);

    do {
        running = Scheduler::get();
        if(!running) break; /* if there's nothing to schedule */
    } while(running->done); /* if the newly fetched thread is marked done
                              (terminated), pop it, and find another */

    //if(running != 0)
        //cout << "Switching to: " << running->id << endl;

    dont_schedule = 0;
}


void startThread(ThreadData *data) {
    Scheduler::put((*PCBs)[data->tid]);
}


void endThread(ThreadData *data) {
    PCB *threadThatIsUnblocked;
    PCB *threadThatEnds = (*PCBs)[data->tid];

    threadThatEnds->done = 1;

    //cout << "Thread ending " << data->tid << endl;

    while(!threadThatEnds->waitingOn.empty()) {
        threadThatIsUnblocked = threadThatEnds->waitingOn.get();
        Scheduler::put(threadThatIsUnblocked);
    }
}


void waitToComplete(ThreadData *data) {
    //cout << "Running (" << running->id << ") waiting on " << data->tid << endl;
    //
    PCB* waitOnPCB = (*PCBs)[data->tid];

    if(!waitOnPCB->done) {
        waitOnPCB->waitingOn.put(running);

        dont_schedule = 1;
    }
}


void sleep(ThreadData *data) {
    sleeping.put(running, data->timeSlice); /* time slice - sleeping time */

    dont_schedule = 1; /* do not put the PCB back into scheduler, flag */
}


void newSemaphore(int *init) {
    KernSem *sem = new KernSem(*init);
    *init = KernSems->append(sem);   // init returns the SID; ugly, but faster
    sem->sid = *init;
}


void deleteSemaphore(unsigned *sid) {
    KernSem *sem = (*KernSems)[*sid];
    PCB *pcb;

    while(pcb = sem->blocked.get())
        Scheduler::put(pcb);

    KernSems->remove(*sid);

    delete sem;
}


void waitSem(unsigned *sid) {
    (*KernSems)[*sid]->wait();
}


void sigSem(unsigned *sid) {
    (*KernSems)[*sid]->signal();
}


void semVal(int *sid) {
    *sid = (*KernSems)[*sid]->value;
}


void newEvent(unsigned *eid) {
    KernEv *ev = new KernEv(IVTNo ivtNo);
    *eid = KernEvs->append(ev);
    ev->eid = *eid;
}


void waitEv(unsigned *eid) {
    (*KernEvs)[*eid]->wait();
}


void sigEv(unsigned *eid) {
    (*KernEvs)[*eid]->signal();
}


void deleteEvent(unsigned *eid) {
    KernEv *ev = (*KernEvs)[*eid];
    PCB *pcb = ev->creator;

    /* this ought not be necessary, only the creator should be doing the
     * destruction. is there any other scenario? */
    //if(ev->val < 0)
        //Scheduler::put(pcb);

    KernEvs->remove(*eid);

    delete ev;
}


void dispatchSyscall(unsigned callID, void *data) {
    //cout << "CallID: " << callID << endl;
    switch(callID) {
        case SYS_dispatch:
            break;
        case SYS_newthread:
            newThread((ThreadData*)data);
            break;
        case SYS_startthread:
            startThread((ThreadData*)data);
            break;
        case SYS_endthread:
            endThread((ThreadData*)data);
            break;
        case SYS_waittocomplete:
            waitToComplete((ThreadData*)data);
            break;
        case SYS_sleep:
            sleep((ThreadData*)data);
            break;

        case SYS_newsem:
            newSemaphore((int*)data);
            break;
        case SYS_deletesem:
            deleteSemaphore((unsigned*)data);
            break;
        case SYS_sigsem:
            sigSem((unsigned*)data);
            break;
        case SYS_waitsem:
            waitSem((unsigned*)data);
            break;
        case SYS_semval:
            semVal((int*)data);
            break;

        case SYS_newevent:
            newEvent((unsigned*)data);
            break;
        case SYS_sigev:
            sigEv((unsigned*)data);
            break;
        case SYS_waitev:
            waitEv((unsigned*)data);
            break;
        case SYS_deleteevent:
            deleteEvent((unsigned*)data);
            break;
        default:
            cout << "Inconsistent syscall! " << callID << endl;
            break;
    }

    dispatch(); /* change the active running */

    if(running != 0) { /* non-sleeping dispatched thread exists */
        tick = running->timeSlice;

        _SP = running->sp;
        _SS = running->ss;
    } else {
        iThread->takeOver(); /* exit point */
    }

    /* this piece of code happens only when switching to regular pcbs */

    kernel_mode = 0; /* mark the fact that we can switch from now on */

    asm {
        pop bp
        pop di
        pop si
        pop ds
        pop es
        pop dx
        pop cx
        pop bx
        pop ax

        iret /* exit point */
    }
}
