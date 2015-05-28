SRC = src\*.cpp
APISRC = src\api\*.cpp
PTESTSRC = test\*.cpp
MYTESTSRC = custom\*.cpp

TESTINCLUDES = test\*.h
INCLUDES = h\*.h
APIINCL = h\api\*.h

LIBS = lib\*.lib
INCLUDEPATH = C:\bc31\include;D:\h\;D:\test\;
LIBSPATH = C:\bc31\lib;D:\lib\;

APIOBJ = out\api\*.obj
OUTOBJ = out\*.obj

all:	os api ptest link

link:	$(TESTSRC) os api
	bcc.exe -mh -v -nOUT -eos.exe -I$(INCLUDEPATH) -L$(LIBSPATH) $(LIBS) $(APIOBJ) $(OUTOBJ)

os:		$(SRC) $(INCLUDES)
	bcc.exe -mh -v -nOUT -c -I$(INCLUDEPATH) -L$(LIBSPATH) $(SRC)

api:	$(APISRC) $(APIINCLUDES)
	bcc.exe -mh -v -nOUT/API -c -I$(INCLUDEPATH) -L$(LIBSPATH) $(APISRC)

ptest:	$(PTESTSRC) $(TESTINCLUDES)
	bcc.exe -mh -v -nOUT -c -I$(INCLUDEPATH) -L$(LIBSPATH) $(PTESTSRC)

mytest:	$(MYTESTSRC)
	bcc.exe -mh -v -nOUT -c -I$(INCLUDEPATH) -L$(LIBSPATH) $(MYTESTSRC)

clean:
	-del out\*.obj
	-del out\*.exe
	-del out\*.asm