#ifndef __H_EVENT__
#define __H_EVENT__

#include <api_type.h>
#include <api_sys.h>

#include <ivtentry.h>

#define PREPAREENTRY(IvtNo, old) \
void interrupt ISR##IvtNo(...); \
IVTEntry entry##IvtNo(IvtNo, &ISR##IvtNo); \
void interrupt ISR##IvtNo(...) { \
    if(old) entry##IvtNo.oldISR(); \
    entry##IvtNo.signalEv(); \
    if(Kernel::state == STATE_kmode) { \
        Kernel::event_flag = 1; \
    } else { \
        dispatch(); \
    } \
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
