CC = gcc
CPP = -I./../ -municode
LINKER = -ladvapi32
CFLAGS = -Wall

SRCS = $(wildcard *.c)
EXES = $(patsubst %.c,%.exe,$(SRCS))

.PHONY: clean all

all: $(EXES)

%.exe: %.c
	$(CC) $(CFLAGS) $(CPP) $< -o $@ $(LINKER)

clean:
	@echo off
	@del *.o *.exe
	@echo on
