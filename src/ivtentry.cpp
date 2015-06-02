/* system libraries */
#include <dos.h>

/* module declaration */
#include <ivtentry.h>


IVTEntry* IVT[256];

IVTEntry::IVTEntry(IVTNo ivtno, ISR newISR) {
    cout << "Preparing event " << (int)ivtno << "; ISR: "<< newISR << endl;
    kevent      = 0;
    this->ivtno = ivtno;
    IVT[ivtno]  = this;

    asm cli;
        oldISR = getvect(ivtno);
        setvect(ivtno, newISR);
    asm sti;
}

IVTEntry::~IVTEntry() {
    asm cli;
    cout << "Destroying IVT " << ivtno << endl << flush;
    oldISR();
    setvect(ivtno, oldISR);
    asm sti;
}

void IVTEntry::setKernEv(KernEv* ev) {
    kevent = ev;
}

void IVTEntry::signalEv() {
    if(kevent == 0) {
        #ifdef DEBUG__EVENT
        cout << "[ERROR]: Event not yet set up!" << endl;
        #endif

        return;
    }

    kevent->signal();
}
