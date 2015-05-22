#include <iostream.h>
#include <dos.h>
#include <kernel.h>
#include <ffvector.h>
#include <queue.h>

int i = 0;

class TestThread : public Thread {
public:
    TestThread() : Thread() { }

    void run() {
        i++;
        cout << i << endl;
    }
};



int userMain(int argc, char *argv[]) {
    cout << "In userMain." << endl;

    //for(int th = 0; th < 20; th++) {
        //TestThread *t = new TestThread();
        //t->start();
    //}

    //for(unsigned long i = 0; i < 4000000000; i++);
    Queue<int> q;

    q.put(10);
    q.put(12);
    q.put(14);

    cout << q.get() << endl;
    cout << q.get() << endl;
    cout << q.get() << endl;

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