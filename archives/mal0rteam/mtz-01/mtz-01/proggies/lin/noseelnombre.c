
// Detector de Scan TCP y UDP  
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h> // if you want to improve the code for posix :)
#include <time.h>
#include <getopt.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define TCPORT 45 // change this number, for a better program
#define UDPORT 46 // this too
#define MAXLOG 50
#define _GNU_SOURCE

main(int argc, char *argv[])
{
  time_t atime;
  int parameter;
  int sin_size;

  if (argc == 1) // if nothing given like arguments
    {
      fprintf(stderr,"\nantilame : kill'em all :)\n\nBeware, lamer are everywhere in the world!\nantilame was builded for look on them 8-!\nlamer scan many time their imaginary targets whitout any discretion.\n\nantilame -d : wait for udp scans (sock_dgram)\nantilame -s : wait for tcp scans (sock_stream)\n\n");
      return(1);
    }

  parameter = getopt(argc, argv, "sd");

  switch(parameter)
    {

    // tcp
    case 's' : {
      int socket_tcp, socket_tcp_used;
      struct sockaddr_in my_addr_tcp;
      struct sockaddr_in their_addr_tcp;

      if ((socket_tcp = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
	  perror("tcp socket");
	  exit(1);
	}
 
      my_addr_tcp.sin_family = AF_INET;     
      my_addr_tcp.sin_port = htons(TCPORT);  
      my_addr_tcp.sin_addr.s_addr = INADDR_ANY;
      memset(&(my_addr_tcp.sin_zero),0,8);

      if (bind(socket_tcp, (struct sockaddr *)&my_addr_tcp, sizeof(struct sockaddr))== -1) 
	{
	  perror("tcp bind");
	  exit(1);
	}

      if (listen(socket_tcp, MAXLOG) == -1) 
	{
	  perror("tcp listen");
	  exit(1);
	}
      while(1) {  
	sin_size = sizeof(struct sockaddr_in);

	if ((socket_tcp_used = accept(socket_tcp, (struct sockaddr *)&their_addr_tcp, &sin_size)) == -1) 
	  {
	    perror("tcp accept");
	    continue;
	  }
	atime = time(NULL);
	printf("%s : %s >is scanning you!!(tcp)\n", inet_ntoa(their_addr_tcp.sin_addr),ctime(&atime));
	close(socket_tcp_used);

	while(waitpid(-1,NULL,WNOHANG) > 0);
      }
      break;
    }

    // udp
    case 'd' : {
      int socket_udp;
      struct sockaddr_in my_addr_udp;
      struct sockaddr_in their_addr_udp;
      char buf[MAXLOG];
      int received;
      
      if ((socket_udp = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
        {
	  perror("udp socket");
	  exit(1);
	}

      my_addr_udp.sin_family = AF_INET;
      my_addr_udp.sin_port = htons(UDPORT);
      my_addr_udp.sin_addr.s_addr = INADDR_ANY;
      memset(&(my_addr_udp.sin_zero),0,8); 

      if (bind(socket_udp, (struct sockaddr *)&my_addr_udp, sizeof(struct sockaddr))== -1)
	{
	  perror("udp bind");
	  exit(1);
	}   	

    	sin_size = sizeof(struct sockaddr_in);
	while(1) {
      if ((received=recvfrom(socket_udp, buf, MAXLOG, 0, (struct sockaddr *)&their_addr_udp, &sin_size)) == -1)
	{
	  perror("udp recvfrom");
	  exit(1);
	}
	atime = time(NULL);
	printf("%s : %s >is scanning you!!(udp)\n", inet_ntoa(their_addr_udp.sin_addr),ctime(&atime));      
    	
      close(socket_udp);
	}
      break;
    }

    }
  return 0;
}