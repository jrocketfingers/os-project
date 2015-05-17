#include <iostream.h>
#include <thread.h>
#include <kernel.h>

int userMain(int argc, char* argv[]);


int main(int argc, char* argv[]) {
    Kernel::init();

    cout << "Back from kernel init in the main(); going into usermain." << endl;

    userMain(argc, argv);

    return 0;
}
