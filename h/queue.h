#ifndef __H_QUEUE__
#define __H_QUEUE__

#include <kernel.h>
#include <debug.h>
#include <api_type.h>

template <class T>
class Queue {
public:
    Queue() {
        head = 0;
        tail = 0;

        elements = 0;
    }

    void put(T val) {
        Elem* newElem = new Elem(val);

        if(!head)
            head = newElem;
        else
            tail->next = newElem;

        tail = newElem;

        elements++;
    }

    T get() {
        if(elements <= 0) {
            #ifdef DEBUG__QUEUE
            cout << "Trying to get from an empty queue! Could also be that the elements variable (" << elements << ") corrupted." << endl << flush;
            #endif
            Kernel::emergency_halt();
        }

        Elem* ret = head;
        head = head->next;

        elements--;

        return ret->val;
    }

    bool empty() {
        if(elements < 0) {
            #ifndef DEBUG__QUEUE
            cout << "Elements value is negative. Something smells here." << endl << flush;
            #endif
            Kernel::emergency_halt();
        }

        #ifdef DEBUG__QUEUE
        cout << "Queue elements: " << elements << endl << flush;
        #endif

        return elements == 0;
        //return head == 0;
    }

private:
    struct Elem {
        Elem(T val) { this->val = val; this->next = 0; }
        Elem *next;
        T val;
    };

    Elem *head, *tail;
    int elements;
};

#endif
