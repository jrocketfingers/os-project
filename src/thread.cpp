#include <api/types.h>
#include <api/syscalls.h>
#include <thread.h>

Thread::Thread (StackSize stackSize, Time timeSlice) {
    sys_newthread(this, Thread::call, stackSize, timeSlice);
}


void Thread::start() {
    sys_startthread(this->tid);
}

Thread::~Thread() {}


void Thread::waitToComplete() {

}


/* Static method */
void Thread::sleep(Time timeToSleep) {

}

void Thread::call(Thread* t) {
    t->run();

    sys_endthread(t->tid);
}