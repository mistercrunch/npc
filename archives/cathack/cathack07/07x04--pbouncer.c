/*  Extracted from CatHack #007 ( www.sindominio.net/cathack )   */

/********************
 *    pBouncer.c    *
 ********************
 
 Nom  : pBouncer.c
 Autor: Pop.tPH (Pancake for the friends ;) ( poptph@hotmail.com )
 Grup : CatHack! ( www.sindominio.net/cathack )
 Notes: pbouncer es un bouncer escrit desde zero, intentant reduir-lo al
        maxim, soc conscient dels fallos que te, pero tp tinc gaire temps
        per poder-m'ho mirar amb calma, entre els estudis i demes...En el
        futur si tinc temps m'agradaria reescriure'l per poder oferir un
        millor bouncer, amb opcions de gateway i fins hi tot de proxy :)
        Espero que us agradi i que disfruteu llegint i utilitzant-lo tant
        com jo escribint-lo.
*/


#include <stdio.h>
#include <netinet/in.h>

#define l_po 9000             /* local port */
#define r_ip 192.168.0.1      /* remote ip (tb pot ser url) */
#define r_po 21               /* remote port */
#define verbose 1             /* comentar la jugada? (0=NO, 1=SI) */
#define echo 1                /* escoltar la comunicacio (0=NO, 1=SI) */
#define bufer 76              /* tamany del buffer a escriure */

int main(int argc, char *argv[])
{
int fill=0;
typedef struct sockaddr_in ips;                           /* IP Structure 
*/
  ips     dListen = { AF_INET, htons(l_po), htonl(INADDR_ANY) };
      ips dIncome = { AF_INET, htons(l_po), htonl(INADDR_ANY) };
      ips dRemote = { AF_INET, htons(r_po), gethostbyname("r_ip") };
  int     sListen = socket(AF_INET, SOCK_STREAM, 0);
      int sIncome = socket(AF_INET, SOCK_STREAM, 0);
      int sRemote = socket(AF_INET, SOCK_STREAM, 0);
int length_buffer=0;
char buffer[1024];

fflush(stdout);
printf("pBouncer v0.01 by Pancake/CatHack!\n");

if ( bind(sListen, (ips *) &dListen, sizeof (ips)) < 0 )
  { printf("ERROR - Binding.\n");
  #if verbose
    printf(" -> Segurament el port d'escolta ja s'esta utilitzant.\n");
  #endif
  } else {                          /* exit(1) */
    
if ( listen(sListen,1) < 0 )
  { printf("ERROR - Listening.\n");
  #if verbose
     printf(" -> Segurament el sistema no permet mes sockets :(( \n");
  #endif
  } else {                          /* exit(1) */

sIncome = accept(sListen,NULL,NULL);

  #if verbose
  printf("Conexio remota rebuda...\n");
  #endif

if ( connect(sRemote, (ips *) &dRemote, sizeof(ips)) < 0 )
   printf ("\nConectat a l'extrem remot");

fill=fork();
if ( fill < 0 ) 
  { printf("ERROR - Forking.\n");
    #if verbose
    printf(" -> El proces actual es impotent.\n");
    #endif
    } else {
    
if (fill!=0) {               /* derivacio del fill */
   while((length_buffer=read(sIncome, buffer, bufer)) > 0)
        {
        write(1,"\n[I]", 5);
        write(1, buffer, length_buffer);
        write(sRemote, buffer, length_buffer);
        }
}
else {                       /* derivacio del pare */
   while((length_buffer=read(sRemote, buffer, bufer)) > 0)
        {
        printf("\n[R]");
        write(1, buffer, length_buffer);
        write(sIncome, buffer, length_buffer);
        }

}

close(sIncome);
close(sRemote);
close(sListen);

exit(0);
}
exit(1);
}}
}


