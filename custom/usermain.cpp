#include <iostream.h>
#include "queue.h"
#include "api_types.h"
#include "thread.h"
#include "DOS.H"
#include "semaphor.h"

#define THDS 2
#define STACK 1024

int cons = 0, dest = 0, runStart = 0, runEnd = 0;

Semaphore * sem = 0;

class TestThread : public Thread {
public:
    TestThread(int v) : Thread(STACK, 1) {
        //debug
        this->v = v;
        cons++;
    }

    void run();

    ~TestThread(){
        //debug
        dest++;
    }

private:
    int v;
};


void TestThread::run() {
    //sem->wait();
    for (unsigned long i = 0; i < 20000000; i++) {
        //if (i % 500000 == 0) {
            //asm cli;
            //cout << v  << ": " << i << endl << flush;
            //asm sti;
        //}
    }
    //Thread::sleep(15);
    asm cli;
    cout << v << ": " << "finished" << endl << flush;
    asm sti;
    //sem->signal();
}


int userMain(int argc, char * argv[]){
    sem = new Semaphore(1);
    int total = THDS;
    TestThread ** niti = new TestThread*[total];

    for (unsigned long i = 0; i < total; i++)
        niti[i] = new TestThread(i);
    for (i = 0; i < total; i++)
        niti[i]->start();
    //for (i = total-1; i >= 0; i--)
    //    niti[i]->start();
    for (i = 0; i < total; i++)
        delete niti[i];
    delete [] niti;

    Thread::sleep(5); // ~10 seconds

    cout << "napravljeno: " << cons << " obrisano: " << dest << endl;
    cout << "End of usermain" << endl;
}
