KERNEL=kernel.exe
ASSEMBLY=kernel.asm
CXX=bcc
CFLAGS=+cmpsw -nD:\OUT
CFLAGSAPI=+cmpsw -nD:\OUT\API

#OBJS:= $(OBJS) output/kernel.o output/thread.o output/main.o output/experiment.o
SRCS=src/kernel.cpp src/thread.cpp src/semaphor.cpp src/main.cpp src/kthread.cpp\
	 src/ithread.cpp src/experi~1.cpp src/syscalls.cpp src/pcb.cpp \
	 src/kernsem.cpp src/api/syscalls.cpp

OBJS=out/kernel.obj out/thread.obj out/semaphor.obj out/main.obj out/kthread.obj\
	 out/ithread.obj out/experi~1.obj out/syscalls.obj out/pcb.obj \
	 out/kernsem.obj out/api/syscalls.obj

all: $(KERNEL)

$(KERNEL): $(OBJS)
	$(CXX) $(CFLAGS) lib/applicat.lib @&&!
$(OBJS)
!

out/kernel.obj:
	$(CXX) $(CFLAGS) -c src/kernel.cpp

out/thread.obj:
	$(CXX) $(CFLAGS) -c src/thread.cpp

out/semaphor.obj:
	$(CXX) $(CFLAGS) -c src/semaphor.cpp

out/main.obj:
	$(CXX) $(CFLAGS) -c src/main.cpp

out/kthread.obj:
	$(CXX) $(CFLAGS) -c src/kthread.cpp

out/ithread.obj:
	$(CXX) $(CFLAGS) -c src/ithread.cpp

out/kernsem.obj:
	$(CXX) $(CFLAGS) -c src/kernsem.cpp

out/syscalls.obj:
	$(CXX) $(CFLAGS) -c src/syscalls.cpp

out/api/syscalls.obj:
	$(CXX) $(CFLAGSAPI) -c src/api/syscalls.cpp

out/pcb.obj:
	$(CXX) $(CFLAGS) -c src/pcb.cpp

out/experi~1.obj:
	$(CXX) $(CFLAGS) -c src/experi~1.cpp
