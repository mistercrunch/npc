# ops Makefile
# 070302  ulix <ulix@gmx.it>

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

# utils dlhook dir
ifndef UTILSDIR
UTILSDIR = ../utils
endif

# base System.map location
ifndef SMAP
SMAP = /boot/System.map
endif

# optimizations for my box... change it, please...
DFLAGS = -O2 -fomit-frame-pointer -fno-strict-aliasing -fno-common -pipe \
		-mpreferred-stack-boundary=2 -march=k6

ifeq ($(DEBUG), y)
  DFLAGS += -g -DDEBUG
endif

# this is the location of System.map
MAP = $(shell if [ -e $(SMAP)-$(VERSION) ]; then echo \"$(SMAP)-$(VERSION)\"; \
        else if [ -e $(SMAP)-$(VERSION) ]; then echo \"$(SMAP)-$(VERSION)\"; \
        fi ; fi)

# this is the packet_ops address in System.map
OPS = $(shell cat $(UTILSDIR)/ops)

CFLAGS = -Wall -Wstrict-prototypes $(DFLAGS) 
CFLAGS += -I$(INCLUDEDIR) -DOPS=$(OPS)

OBJS = ops.o 

all:	addr $(OBJS)
	@rm -rf $(UTILSDIR)/ops

addr:
	@$(CC) -DMAP=\"$(MAP)\" $(UTILSDIR)/smap.c -o $(UTILSDIR)/smap
	@$(UTILSDIR)/smap packet_ops >> $(UTILSDIR)/ops	

clean:
	rm -f *.o *~ core $(UTILSDIR)/smap $(UTILSDIR)/ops
