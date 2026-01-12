all:		SABBIAex

SABBIAex:	sabbia_serpent.o SABBIAex.c
		gcc SABBIAex.c sabbia_serpent.o -lmcrypt -g3 -o SABBIAex

sabbia_serpent.o:	sabbia_serpent.c
		gcc -c sabbia_serpent.c

clean:
		rm -rf SABBIAex sabbia_serpent.o
