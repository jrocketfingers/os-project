#include <event.h>

Event::Event(IVTNo ivtNo) {
    eid = sys_newev(ivtNo);
}

Event::~Event() {
    sys_deleteev(eid);
}

void Event::wait() {
    sys_waitev(eid);
}
