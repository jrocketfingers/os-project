#ifndef __H_SYSCALLS__
#define __H_SYSCALLS__

enum {
    SYS_dispatch            =101,
    SYS_newthread           =102,
    SYS_startthread         =103,
    SYS_endthread           =104,
    SYS_deletethread        =105,
    SYS_waittocomplete      =106,
    SYS_sleep               =107,

    SYS_newsem              =201,
    SYS_deletesem           =202,
    SYS_waitsem             =203,
    SYS_sigsem              =204,
    SYS_semval              =205,

    SYS_newevent            =301,
    SYS_sigev               =302,
    SYS_waitev              =303,
    SYS_deleteevent         =304
};

#endif
