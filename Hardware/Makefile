CC = gcc
CPP = -I./../
CFLAGS = -Wall
LINKER = -lsetupapi

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
