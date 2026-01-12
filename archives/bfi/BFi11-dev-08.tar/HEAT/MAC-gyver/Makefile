SOURCE=MAC-gyver.c
BINARY=MAC-gyver
CONFFILE=ethernet_hosts.conf
MANPAGE=MAC-gyver.8
CC=cc
CC_OPTIONS=-Wall
CC_LIBS=`libnet-config --defines --libs` -lpcap
INSTALLBIN=install
INSTALLDIR=/usr/local/sbin/
CONFDIR=/etc/
MANDIR=/usr/local/man/man8/

all: MAC-gyver

MAC-gyver:
	$(CC) $(CC_OPTIONS) -o $(BINARY) $(SOURCE) $(CC_LIBS)

install: MAC-gyver-install man-install

MAC-gyver-install:
	$(INSTALLBIN) -m 755 $(BINARY) $(INSTALLDIR)
	$(INSTALLBIN) -m 600 $(CONFFILE) $(CONFDIR)

man-install:
	$(INSTALLBIN) -m 644 $(MANPAGE) $(MANDIR)

clean:
	rm -rf $(BINARY)
