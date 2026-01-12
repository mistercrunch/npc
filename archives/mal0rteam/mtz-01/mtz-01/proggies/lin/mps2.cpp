/**********************************************************************
 *             C0d3d  by  SHYSAP ---->shysap@web-mail.com.ar         *
 *                                                                    *
 *             from mal0r team  ---->www.malorteam.com.ar             *
 *                                                                    *
 *                   (malor port scanner)                             *
 *                          mps.c                                     *
 *       "Un simple escanner de puertos TCP para Linux"               *
 **********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

main(int argc, char *argv[])
{

	int sock, h, puerto, lo = 0, hi = 0;
	struct sockaddr_in ellos;
   struct hostent *host;
   struct servent *serv;

   if (argc < 3)
   {
     	printf("\n\nUso: ./%s <ip> <puerto bajo> <puerto alto>\n\n", argv[0]);
      return 0;
   }

   if ((host = gethostbyname(argv[1])) == NULL)
   {
   	printf("\nLa IP no es valida\n");
      return 0;
   }
   else
     	h = inet_ntoa(*((struct in_addr*)host->h_addr));

   lo = atoi(argv[2]);
   hi = atoi(argv[3]);

   printf("Buscando puertos abiertos en %s ...\n", argv[1]);

   for (puerto = lo; puerto <= hi; puerto++)
   {
     	sock = socket(AF_INET, SOCK_STREAM, 0);
   	ellos.sin_family = AF_INET;
   	ellos.sin_port = htons(puerto);
   	ellos.sin_addr.s_addr = inet_addr(h);
		bzero(&(ellos.sin_zero), 8);

      if (connect(sock, (struct sockaddr *)&ellos, sizeof(ellos)) < 0)
      {
      	close(sock);
         continue;
      }

      serv = getservbyport(htons(puerto), "tcp");

      printf("\nPuerto  %d  abierto  <%s>\n", puerto, serv);

      if (serv == NULL)
      	printf("Desconocido\n");
      else
         serv->s_name;

      close(sock);
   }
	return 0;
}
