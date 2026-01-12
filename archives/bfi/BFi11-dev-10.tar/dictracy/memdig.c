/*
 * Memdig.c
 *            by -  twiz   - twiz@email.it
 *                  thefly - thefly@acaro.org
 *
 * This is the userspace interface to dictracy module and will let you :
 *   - get virtual addresses of a process (code start, data start ...)
 *   - read and dump from a virtual address
 *   - write to a particular address
 *   - search for a string in an arbitrary memory area
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "dictracy.h"

#define CHARINLINE 22

extern int parse_rwvirtaddr(int, char *, int, unsigned long, int);
extern int parse_getvirtaddr(struct mem_addr *, int);
int traversemem(unsigned long, int, char *, int, int);
void usage(char *name);
void mcheck(char *maddr);

main(int argc, char **argv) 
{ 
 char *buffer, *temp;
 struct mem_addr addresses;
 int pid, fd, ret, size, i, line = 0;
 unsigned long addr; 

 if ( argc != 3 ) 
    usage(argv[0]);

/* Just an example... i'm too lazy to use getopt :)) */ 

 if ( strcmp(argv[1], "get") == 0) 
  { 
   pid = atoi(argv[2]);
   printf("PID examined: %d\n", pid);
   ret = parse_getvirtaddr(&addresses, pid);

    if ( ret == 1 )
     {
       fprintf(stderr, "Error in triggering syscall : is the pid correct ?\n");
       exit(1);
     }

    if ( ret == -1 )
     {
       fprintf(stderr, "Error in retrieving page struct : out of memory\n");
       exit(1);
     }

   printf("RESULTS: \n");
   printf("\tcode_start: \t%p\n", addresses.startcode);
   printf("\tcode_end: \t%p\n", addresses.endcode);
   printf("\tdata_start: \t%p\n", addresses.startdata);
   printf("\tdata_end: \t%p\n", addresses.enddata);
   printf("\theap_start: \t%p\n", addresses.startbrk);
   printf("\tstack_start: \t%p\n", addresses.startstack);
   exit(0);

  }
         
 if ( strcmp(argv[1], "read") == 0 )
  {
   pid = atoi(argv[2]); 
   printf("Insert virtual address to start reading from : ");
   scanf("%x", &addr); 
   printf("Insert size of memory you want to read : ");
   scanf("%d", &size); 
  
   buffer = malloc(size);
   mcheck(buffer);
   printf("PID examined: %d\n", pid);
   printf("Reading at virtual address %p %d bytes of memory\n", addr, size);
   ret = parse_rwvirtaddr(pid, buffer, size, addr, 0);
 
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
 
printf("Only valid alphanumeric chars would be printed, where not possible a \".\" will be printed\n");
printf("READ : \n");

for ( i = 0; i < size; i++ )
  {
   if ( buffer[i] == 0xa ) 
    {
     printf("\\n");
     continue;
    }
   if ( buffer[i] == '\0' )
    {
     printf("\\0");
     continue;
    }
   if ( buffer[i] < 0x20 || buffer[i] > 0x7e )
    {
     printf(".");
     continue;
    }
  printf("%c", buffer[i]);
 }
   printf("\n");
   exit(0);
 } 

if ( strcmp(argv[1], "dump") == 0 )  
 { 
   pid = atoi(argv[2]);
   printf("Insert virtual address to start reading from : ");
   scanf("%x", &addr);
   printf("Insert size of memory you want to dump : ");
   scanf("%d", &size);

   buffer = (char *)malloc(size);
   mcheck(buffer);
   printf("PID examined: %d\n", pid);
   printf("Dumping %d bytes of memory starting at virtual address : %p\n", size,           addr);
   ret = parse_rwvirtaddr(pid, buffer, size, addr, 0);

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

  for (i = 0; i < size; i++)
   {
    if ( line == CHARINLINE ) 
      { 
       printf("%02x\n", (unsigned char)buffer[i]);
       line = 0;
       continue;
      }
    printf("%02x ", (unsigned char)buffer[i]);
    line++;
    } 
  printf("\nEnd of memory dump\n");
  exit(0);
} 

if ( strcmp(argv[1], "write") == 0 )
 {
   pid = atoi(argv[2]);
   printf("Insert virtual address to start writing to : ");
   scanf("%x", &addr);
   printf("Insert size of buffer you want to write : ");
   scanf("%d", &size);
   temp = (char *)malloc(size+1);
   mcheck(temp);
   printf("Insert buffer : ");
   scanf("%s", temp);   
   buffer = malloc(size);
   mcheck(buffer);
   strncpy(buffer, temp, size);
   ret = parse_rwvirtaddr(pid, buffer, size, addr, 1); 

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

if ( strcmp(argv[1], "src") == 0)
 {
   pid = atoi(argv[2]);
   printf("Insert virtual address to start searching from : ");
   scanf("%x", &addr);
   printf("Insert size of memory area you want to scan : ");
   scanf("%d", &i);
   printf("Insert size of buffer to search : ");
   scanf("%d", &size);
   temp = (char *)malloc(size+1);
   mcheck(temp);
   printf("Insert buffer: ");
   scanf("%s", temp);
   buffer = (char *)malloc(size);
   mcheck(buffer);
   strncpy(buffer, temp, size);
   ret = traversemem(addr, i, buffer, size, pid);

    if ( ret == -1 )
     {
      fprintf(stderr, "Buffer not found in memory area scanned\n");
      exit(1);
     }

  printf("Found buffer at offset : %d - virtual address : %p\n", ret, addr+ret);
  exit(0);
}
 
 usage(argv[0]);   
}


void usage(char *name)
 {
   fprintf(stderr, "Usage: \n");
   fprintf(stderr, "\t%s get pid  --  get virtual addresses from pid\n", name);
   fprintf(stderr, "\t%s read pid --  read a part of memory (char)\n", name);
   fprintf(stderr, "\t%s dump pid --  dump the hexcode of a part of memory\n",             name); 
   fprintf(stderr, "\t%s write pid -  write on a part of memory\n", name); 
   fprintf(stderr, "\t%s src pid  --  search a particolar memory area\n\n",                          name);
   exit(1);
 }

int traversemem(unsigned long addr,
                int size,
                char *buffer, 
                int bufflen,
                int pid)
{
 char *base, *found;
 int off, ret;

 base = (char *)malloc(size);
 mcheck(base);
 ret = parse_rwvirtaddr(pid, base, size, addr, 0);

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
   
  found = (char *)memmem(base, size, buffer, bufflen);
    if ( found == NULL ) 
      return -1;
 
  off = (unsigned long)found - (unsigned long)base;
  return off;

}

void mcheck(char *maddr)
{ 
 if (maddr == NULL) 
  {
   fprintf(stderr, "Memory allocation error - malloc()\n"); 
   exit(1);
  }
}         
