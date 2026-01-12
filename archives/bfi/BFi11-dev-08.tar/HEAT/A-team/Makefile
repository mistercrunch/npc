SOURCE=A-team.c
BINARY=A-team
MANPAGE=A-team.8
CC=cc
CC_OPTIONS=-Wall
CC_LIBS=`libnet-config --defines --libs` -lpcap -pthread
INSTALLBIN=install
INSTALLDIR=/usr/local/sbin/
MANDIR=/usr/local/man/man8/

all: A-team

A-team:
	$(CC) $(CC_OPTIONS) -o $(BINARY) $(SOURCE) $(CC_LIBS)

install: A-team-install man-install

A-team-install:
	$(INSTALLBIN) -m 755 $(BINARY) $(INSTALLDIR)

man-install:
	$(INSTALLBIN) -m 644 $(MANPAGE) $(MANDIR)


clean:
	rm -rf $(BINARY)
