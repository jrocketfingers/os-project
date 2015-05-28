SRC = src\*.cpp
APISRC = src\api\*.cpp
INCLUDES = h\*.h
APIINCL = h\api\*.h
LIBS = lib\*.lib
INCLUDEPATH = C:\bc31\include;D:\h\;
LIBSPATH = C:\bc31\lib;D:\lib\;
TARGET = os.exe
API = out/api/*.obj

$(TARGET):	api $(SRC) $(INCLUDES)
	bcc.exe -mh -v -nOUT -e$(TARGET) -I$(INCLUDEPATH) -L$(LIBSPATH) $(SRC) $(LIBS) out/api/syscalls.obj

api: $(APISRC) $(APIINCLUDES)
	bcc.exe -mh -v -nOUT/API -c -I$(INCLUDEPATH) -L$(LIBSPATH) $(APISRC)

all:	$(TARGET)
clean:
	-del *.obj
	-del $(TARGET)
	-del *.asm