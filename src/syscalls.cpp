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

void newThread(ThreadData* data) {
    PCB* newPCB = new PCB(data->timeSlice);

    newPCB->createStack(data->_this, data->_run, data->stackSize);

    /* enlist the thread in the available PCBs vector
     * not all PCBs need to be enlisted - hence this isn't in the constructor.
     * kernel PCB remains unlisted and is switched to manually */
    data->tid = Kernel::PCBs->append(newPCB);
    newPCB->id = data->tid;
}


void fetch_next_running_or_idle() {
    do {
        Kernel::running = Scheduler::get();

        #ifdef DEBUG__THREADS
        cout << "Thread with state " << PCBStateName[Kernel::running->state] << " fetched from scheduler" << endl << flush;
        #endif

        if(Kernel::running == 0) Kernel::iThread.takeOver(); /* if there's nothing to schedule */
    } while(Kernel::running->state != STATE_ready); /* if the newly fetched thread is marked stopped
                                                (terminated), pop it, and find another */
}


void switch_context() {
    #ifdef DEBUG__THREADS
    cout << "[Switch] Thread " << Kernel::running->id << " SP adr: " << Kernel::running->sp << endl << flush;
    #endif
    #ifdef DEBUG__VERBOSE
    cout << "[syscall]====================| syscall done" << endl << flush;
    #endif

    asm cli;

    Kernel::state = STATE_working;

    _SP = Kernel::running->sp;
    _SS = Kernel::running->ss;

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


/* it should be named dispatch, but the specification requires the syscall
 * wrapper to be named dispatch() */
void _dispatch() {
    #ifdef DEBUG__THREADS
    cout << "Switching from: " << Kernel::running->id << endl << flush;
    #endif

    if(Kernel::running->state == STATE_ready) {
        /* the scheduler code can be moved to the PCB, consider that */
        /* argument against the idea - PCB should be decoupled from the kernel
         * implementation? */

        #ifdef DEBUG__THREADS
        cout << "Schedule thread ID: " << Kernel::running->id << endl << flush;
        #endif

        Scheduler::put(Kernel::running);
    }
    #ifdef DEBUG__THREADS
    else {
        cout << "Thread ID " << Kernel::running->id << ", state " << PCBStateName[Kernel::running->state] << "; not scheduled." << endl << flush;
    }
    #endif

    #ifdef DEBUG__THREADS
    cout << "[DISPATCH] Active threads: " << Kernel::active_threads << endl << flush;
    cout << "[DISPATCH] Scheduled threads: " << Kernel::ready_threads << endl << flush;
    cout << "[DISPATCH] Blocked threads: " << Kernel::blocked_threads << endl << flush;
    #endif

    /* if the thread is not running anymore (blocked, sleeping, stopped) */
    if(Kernel::running->state != STATE_running) {
        //fetch_next_running_or_idle(); /* exit point if we're idling */
        if((Kernel::running = Scheduler::get()) == 0) {
            Kernel::iThread.takeOver();
        }

        Kernel::running->dispatch();
        #ifdef DEBUG__THREADS
        cout << "Thread " << Kernel::running->id << " dispatched." << endl << flush;
        #endif
        switch_context(); /* exit point if we're working */
    }
}


void startThread(ThreadData *data) {
    PCB* pcb = (*Kernel::PCBs)[data->tid];
    Scheduler::put(pcb);
    pcb->start(); /* mark the state as STATE_ready */
}


void endThread(ThreadData *data) {
    PCB *unblocked;
    PCB *ending = (*Kernel::PCBs)[data->tid];

    ending->stop();

    #ifdef DEBUG__THREADS
    cout << "Thread ending " << data->tid << endl << flush;
    #endif

    while(!ending->blocking.empty()) {
        unblocked = ending->blocking.get();
        Scheduler::put(unblocked);
        unblocked->unblock();
    }
}


void waitToComplete(ThreadData *data) {
    PCB* target = (*Kernel::PCBs)[data->tid];

    if(target->state == STATE_ready || target->state == STATE_blocked) {
        target->blocking.put(Kernel::running);
        Kernel::running->block();

        #ifdef DEBUG__THREADS
        cout << "Joining with thread [" << target->id << "]" << endl << flush;
        #endif

        _dispatch();
    }
    #ifdef DEBUG__THREADS
    else if(target->state == STATE_stopped)
    {
        cout << "Thread already finished. [" << target->id << "] Not blocking." << endl << flush;
    }
    else {
        cout << "Blocking on a thread with illegal thread state (" << PCBStateName[target->state] << ")" << endl << flush;
    }
    #endif
}


void sleep(ThreadData *data) {
    Kernel::running->sleep(data->timeSlice);
}


void newSemaphore(int *init) {
    KernSem *sem = new KernSem(*init);

    *init        = Kernel::KernSems->append(sem);   // init returns the SID; ugly, but faster
    sem->sid     = *init;

    #ifdef DEBUG__SEMAPHORES
    cout << "[" << sem->sid << "] Semaphore initial value: " << sem->value << endl << flush;
    #endif
}


void deleteSemaphore(unsigned *sid) {
    KernSem *sem = (*Kernel::KernSems)[*sid];
    PCB     *pcb;

    while(pcb = sem->blocked.get())
        Scheduler::put(pcb);

    Kernel::KernSems->remove(*sid);

    delete sem;
}


void waitSem(unsigned *sid) {
    (*Kernel::KernSems)[*sid]->wait();
}


void sigSem(unsigned *sid) {
    (*Kernel::KernSems)[*sid]->signal();
}


void semVal(int *sid) {
    *sid = (*Kernel::KernSems)[*sid]->value;
}


void newEvent(unsigned *ivt) {
    KernEv *ev = new KernEv();

    IVT[*ivt]->setKernEv(ev);

    /* return the new eid using the ivt ptr */
    *ivt = Kernel::KernEvs->append(ev);
    ev->eid = *ivt;
}


void waitEv(unsigned *eid) {
    (*Kernel::KernEvs)[*eid]->wait();
}


void sigEv(unsigned *eid) {
    (*Kernel::KernEvs)[*eid]->signal();
}


void deleteEvent(unsigned *eid) {
    KernEv  *ev  = (*Kernel::KernEvs)[*eid];
    PCB     *pcb = ev->creator;

    /* this ought not be necessary, only the creator should be doing the
     * destruction. is there any other scenario? */
    //if(ev->val < 0)
        //Scheduler::put(pcb);

    Kernel::KernEvs->remove(*eid);

    delete ev;
}


void dispatchSyscall(unsigned callID, void *data) {
    #ifdef DEBUG__VERBOSE
    cout << "[syscall]==============================| " << callNames[callID] << endl << flush;
    #endif

    switch(callID) {
        case SYS_dispatch:
        case SYS_preempt:
            /* both timer and manual dispatching set the thread to ready
             * the switch is executed in _dispatch at the end of syscall */
            if(Kernel::running->state == STATE_running)
                Kernel::running->schedule();
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
            _dispatch();
            break;
        case SYS_waittocomplete:
            waitToComplete((ThreadData*)data);
            /* conditional dispatch inside the call */
            break;
        case SYS_sleep:
            sleep((ThreadData*)data);
            _dispatch();
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
            /* conditional dispatch inside the call */
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
            /* conditional dispatch inside the call */
            break;
        case SYS_deleteevent:
            deleteEvent((unsigned*)data);
            break;
        default:
            #ifdef DEBUG__VERBOSE
            cout << "Inconsistent syscall! " << callID << endl << flush;
            #endif
            break;
    }

    #ifdef DEBUG__THREADS
    cout << "Active threads: " << Kernel::active_threads << endl << flush;
    cout << "Scheduled threads: " << Kernel::ready_threads << endl << flush;
    cout << "Blocked threads: " << Kernel::blocked_threads << endl << flush;
    #endif

    switch_context();
}
