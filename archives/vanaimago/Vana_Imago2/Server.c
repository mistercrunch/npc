// PROGRAMMINO DI ESEMPIO SCRITTO DA TECHNO KNIGHT PER LA QUARTA
// PUNTATA DEL CORSO DI C

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
int s,s2;
char io[200];
struct sockaddr_in me;
struct sockaddr_in te; 
struct hostent *hp;
char stringa[1024],registra[1024];
FILE *ilfile;

main(int argc,char *argv[]){

	bzero(me,sizeof me);	
	gethostname(io,199);
	hp = gethostbyname(io);
        me.sin_family=hp->h_addrtype;
        me.sin_port=htons(atoi(argv[2]));
	strcpy(stringa,argv[1]);
	strcat(stringa,"\n\r");
	
	s = socket(AF_INET,SOCK_STREAM,0);
        bind(s,(struct sockaddr *)&me,sizeof me); 
	listen(s,3);
	for (;;){
		s2=accept(s,NULL,NULL);
		send(s2,stringa,strlen(stringa),0);
		recv(s2,registra,1024,0);
		close(s2);
	        ilfile=fopen("/tmp/azzzz","a+");
	        fprintf(ilfile,"%s\n",registra);
	        fclose(ilfile);

	}
	
}
