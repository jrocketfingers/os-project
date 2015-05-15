#include <iostream.h>
#include <dos.h>
#include <kernel.h>
#include <ffvector.h>

unsigned int oldval, newval;


int userMain(int argc, char *argv[]) {
    //Kernel::init();
    cout << "In userMain.";

    unsigned int *stack = new unsigned int[64000];
    unsigned int sp = FP_OFF(stack+63999);
    unsigned int ss = FP_SEG(stack+63999);
    unsigned int *topofstack = (unsigned int*)MK_FP(ss, sp);

    stack[63999] = 102;

    cout << "STACK: " << stack[63999] << endl;
    cout << "TOS: " << *topofstack << endl;

    delete[] stack;

    return 0;
}



void ffvectest(void) {
    ffvector<int> testvec(3);

    cout << "Empty length: " << testvec.length() << endl;

    testvec.append(5);
    testvec.append(4);
    testvec.append(3);
    testvec.append(2);

    testvec.remove(2);
    testvec.remove(3); // ERROR, OUT OF RANGE, didn't report

    testvec.append(10);
    testvec.append(12);

    for(int i = 0; i < testvec.length(); i++) {
        cout << testvec[i] << endl;
    }

    cout << "Used: " << testvec.usage() << endl;
    cout << "Highest used: " << testvec.length() << endl;
    cout << "Capacity: " << testvec.actual_size() << endl;
}