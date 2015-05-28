#ifndef __H_IVTENTRY__
#define __H_IVTENTRY__

#include <kernev.h>

typedef void interrupt (*ISR)();

class IVTEntry {
public:
    IVTEntry(IVTNo ivtNo, ISR newISR); /* sets up the entry in the entry table */
    ~IVTEntry(); /* restores the old interrupt routine */

    void setKernEv(KernEv *ev);

    KernEv *kevent;

    ISR oldISR;

private:
    unsigned oldISR_seg;
    unsigned oldISR_off;
};

extern IVTEntry* IVT[256];

#endif
