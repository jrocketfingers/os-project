SRC = src\*.cpp
APISRC = src\api\*.cpp
INCLUDES = h\*.h
APIINCL = h\api\*.h
LIBS = lib\*.lib
INCLUDEPATH = C:\bc31\include;D:\h\;
LIBSPATH = C:\bc31\lib;D:\lib\;
TARGET = Projekat.exe

$(TARGET):	$(SRC) $(INCLUDES) $(APISRC) $(APIINCLUDES)
	bcc.exe -mh -v -e$(TARGET) -I$(INCLUDEPATH) -L$(LIBSPATH) $(APISRC) $(SRC) $(LIBS)
all:	$(TARGET)
clean:
	-del *.obj
	-del $(TARGET)
	-del *.asm