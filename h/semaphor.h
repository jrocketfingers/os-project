#ifndef __H_SEMAPHORE__
#define __H_SEMAPHORE__

class Semaphore {
public:
    Semaphore (int init=1);
    virtual ~Semaphore ();
    virtual void wait ();
    virtual void signal();
    int val () const;                                  // Returns the current
                                                       // value of the semaphore
private:
    unsigned int sid;
};

#endif
