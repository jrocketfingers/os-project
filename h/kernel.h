#ifndef __KERNEL_H__
#define __KERNEL_H__

#include <thread.h>
#include <dos.h>
#include <api/types.h>
#include <ffvector.h>

#define lock asm cli
#define unlock asm sti

class Kernel {
public:
    static void init();
    static void stop();

    static tid_t enlistPCB(PCB *newPCB);
};

#endif