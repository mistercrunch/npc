# usk Makefile
# 270202  ulix <ulix@gmx.it>

# gcc stuff
CC = gcc

# [(en),(dis)]able debugging printouts
DEBUG = y

# base kernel dir
BASE = /usr/src/linux

# host kernel version
VERSION = $(shell uname -r)

# sure kernel dir ;) 
ifndef KDIR
KDIR = $(shell if [ -d $(BASE)-$(VERSION) ]; then echo $(BASE)-$(VERSION); \
	else if [ -d $(BASE) ]; then echo $(BASE); \
	fi ; fi)
endif

# include kernel dir
ifndef INCLUDEDIR
INCLUDEDIR = $(KDIR)/include
endif

# optimizations for my box... change it, please...
DFLAGS = -O2 -fomit-frame-pointer -fno-strict-aliasing -fno-common -pipe \
		-mpreferred-stack-boundary=2 -march=k6

ifeq ($(DEBUG), y)
  DFLAGS += -g -DDEBUG
endif

CFLAGS = -Wall -Wstrict-prototypes $(DFLAGS) 
CFLAGS += -I$(INCLUDEDIR)

OBJS = usk.o

all: $(OBJS)

clean:
	rm -f *.o *~ core
