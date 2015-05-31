#ifndef __KERNEL_H__
#define __KERNEL_H__

#include <thread.h>
#include <dos.h>
#include <api/types.h>

#include <kthread.h>
#include <ithread.h>

#include <sleepq.h>
#include <ffvector.h>

#define lock asm cli
#define unlock asm sti

enum KernelState {
    STATE_idling=1,
    STATE_working,
    STATE_wakeup,
    STATE_kmode
};

/* forward declarations */
class KernSem;
class KernEv;
class PCB;

class Kernel {
public:
    static void init();
    static void stop();

    static tid_t enlistPCB(PCB *newPCB);
    static KernelState state;

    static void wake() { state = STATE_wakeup; }
    static void idle() { state = STATE_idling; }
    static void work() { state = STATE_working; }

    static int active_threads;
    static int ready_threads;
    static int blocked_threads;
    static int sleeping_threads;

    static SleepQ sleeping;

    static ffvector<PCB*>* PCBs;
    static ffvector<KernSem*>* KernSems;
    static ffvector<KernEv*>* KernEvs;

    static KThread kThread;
    static IThread iThread;

    static PCB *running;

    static int tick;
};

#endif