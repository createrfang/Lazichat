CC = gcc
INCLUDE = -I.
TARGET = Ser Cli
LIBS = 

all: $(TARGET)

Cli: client.o Mesg.o
	$(CC)  -oCli client.o Mesg.o

client.o: Mesg.h server/serverconf.h client/client.c
	$(CC) -c -o client.o client/client.c $(LIBS)

Ser: Ser.o Mesg.o 
	$(CC)  -oSer Ser.o Mesg.o
	
Ser.o:	server/serverconf.h Mesg.h server/Ser.c
	$(CC) -c -oSer.o server/Ser.c $(LIBS)

Mesg.o: Mesg.h Mesg.c
	$(CC) -c -oMesg.o Mesg.c

.PHONY: clean
clean: 
	-rm $(TARGET) *.o

