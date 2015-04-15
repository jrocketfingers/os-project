#include "os.h"

#define KERNEL_STACK_SIZE 1024

#define savestack asm {     \
    mov SP, sp              \
    mov SS, ss              \
    mov sp, kSP             \
    mov ss, kSS             \
}

#define restorestack asm {  \
    mov kSP, sp             \
    mov kSS, ss             \
    mov sp, SP              \
    mov ss, SS              \
}

REG SP;
REG SS;

REG kSP;
REG kSS;
word *kstack;

void interrupt syscall() {
    savestack

    /* yield call */
    if(_AX == 0) {
        cout << "Thread " << caller << " yielding!";
    }

    restorestack
}


int OS::last_id = 0;


void OS::init() {
    lock

    kstack = new word[1024];

    kSP = FP_OFF(kstack+1023);
    kSS = FP_SEG(kstack+1023);
    kBP = FP_OFF(kstack+1023);

    unlock
}


void OS::yield() {
    asm {
        mov ax, 0;
        mov bx, 10;
        ins 60h;
    }
}


void OS::dispatch() {

}
