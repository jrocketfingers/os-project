#include <iostream.h>
#include <thread.h>
#include <kernel.h>

int userMain(int argc, char* argv[]);


class UserMainThread : public Thread {
public:
    UserMainThread(int argc, char* argv[]) {
        this->argc = argc;
        this->argv = argv;
    }

    ~UserMainThread() {
        waitToComplete();
    }

    void run() {
        userMain(argc, argv);
    }

private:
    int argc;
    char **argv;
};

int main(int argc, char* argv[]) {
    Kernel::init();

    cout << "Pravi se userMain thread." << endl;
    UserMainThread* u = new UserMainThread(argc, argv);

    cout << "Startuje se userMain thread." << endl;
    u->start();

    return 0;
}
