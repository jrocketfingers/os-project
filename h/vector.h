#ifndef __H_VECTOR__
#define __H_VECTOR__

#include <assert.h>

template <class T>
class vector {
public:
    vector(vector &rhs) {
        this->capacity = rhs.capacity;
        this->free = rhs.free;
        this->used = rhs.used;

        this->data = new T[this->capacity];

        for(unsigned int i = 0; i < this->capacity; i++)
            this->data[i] = rhs.data[i];
    }

    vector(unsigned int capacity) {
        this->capacity = capacity;
        this->free = 0;
        this->used = 0;

        this->data = new T[this->capacity];
    }

    ~vector() {
        delete[] this->data;
    }

    unsigned int append(T datum) {
        if(free >= capacity)
            extend(capacity * 2);

        this->data[free++] = datum;
        this->used++;

        return free - 1;
    }

    /* Shaky foundations: free index is being placed one step after the
     * 'farthest datum inserted' index. This prevents overwriting inserted data,
     * but causes more frequent resizes. */
    void insert(T datum, unsigned int pos) {
        if(pos >= capacity) {
            extend(capacity * 2);
        }

        if(pos > free)
            free = pos + 1;

        this->data[pos] = datum;
        this->used++;
    }

    T remove(unsigned int pos) {
        assert(pos < free);
        this->used--;
        return this->data[pos];
    }

    T pop_back() {
        this->used--;
        this->free--;
        return this->data[free];
    }

    T& operator[](unsigned int pos) {
        return this->data[pos];
    }

    unsigned int usage() { return this->used; }
    unsigned int length() { return this->free; }
    unsigned int actual_size() { return this->capacity; }

protected:
    T *data;

    void extend(unsigned int new_capacity) {
        T *olddata = this->data;
        this->data = new T[capacity * 2];

        /* still uses capacity value before doubling */
        for(int i = 0; i < capacity; i++)
            this->data[i] = olddata[i];

        /* marks the capacity as doubled, must happen last */
        capacity = capacity * 2;
    }

    unsigned int capacity;
    unsigned int free;
    unsigned int used;
}

#endif
