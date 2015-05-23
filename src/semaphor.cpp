#include <iostream.h>

#include <semaphor.h>

#include <api/syscalls.h>

Semaphore::Semaphore(int init) {
    sid = sys_newsem(init);
}

Semaphore::~Semaphore() {
    sys_deletesem(sid);
}

void Semaphore::signal() {
    sys_signal(sid);
}

void Semaphore::wait() {
    sys_wait(sid);
}

int Semaphore::val() const {
    return sys_semval(sid);
}
