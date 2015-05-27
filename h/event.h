#ifndef __H_EVENT__
#define __H_EVENT__

#include <api/types.h>

#include <ivtentry.h>

#define PREPAREENTRY(IvtNo, old) \
IVTEntry entry##IvtNo(IvtNo); \
void ISR##IvtNo() { \
    if(old) entry##IvtNo.oldISR(); \
    sys_sigev(IvtNo); \
}

class Event {
public:
    Event(IVTNo ivtNo);
    ~Event();

    void wait();

private:
    ID eid;
};

#endif
