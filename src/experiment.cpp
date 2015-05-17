#include <iostream.h>
#include <dos.h>
#include <kernel.h>
#include <ffvector.h>

class TestThread : public Thread {
public:
    TestThread() : Thread() { }

    void run() {
        cout << "Inside a thread.";
    }
};


int userMain(int argc, char *argv[]) {
    cout << "In userMain." << endl;

    TestThread t1 = TestThread();

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