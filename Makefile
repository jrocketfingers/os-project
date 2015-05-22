KERNEL=kernel.exe
ASSEMBLY=kernel.asm
CXX=bcc
CFLAGS=+cmpsw

#OBJS:= $(OBJS) output/kernel.o output/thread.o output/main.o output/experiment.o
SRCS=src/kernel.cpp src/thread.cpp src/main.cpp src/kthread.cpp src/experi~1.cpp src/syscalls.cpp src/pcb.cpp src/api/syscalls.cpp
OBJS=out/kernel.obj out/thread.obj out/main.obj out/kthread.obj out/experi~1.obj out/syscalls.obj out/pcb.obj out/api/syscalls.obj

all: $(KERNEL)

$(KERNEL): $(OBJS)
	$(CXX) $(CFLAGS) lib/applicat.lib @&&!
$(OBJS)
!

out/kernel.obj:
	$(CXX) $(CFLAGS) -c src/kernel.cpp

out/thread.obj:
	$(CXX) $(CFLAGS) -c src/thread.cpp

out/main.obj:
	$(CXX) $(CFLAGS) -c src/main.cpp

out/kthread.obj:
	$(CXX) $(CFLAGS) -c src/kthread.cpp

out/syscalls.obj:
	$(CXX) $(CFLAGS) -c src/syscalls.cpp

out/api/syscalls.obj:
	$(CXX) $(CFLAGS) -c src/api/syscalls.cpp

out/pcb.obj:
	$(CXX) $(CFLAGS) -c src/pcb.cpp

out/experi~1.obj:
	$(CXX) $(CFLAGS) -c src/experi~1.cpp
