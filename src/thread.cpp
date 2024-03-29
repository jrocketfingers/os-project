#include <iostream.h>

#include <thread.h>

Thread::Thread (StackSize stackSize, Time timeSlice) {
    this->tid = sys_newthread(this, Thread::call, stackSize, timeSlice);
}


void Thread::start() {
    sys_startthread(this->tid);
}


Thread::~Thread() {
    waitToComplete();
}


void Thread::waitToComplete() {
    sys_waittocomplete(tid);
}


/* Static method */
void Thread::sleep(Time timeToSleep) {
    if(timeToSleep != 0)
        sys_sleep(timeToSleep);
}

void Thread::call(Thread* t) {
    t->run();

    sys_endthread(t->tid);
}