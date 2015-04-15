CFLAGS = -mh -IC:\BC31\INCLUDE;D:\H -LC:\BC31\LIB;D:\LIB -nOUTPUT
experiment.exe: src/experi~1.cpp
	BCC $(CFLAGS) src/experi~1.cpp

main.obj: src/main.cpp
	BCC $(CFLAGS) $*.cpp

thread.obj: src/thread.cpp
	BCC $(CFLAGS) $**.cpp

os.obj: src/os.cpp
	BCC $(CFLAGS) $*.cpp
