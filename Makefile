CC = gcc
CFLAGS = -std=gnu99 -g -O0 
LIBNAME = capstone
COMPILE=-O3 -l

all: main ex02 ex03

ex02: ex02.c
	$(CC) $(CFLAGS) $< -o $@ 
ex03: ex03.c
	$(CC) $(CFLAGS) $< -o $@ 

main: main.c
	$(CC) $(CFLAGS) $< $(COMPILE)$(LIBNAME) -o $@

.PHONY: all clean

clean:
	rm  main ex02 ex03
