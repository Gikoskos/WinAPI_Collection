CC = gcc

all: MsgBoxError.o tests.exe

tests.exe: tests.c MsgBoxError.o
	$(CC) -o $@ $^

MsgBoxError.o: MsgBoxError.c
	$(CC) -o $@ -c $^

.PHONY: clean
clean:
	@del -f *.o *.lib
