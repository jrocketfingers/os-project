#include <iostream.h>
#include <dos.h>
#include <kernel.h>
#include <ffvector.h>
#include <queue.h>
#include <semaphor.h>

#define THDS 10

int i = 0;

Semaphore *s;

class TestThread : public Thread {
public:
    TestThread(int v) : Thread() { this->v = v; }

    void run() {
        //cout << "Waiting. " << v << endl;
        //s->wait();
        //cout << "Locked. " << v << ";" << endl;
        for(unsigned long i = 0; i < 1000; i++);
        //cout << "Finished." << v << endl;
        //s->signal();
    }

private:
    int v;
};



int userMain(int argc, char *argv[]) {
    cout << "In userMain." << endl;

    s = new Semaphore();

    TestThread **t = new TestThread*[THDS];
    for(int thi = 0; thi < THDS; thi++) {
        t[thi] = new TestThread(thi);
        cout << "Making thread " << thi << endl;
        t[thi]->start();
    }

    for(int thj = 0; thj < THDS; thj++) {
        cout << "Waiting for: " << thj << endl;
        t[thj]->waitToComplete();
    }

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