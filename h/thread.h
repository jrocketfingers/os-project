#ifndef __THREAD_H__
#define __THREAD_H__

#include <stddef.h>
#include <syscalls.h>
#include <tdata.h>

#include <api/types.h>

const StackSize defaultStackSize = 4096;
const Time defaultTimeSlice = 2;            // x 55ms
                                            //default = 2*55ms

class PCB;

class Thread {
public:
    void start();
    void waitToComplete();
    virtual ~Thread();
    static void sleep(Time timeToSleep);

    friend class PCB;

protected:
    Thread (StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice);
    virtual void run() {}

    static void call(Thread* t);

    tid_t tid;
};


#endif