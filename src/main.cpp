#include <thread.h>
#include <kernel.h>

#include <debug.h>

int userMain(int argc, char* argv[]);


int main(int argc, char* argv[]) {
    Kernel::init();

#ifdef DEBUG
    cout << "Back from kernel init in the main(); going into usermain." << endl;
#endif

    userMain(argc, argv);

    return 0;
}
