C=gcc
L=ld
CFLAGS=-fomit-frame-pointer -O2 -I/usr/src/linux/include

all: dictracy memdig fakeflow saled

dictracy:
	$(C) -c dictracy.c $(CFLAGS)
memdig:
	$(C) -o memdig memdig.c parse.s
saled:
	echo "Have you correctly set libdisasm, don't you ? ;)"
	$(C) -o saled -ldisasm -I./ saled.c parse.s
fakeflow:
	$(C) -o fakeflow fakeflow.c parse.s
