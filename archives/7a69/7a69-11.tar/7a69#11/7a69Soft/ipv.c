/*
  trycky : trycky@7a69ezine.org 
  IPV 01 
  Se puede decir y se dice que el programar casi por 100% lo cogido del ircd 
  del Hispano la version 02 trae para sacar la key de encriptacion a fuerza 
  bruta entre otras cosas para el 7a69#12 la publicare si eso .
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VERSION "ipv Version 01 . trycky : trycky@7a69ezine.org"
#define NUMNICKBASE 64
#define NUMNICKMASK 63
#define NUMNICKLOG 6
#define NUMNICKMAXCHAR 'z'

static const unsigned int convert2n[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  52,53,54,55,56,57,58,59,60,61, 0, 0, 0, 0, 0, 0,
  0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
  15,16,17,18,19,20,21,22,23,24,25,62, 0,63, 0, 0,
  0,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
  41,42,43,44,45,46,47,48,49,50,51, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static const char convert2y[] = {
  'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
  'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
  'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
  'w','x','y','z','0','1','2','3','4','5','6','7','8','9','[',']'
};

void tea(unsigned int v[], unsigned int k[], unsigned int x[]);
unsigned int base64toint(const char *s);
const char *inttobase64(char *buf, unsigned int v, unsigned int count);

void uso(char *p);

char virtualhost[48];

int main(int argc,char *argv[]) {
  unsigned int v[2], k[2], x[2];
  unsigned long host;
  int ts=0;
  char clave[13];
  char tmp;

  if(argc < 2){
    uso(argv[0]);
  }

  strncpy(clave, argv[2], sizeof(clave) - 1);

  x[0] = x[1] = 0;

  tmp = clave[6];
  clave[6] = '\0';
  k[0] = base64toint(clave);
  clave[6] = tmp;
  k[1] = base64toint(clave + 6);

  host=inet_addr(argv[1]);

  v[0] = (k[0] & 0xffff0000) + ts;
  v[1] = ntohl((unsigned long)host);

  tea(v, k, x);

  inttobase64(virtualhost, x[0], 6);
  virtualhost[6] = '.';
  inttobase64(virtualhost + 7, x[1], 6);
  strcpy(virtualhost + 13, ".virtual");

  printf("Resultado : %s \n",virtualhost);
  exit(0);
}

void tea(unsigned int v[], unsigned int k[], unsigned int x[])
{
  unsigned int y = v[0] ^ x[0], z = v[1] ^ x[1], sum = 0, delta = 0x9E3779B9;
  unsigned int a = k[0], b = k[1], n = 32;
  unsigned int c = 0, d = 0;

  while (n-- > 0)
    {
      sum += delta;
      y += (z << 4) + a ^ z + sum ^ (z >> 5) + b;
      z += (y << 4) + c ^ y + sum ^ (y >> 5) + d;
    }

  x[0] = y;
  x[1] = z;
}

unsigned int base64toint(const char *s)
{
  unsigned int i = convert2n[(unsigned char)*s++];
  while (*s)
    {
      i <<= NUMNICKLOG;
      i += convert2n[(unsigned char)*s++];
    }
  return i;
}

const char *inttobase64(char *buf, unsigned int v, unsigned int count)
{
  buf[count] = '\0';
  while (count > 0)
    {
      buf[--count] = convert2y[(v & NUMNICKMASK)];
      v >>= NUMNICKLOG;
    }
  return buf;
}

void uso(char *p)
{
  printf("%s \n",VERSION);
  printf("uso : %s ip clave \n",p);
  exit(0);
}
