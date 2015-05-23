#ifndef __H_SYSCALLS__
#define __H_SYSCALLS__

enum {
    SYS_dispatch            =101,
    SYS_newthread           =102,
    SYS_startthread         =103,
    SYS_endthread           =104,
    SYS_deletethread        =105,
    SYS_waittocomplete      =106,

    SYS_newsem              =201,
    SYS_deletesem           =202,
    SYS_wait                =203,
    SYS_signal              =204,
    SYS_semval              =205
};

#endif
