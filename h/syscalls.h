#ifndef __H_SYSCALLS__
#define __H_SYSCALLS__

enum {
    SYS_dispatch            =101,
    SYS_newthread           =102,
    SYS_startthread         =103,
    SYS_endthread           =104,
    SYS_wait                =201,
    SYS_signal              =202
};

#endif
