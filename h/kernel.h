#ifndef __KERNEL_H__
#define __KERNEL_H__

#include <thread.h>
#include <dos.h>
#include <api/types.h>
#include <ffvector.h>

#define lock asm cli
#define unlock asm sti

enum KernelState {
    STATE_idling=1,
    STATE_working,
    STATE_wakeup,
    STATE_kthread
};

class Kernel {
public:
    static void init();
    static void stop();

    static tid_t enlistPCB(PCB *newPCB);
    static KernelState state;

    static void wake() { state = STATE_wakeup; }
    static void idle() { state = STATE_idling; }
    static void work() { state = STATE_working; }
};

#endif