KERNEL=kernel.exe
ASSEMBLY=kernel.asm
CXX=bcc
CFLAGS=+cmpsw

#OBJS:= $(OBJS) output/kernel.o output/thread.o output/main.o output/experiment.o
SRCS=src/kernel.cpp src/thread.cpp src/main.cpp src/kthread.cpp src/experi~1.cpp
OBJS=out/kernel.obj out/thread.obj out/main.obj out/kthread.obj out/experi~1.obj

all: $(KERNEL)

$(KERNEL): $(OBJS)
	$(CXX) $(CFLAGS) $(OBJS)

out/kernel.obj:
	$(CXX) $(CFLAGS) -c src/kernel.cpp

out/thread.obj:
	$(CXX) $(CFLAGS) -c src/thread.cpp

out/main.obj:
	$(CXX) $(CFLAGS) -c src/main.cpp

out/kthread.obj:
	$(CXX) $(CFLAGS) -c src/kthread.cpp

out/experi~1.obj:
	$(CXX) $(CFLAGS) -c src/experi~1.cpp
