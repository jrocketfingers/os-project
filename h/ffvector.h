#ifndef __H_FFVECTOR__
#define __H_FFVECTOR__

#include <vector.h>

template <class T>
class ffvector : public vector<T> {
public:
    ffvector(ffvector& rhs) : ids(rhs.capacity), vector<T>(rhs) { }

    ffvector(unsigned int capacity) : ids(capacity), vector<T>(capacity) { }

    unsigned int append(T datum) {
        if(ids.length() > 0) {
            int pos = ids.pop_back();
            insert(datum, pos);
            return pos;
        } else {
            return vector<T>::append(datum);
        }
    }

    T remove(unsigned int pos) {
        T ret = vector<T>::remove(pos);
        ids.append(pos);
        return ret;
    }

private:
    /* available ids, in fragmented space */
    vector<int> ids;
}

#endif