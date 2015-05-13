#ifndef __THREAD_H__
#define __THREAD_H__

#include <stddef.h>
#include "os.h"
#include "types.h"

const StackSize defaultStackSize = 4096;
const Time defaultTimeSlice = 2;            // x 55ms
                                            //default = 2*55ms

class Thread {
public:
    void start();
    void waitToComplete();
    virtual ~Thread();
    static void sleep(Time timeToSleep);

    void yield() { OS::yield(); }

protected:
    friend class OS;
    friend class PCB;
    Thread (StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice);
    virtual void run() {}

private:
    word *stack;
    tid_t tid;

    void call();
};

#endif