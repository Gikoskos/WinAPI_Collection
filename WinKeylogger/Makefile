CC = gcc
CFLAGS = -static-libgcc -std=c99 -ggdb -Wall
LINKER = -lntdll
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))
EXE = WinKeylogger.exe


$(EXE): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LINKER)

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	@del -f *.o $(EXE)
