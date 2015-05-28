/* system libraries */
#include <dos.h>

/* module declaration */
#include <ivtentry.h>


IVTEntry* IVT[256];

IVTEntry::IVTEntry(IVTNo ivtno, ISR newISR) {
    kevent = 0;
    IVT[ivtno] = this;

    asm {
        push es
        push ax
        push bx
        mov ax, 0
        mov es, ax
    }


    asm cli;

    _BX = ivtno*4;
    asm mov ax, word ptr es:bx;
    oldISR_off = _AX;

    _BX = ivtno*4 + 2;
    asm mov ax, word ptr es:bx;
    oldISR_seg = _AX;

    oldISR = (ISR)MK_FP(oldISR_seg, oldISR_off);


    _AX = FP_OFF(newISR);
    _BX = ivtno*4;
    asm mov word ptr es:bx, ax;

    _AX = FP_SEG(newISR);
    _BX = ivtno*4 + 2;
    asm mov word ptr es:bx, ax;

    asm sti;


    asm pop bx;
    asm pop ax;
    asm pop es;
}


IVTEntry::~IVTEntry() {
    asm push ax;
    asm push es;


    /* let's make sure this is atomic */
    asm cli;
    _AX = oldISR_off;
    asm mov word ptr es:0180h, ax;
    _AX = oldISR_seg;
    asm mov word ptr es:0182h, ax;
    asm sti;


    asm pop es;
    asm pop ax;
}

void IVTEntry::setKernEv(KernEv* ev) {
    kevent = ev;
}
