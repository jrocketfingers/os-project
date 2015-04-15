#include <iostream.h>
#include <dos.h>


void interrupt syscallISR() {
    asm {

    }
}

unsigned int kSP, kSS;

int main(void) {
    kSP = _SP;
    kSS = _SS;

    unsigned int *kstack = new unsigned int[1024];

    _SP = FP_OFF(kstack+1023);
    _SS = FP_SEG(kstack+1023);
    _BP = FP_OFF(kstack+1023);

    int a = 10, b = 11, c = 12, d = 13, e = 14, f = 15, g = 16, h, i, j, k, l, m, n;

    a++;

    delete[] kstack;

    return 0;
}