CFLAGS = -mh -IC:\BC31\INCLUDE;D:\H -LC:\BC31\LIB;D:\LIB -nOUTPUT
experiment.exe: kernel.obj thread.obj
	BCC $(CFLAGS) src/experi~1.cpp output/kernel.obj output/thread.obj APPLICAT.LIB
	#TLINK output/experi~1.obj output/kernel.obj

experiment.asm: kernel.obj
	BCC $(CFLAGS) -S src/experi~1.cpp src/kernel.cpp

main.obj: src/main.cpp
	BCC $(CFLAGS) $**

thread.obj: src/thread.cpp
	BCC $(CFLAGS) $**

kernel.obj: src/kernel.cpp src/main.cpp
	BCC $(CFLAGS) -c $**
