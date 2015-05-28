#ifndef __H_SYSCALLS__
#define __H_SYSCALLS__

//enum {
    //SYS_dispatch            =101,
    //SYS_newthread           =102,
    //SYS_startthread         =103,
    //SYS_endthread           =104,
    //SYS_deletethread        =105,
    //SYS_waittocomplete      =106,
    //SYS_sleep               =107,

    //SYS_newsem              =201,
    //SYS_deletesem           =202,
    //SYS_waitsem             =203,
    //SYS_sigsem              =204,
    //SYS_semval              =205,

    //SYS_newevent            =301,
    //SYS_sigev               =302,
    //SYS_waitev              =303,
    //SYS_deleteevent         =304
//};

enum {
    SYS_dispatch            =1,
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

const char callNames[][50] = {
    "CALL NAME ERROR (call was 0)",
    "Dispatch",
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
