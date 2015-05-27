#ifndef __H_IVTENTRY__
#define __H_IVTENTRY__

#include <kernev.h>

typedef void interrupt (*ISR)();

class IVTEntry {
public:
    IVTEntry(IVTNo ivtNo, void interrupt (*ISR)()); /* sets up the entry in the entry table */
    ~IVTEntry(); /* restores the old interrupt routine */

    KernEv *kevent;

    ISR oldISR;

private:
    unsigned oldISR_seg;
    unsigned oldISR_off;
};

extern IVTEntry* IVT[256];

#endif
