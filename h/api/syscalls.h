#ifndef __H_API_SYSCALLS__
#define __H_API_SYSCALLS__

#include <api/types.h>

int sys_newthread(void *_this, void *_run, StackSize stackSize, Time timeSlice);
void sys_startthread(unsigned tid);
void sys_endthread(unsigned tid);
void sys_waittocomplete(unsigned tid);
void sys_sleep(Time time);

int sys_newsem(int init);
void sys_deletesem(int sid);
void sys_wait(int sid);
void sys_signal(int sid);
int sys_semval(int sid);

#endif
