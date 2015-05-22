#ifndef __H_API_SYSCALLS__
#define __H_API_SYSCALLS__

#include <api/types.h>

int sys_newthread(void *_this, void *_run, StackSize stackSize, Time timeSlice);
void sys_startthread(unsigned tid);
void sys_endthread(unsigned tid);

#endif
