/*
 * saled.c - Simple and Lame ELF Disassembler
 *                                    by twiz - twiz@email.it
 *
 * Questo codice e' parte dell' articolo "Linux Virtual Memory Tripping"
 *
 * Compile with :
 *   gcc -o saled -ldisasm -I./ saled.c parse.s
 *
 * Assuming that you've in libdisasm.so in /usr/lib and libdis.h (and all files
 * included in libdis.h) in compiling directory
 * If not just use -I gcc tag to point to dir of libdisasm includes or modify
 * CPATH variable.
 *
 * The official site of libdisasm is :
 *   http://bastard.sourceforge.net
 *
 * In the dictracy package there's a patched/modified version of libdisasm
 * that, at the moment of release, isn't online yet.
 * Many thanks to : _mammon
 */

#include <stdio.h>
#include <stdlib.h>
#include <linux/limits.h>
#include <elf.h>
#include <libdis.h>
#include "dictracy.h" 

extern int parse_rwvirtaddr(int, char *, int, unsigned long, int);
extern int parse_getvirtaddr(struct mem_addr *, int);
void mcheck(void *maddr);
char *elf_get_type(int type);
char *elf_get_machine(int machine);

main(int argc, char **argv)
{
 Elf32_Ehdr * elfhdr;
 struct mem_addr *mem;
 unsigned char *img;
 int pid, ret, distance, n, size, i = 0;
 unsigned long codeaddr, startaddr, endaddr;
 struct instr curr_inst;

if ( argc != 2 )
 {
  fprintf(stderr, "Usage : %s pid\n", argv[0]);
  exit(1);
 }

pid = atoi(argv[1]);
mem = (struct mem_addr *)malloc(sizeof(struct mem_addr));
mcheck(mem);
ret = parse_getvirtaddr(mem, pid);

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

endaddr = mem->endcode;
codeaddr = mem->startcode;
free(mem);

img = (char *)malloc(52);
mcheck(img);
printf("--- PID examined: %d\n", pid);
printf("\nELF Header - dumping at address : %p\n", codeaddr);
ret = parse_rwvirtaddr(pid, img, 52, codeaddr, 0);

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

 elfhdr = (void *)img;
 if ( elfhdr->e_ident[0] != ELFMAG0 ||
      elfhdr->e_ident[1] != ELFMAG1 || 
      elfhdr->e_ident[2] != ELFMAG2 || 
      elfhdr->e_ident[3] != ELFMAG3 )
  {
   fprintf(stderr, "Error in ELF magic\n");
   exit(1);
  }  

 printf("ELF's identification first 4 bytes 0x%2x %c %c %c\n", elfhdr->e_ident[0], elfhdr->e_ident[1], elfhdr->e_ident[2], elfhdr->e_ident[3]);
 
 if ( elfhdr->e_ident[EI_CLASS] == 1 )
  printf("Class : ELFCLASS32 - 32 bit objects\n");
 if ( elfhdr->e_ident[EI_CLASS] == 2 )
  printf("Class : ELFCLASS32 - 64 bit objects\n");

 if ( elfhdr->e_ident[EI_DATA] == 1 )
  printf("Data Encoding : Least Significant Bit\n");
 if ( elfhdr->e_ident[EI_DATA] == 2 )
  printf("Data Encoding : Most Significant Bit\n");

 printf("Type : %d - %s\n", elfhdr->e_type, elf_get_type(elfhdr->e_type));
 printf("Machine : %d - %s\n",elfhdr->e_machine,                                         elf_get_machine(elfhdr->e_machine)); 

 if ( elfhdr->e_version == 1 )
  printf("Version : EV_CURRENT\n");
 printf("Entry : %p - virtual address to which the system first transfers control\n", elfhdr->e_entry);
 startaddr = elfhdr->e_entry;
 printf("PHT offset : %d\n", elfhdr->e_phoff);
 printf("SHT offset : %d\n", elfhdr->e_shoff);
 printf("ELF's header size : %d\n", elfhdr->e_ehsize);
 printf("Size of one entry in PHT : %d\n", elfhdr->e_phentsize);
 printf("Number of PHT's entry : %d\n", elfhdr->e_phnum);
 free(img);
 
 printf("\nStarting Debugging\n\n");
 distance = endaddr - startaddr;
 printf("Starting at address : %p \nEnding at address : %p\nOffset : %d\n\n",        startaddr, endaddr, distance); 
 img = (char *)malloc(distance); 
 ret = parse_rwvirtaddr(pid, img, distance, startaddr, 0);

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

 disassemble_init(IGNORE_NULLS, ATT_SYNTAX);
 while (i < distance)
  {
   memset(&curr_inst, 0, sizeof(struct code));
   printf("%p   \t", startaddr + i);
   size = disassemble_address(img + i, &curr_inst);
     if (size)
       {
        for ( n = 0; n < 12; n++)
          {
          if ( n < size )
             printf("%02X ", img[i + n]);
          else 
             printf("   ");
          }
        printf("%s", curr_inst.mnemonic);
          if (curr_inst.src[0] != 0)
             printf("\t%s,", curr_inst.src);
          if (curr_inst.aux[0] != 0)
             printf(", %s", curr_inst.aux);
          if (curr_inst.dest[0] != 0) 
             printf(" %s", curr_inst.dest);
        printf("\n");
        i += size;
       } 
     else 
       {
        printf("invalid opcode %02X\n", img[i]);
        i++;
        }
   }
 disassemble_cleanup();
 exit(0);
}


void mcheck(void *maddr)
{
 if (maddr == NULL)
  {
   fprintf(stderr, "Memory allocation error - malloc()\n");
   exit(1);
  }
}

char *elf_get_type(int type)
{ 
 switch(type) 
 { 
  case 0: return "No file type";
  case 1: return "Relocatable file";
  case 2: return "Executable file";
  case 3: return "Shared Object file";
  case 4: return "Core file";
  default: 
          if ( type >= 0xff00 && type <= 0xffff ) 
            return "Processor Specific Type";
          return "Reserved Value - Unknown";
 }
}

char *elf_get_machine(int machine)
{
 switch(machine)
 {
  case 0: return "No machine";
  case 1: return "AT&T WE 32100";
  case 2: return "SPARC";
  case 3: return "Intel 80386";
  case 4: return "Motorola 68000";
  case 5: return "Motorola 88000";
  case 7: return "Intel 80860"; 
  case 8: return "MIPS RS3000"; 
  default: 
          return "Reserved Value - Unknown";
 }
}
