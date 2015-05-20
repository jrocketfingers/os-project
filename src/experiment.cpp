#include <iostream.h>
#include <dos.h>
#include <kernel.h>
#include <ffvector.h>

class TestThread : public Thread {
public:
    TestThread() : Thread() { }

    void run() {
        for(unsigned long i = 0; i < 100000; i++)
            if(i % 10000 == 0)
                cout << "Thread: " << i << endl;

        cout << "Finished thread." << endl;
    }
};



int userMain(int argc, char *argv[]) {
    cout << "In userMain." << endl;

    unsigned int *a = new unsigned int[128000];
    unsigned int *b = a + 120000;
    cout << FP_SEG(a) << "; " << FP_SEG(b) << endl;

    //TestThread t1 = TestThread();
    //t1.start();

    //for(unsigned long i = 0; i < 4000000000; i++);

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