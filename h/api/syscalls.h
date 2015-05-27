#ifndef __H_API_SYSCALLS__
#define __H_API_SYSCALLS__

#include <api/types.h>

int sys_newthread(void *_this, void *_run, StackSize stackSize, Time timeSlice);
void sys_startthread(unsigned tid);
void sys_endthread(unsigned tid);
void sys_waittocomplete(unsigned tid);
void sys_sleep(Time time);

unsigned sys_newsem(int init);
void sys_deletesem(unsigned sid);
void sys_waitsem(unsigned sid);
void sys_sigsem(unsigned sid);
int sys_semval(unsigned sid);

unsigned sys_newev(IVTNo ivtNo);
void sys_deleteev(unsigned eid);
void sys_waitev(unsigned eid);
void sys_sigev(unsigned eid);

#endif
