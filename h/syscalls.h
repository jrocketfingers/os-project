#ifndef __H_SYSCALLS__
#define __H_SYSCALLS__

enum {
    SYS_dispatch            =1,
    SYS_preempt             ,
    SYS_newthread           ,
    SYS_startthread         ,
    SYS_endthread           ,
    SYS_deletethread        ,
    SYS_waittocomplete      ,
    SYS_sleep               ,

    SYS_newsem              ,
    SYS_deletesem           ,
    SYS_waitsem             ,
    SYS_sigsem              ,
    SYS_semval              ,

    SYS_newevent            ,
    SYS_sigev               ,
    SYS_waitev              ,
    SYS_deleteevent
};

#ifdef DEBUG__VERBOSE
const char callNames[][50] = {
    "CALL NAME ERROR (call was 0)",
    "Dispatch",
    "Timer preemption",
    "New thread",
    "Start thread",
    "End thread",
    "Delete thread",
    "Join",
    "Sleep",
    "New semaphore",
    "Delete semaphore",
    "Wait semaphore",
    "Signal semaphore",
    "Get semaphore value",
    "New event",
    "Signal event",
    "Wait event",
    "Delete event"
};
#endif

#endif
