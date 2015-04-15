#ifndef __OS_H__
#define __OS_H__

#define lock asm cli
#define unlock asm sti



class PCB {                      // Kernel's implementation of a user's thread
public:
    word sp;
    word ss;
    word bp;
    word cs;
    word pc;
    bool done;
    int timeSlice;
};



class OS {
public:
    static void waitFor(int tid);
    static void sleep(Time time);
    static void dispatch();
    static void yield();
    static void schedule(int tid);
    static void stop();
    static int enlistPCB(PCB *newPCB);
private:
    static PCB *running;
    static struct id_list *available_id;

    static int last_id;

    struct id_list {
        int id;
        id_list *next;
        id_list(int id) { this->id = id; next = NULL; }
    };
};

#endif