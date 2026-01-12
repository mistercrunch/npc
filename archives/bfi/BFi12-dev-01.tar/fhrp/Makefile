#Makefile for modul.o
CC=gcc
CFLAGS= -c -I/usr/src/linux/include
OUTMOD= modul.o

all:
	$(CC) $(CFLAGS) cr3-main.c
	$(CC) $(CFLAGS) cr3-func.c
	$(CC) $(CFLAGS) cr3-timer.c
	ld -o fhrp.o cr3-main.o cr3-func.o cr3-timer.o -i 
