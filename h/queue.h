#ifndef __H_QUEUE__
#define __H_QUEUE__

#include <api/types.h>

template <class T>
class Queue {
public:
    Queue() {
        head = 0;
        tail = 0;
    }

    void put(T val) {
        Elem* newElem = new Elem(val);

        if(!head)
            head = newElem;
        else
            tail->next = newElem;

        tail = newElem;
    }

    T get() {
        Elem* ret = head;
        head = head->next;

        return ret->val;
    }

    bool empty() {
        return head == 0;
    }

private:
    struct Elem {
        Elem(T val) { this->val = val; this->next = 0; }
        Elem *next;
        T val;
    };

    Elem *head, *tail;
};

#endif
