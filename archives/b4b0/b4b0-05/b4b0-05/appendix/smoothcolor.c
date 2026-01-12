/*
 * smoothcolor v1.0 by baldor & giemor
 * quick hack - done in about 30mins during a meeting on April 9th 98
 *
 * compile with gcc -O -o smoothcolor smoothcolor.c
 * (DON'T FORGET THE -O : the program doesnt compile without it !!!)
 *
 * you have to run this program as root because it accesses the ports
 * directly.
 *
 * works with Linux only !
 */

#include <stdio.h>
#include <unistd.h>
#include <asm/io.h>

void pal(int color, int r, int g, int b)
{
        outb(color, 0x3c8);
        outb(r, 0x3c9);
        outb(g, 0x3c9);
        outb(b, 0x3c9);
}

void docolor()
{
        int i;

        /* I know this is ugly but hey - it works ! */

        for (i=63; i>0; i--) {
                usleep(30000);
                pal(7,63,i,i);
        }
        for (i=0; i<63; i++) {
                usleep(30000);
                pal(7,63,i,0);
        }
        for (i=63; i>0; i--) {
                usleep(30000);
                pal(7,i,63,0);
        }
        for (i=0; i<63; i++) {
                usleep(30000);
                pal(7,0,63,i);
        }
        for (i=63; i>0; i--) {
                usleep(30000);
                pal(7,0,i,63);
        }
        for (i=0; i<63; i++) {
                usleep(30000);
                pal(7,i,0,63);
        }
        for (i=0; i<63; i++) {
                usleep(30000);
                pal(7,63,i,63);
        }
}

int main(int argc, char *argv[])
{
        int i;
        printf("\n                Smoothcolor v1.0 by baldor & giemor (1998)\n\
n");

        if( (getuid()!=0) && (geteuid()!=0) ) {
                printf("ERROR: you need to be root to run this program\n");
                exit(1);
        }


        if( fork() != 0 ) return(0); /* go into background */

        /* Get IO-Permissions for port */
        ioperm(0x3c8,3,1);
        ioperm(0x3c9,3,1);

        while(1) {
                docolor();
        }

        /* Drop the permissions */
        ioperm(0x3c8,3,0);
        ioperm(0x3c9,3,0);
}

