/* default includes */
#include <dos.h>

/* dependencies */
#include <pcb.h>            /* handles PCBs */
#include <ffvector.h>       /* handles PCB storing */
#include <tdata.h>          /* thread transient data */
#include <sdata.h>          /* semaphore transient data */
#include <syscalls.h>       /* enumerated list of syscalls */
#include <scheduler.h>      /* scheduler implementation */
#include <kernsem.h>        /* handles kernel semaphores */
#include <kernel.h>
#include <ivtentry.h>
#include <kernev.h>
#include <sleepq.h>
#include <ithread.h>

#include <debug.h>


/* kerenl.cpp */
extern PCB *running;
extern ffvector<PCB*>* PCBs;
extern ffvector<KernSem*>* KernSems;
extern ffvector<KernEv*>* KernEvs;
extern unsigned int tick;
extern SleepQ sleeping;
extern IThread *iThread;
extern bool kernel_mode;

KernelState Kernel::state = 0;

void newThread(ThreadData* data) {
    PCB* newPCB = new PCB(data->timeSlice);

    newPCB->createStack(data->_this, data->_run, data->stackSize);

    /* enlist the thread in the available PCBs vector
     * not all PCBs need to be enlisted - hence this isn't in the constructor.
     * kernel PCB remains unlisted and is switched to manually */
    data->tid = PCBs->append(newPCB);
    newPCB->id = data->tid;
}


/* it should be named dispatch, but the specification requires the syscall
 * wrapper to be named dispatch() */
void _dispatch() {
#ifdef DEBUG__THREADS
    cout << "Switching from: " << running->id << endl;
#endif

    if(running->state == STATE_running) {
        /* the scheduler code can be moved to the PCB, consider that */
        /* argument against the idea - PCB should be decoupled from the kernel
         * implementation? */

#ifdef DEBUG__THREADS
        cout << "Putting thread ID: " << running->id << endl;
#endif

        Scheduler::put(running);
        running->schedule(); /* adjust the state accordingly */
    }

    do {
        running = Scheduler::get();
        if(running == 0) break; /* if there's nothing to schedule */
    } while(running->state == STATE_stopped); /* if the newly fetched thread is marked stopped
                                                (terminated), pop it, and find another */

    if(running) {
        running->dispatch();

#ifdef DEBUG__THREADS
        cout << "Getting thread: " << running->id << endl;
#endif
    }
}


void startThread(ThreadData *data) {
    PCB* pcb = (*PCBs)[data->tid];
    Scheduler::put(pcb);
    pcb->start(); /* mark the state as STATE_running */
}


void endThread(ThreadData *data) {
    PCB *threadThatIsUnblocked;
    PCB *threadThatEnds = (*PCBs)[data->tid];

    threadThatEnds->stop();

#ifdef DEBUG__THREADS
    cout << "Thread ending " << data->tid << endl;
#endif

    while(!threadThatEnds->waitingOn.empty()) {
        threadThatIsUnblocked = threadThatEnds->waitingOn.get();
        Scheduler::put(threadThatIsUnblocked);
        threadThatIsUnblocked->unblock();
    }
}


void waitToComplete(ThreadData *data) {
    PCB* waitOnPCB = (*PCBs)[data->tid];

    if(waitOnPCB->state != STATE_stopped) {
        waitOnPCB->waitingOn.put(running);
        running->block();
#ifdef DEBUG__THREADS
        cout << "Joining with thread [" << waitOnPCB->id << "]" << endl;
    } else {

        cout << "Thread already finished. [" << waitOnPCB->id << "] Not blocking." << endl;
#endif /* smooth - will shoot you in the foot if you touch it */
    }
}


void sleep(ThreadData *data) {
    sleeping.put(running, data->timeSlice); /* time slice - sleeping time */
}


void newSemaphore(int *init) {
    KernSem *sem = new KernSem(*init);
    *init = KernSems->append(sem);   // init returns the SID; ugly, but faster
    sem->sid = *init;

#ifdef DEBUG__SEMAPHORES
    cout << "[" << sem->sid << "] Semaphore initial value: " << sem->value << endl;
#endif
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


void newEvent(unsigned *ivt) {
    KernEv *ev = new KernEv();
    IVT[*ivt]->setKernEv(ev);

    /* return the new eid using the ivt ptr */
    *ivt = KernEvs->append(ev);
    ev->eid = *ivt;
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
#ifdef DEBUG__VERBOSE
    cout << "Call: " << callNames[callID] << endl;
#endif
    switch(callID) {
        case SYS_dispatch:
            _dispatch();
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
#ifdef DEBUG__VERBOSE
            cout << "Inconsistent syscall! " << callID << endl;
#endif
            break;
    }

    if(running->state != STATE_running)
        _dispatch(); /* change the active running */

    if(running != 0) { /* non-sleeping dispatched thread exists */
        tick = running->timeSlice;

        _SP = running->sp;
        _SS = running->ss;
    } else {
#ifdef DEBUG__THREADS
        cout << "Idling..." << endl;
#endif
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
