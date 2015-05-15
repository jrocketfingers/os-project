CFLAGS = +cmpsw
experiment.exe: kernel.obj thread.obj main.obj
	BCC $(CFLAGS) src/experi~1.cpp output/kernel.obj output/thread.obj output/main.obj APPLICAT.LIB
	#TLINK output/experi~1.obj output/kernel.obj

experiment.asm: kernel.obj
	BCC $(CFLAGS) -S src/experi~1.cpp src/kernel.cpp

main.obj: src/main.cpp
	BCC $(CFLAGS) -c $**

thread.obj: src/thread.cpp
	BCC $(CFLAGS) -c $**

kernel.obj: src/kernel.cpp
	BCC $(CFLAGS) -c $**
