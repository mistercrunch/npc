/*
 *  MDAC-scan.c
 *
 * 
 *  Code improvements + comments always welcome.
 *  Send them to: gin@dds.nl.
 *
 *  #Phreak.nl Current Website: http://www.casema.net/~gin
 *  Future Website: http://home.wanadoo.nl/~gin
 *
 *  -- Xphere --
 */



#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <unistd.h>



void catch_alarm()
{
     alarm(0);
     signal(SIGALRM, SIG_DFL);
     exit(0);
}



int main(int argc, char *argv[])
{
     FILE *in, *out;
     char ip[50];
     int pid;
     signal(SIGCHLD, SIG_IGN);
     signal(SIGALRM, catch_alarm);
     

     if (argc != 3) {
          fprintf(stderr, "\n\e[0;34m[ MDAC scanner by: ");
	  fprintf(stderr, "Xphere -- #Phreak.nl ]\e[0m\n\n");
	  fprintf(stderr, "Usage: %s <host_list> <log_file> &\n", argv[0]);
	  exit(0);
     }
     
     in = fopen(argv[1], "r");
     out = fopen(argv[2], "a");
     
     if (in == 0) {
          fprintf(stderr, "Error reading %s.\n", argv[1]);
	  exit(0);
     }

     if (out == 0) {
	  fprintf(stderr, "Error creating %s.\n", argv[2]);
	  exit(0);
     }

     while (fscanf(in, "%s\n", ip) != EOF) {
	  pid = fork();

	  if (pid < 0) {
	       fprintf(stderr, "Fork Error.\n");
	       exit(0);
	  }
	  else if (pid > 0) {
	       sleep(10);     /* You got the bandwidth? */
	  }
	  else if (pid == 0) {
               alarm(25);
	       checkh(ip, 80, argv[2]);
	       alarm(0);
	       exit(0);
	  }
     }
     sleep(5);
}



int checkh(char *ip, int port, char *file)
{
     int s, r, c;
     char *string = "GET /msadc/msadcs.dll HTTP/1.0\r\n\r\n";
     char *search = "HTTP/1.1 200 OK";
     char serv_string[100];
     FILE *out;
     
     struct sockaddr_in addr;
     struct hostent *foo;
     bzero((char *) &addr, sizeof(addr));
     addr.sin_family = AF_INET;
     addr.sin_addr.s_addr = inet_addr(ip);
     addr.sin_port= htons(port);

     if ((foo = gethostbyname(ip)) != NULL) {
          bcopy(foo->h_addr, (char *) &addr.sin_addr, foo->h_length);
     }
     else {
	  if ((addr.sin_addr.s_addr = inet_addr(ip)) < 0) {
	       return(0);
	  }
     }

     s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
     r = connect(s, (struct sockaddr *) &addr, sizeof(addr));

     write(s, string, strlen(string));
     c = 0;
     while (read(s, serv_string, sizeof(serv_string)) && c == 0) {
	  if (strstr(serv_string, search) != NULL) {
	       out = fopen(file, "a");
	       fputs("Server: ", out);
	       fputs(ip, out);
	       fputs(" runs MDAC.\n", out);
	       fclose(out);
	       c = 1;
	  }
     }
     close(s);
     return(0);
}
