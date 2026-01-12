/*
   hhp-ms.c (Messaging System v0.2)
   by tGb.
   h0h0h0 hhp kickin it in 99.

   Tested on Slackware and RedHat 2.0.36 (Might work on FreeBSD)
   To compile: gcc hhp-ms.c -o hhp-ms.

   Coming in future versions.
   Account management, message management, user-to-user messaging,
   and passworded accounts.
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define VER "v0.2"
#define BACKLOG 20
#define BUF 10000

int newfd;

int main()
{
  int sockfd, sin_size, bytes, ss;

  struct sockaddr_in mine;
  struct sockaddr_in theirs;

  char passwd[1024];
  char *gp;
  char dir[50];
  int port;
  char password[50];

  printf("What /path/filename would you like the msgs to be stored? ");
  scanf(" %s", dir);
  printf("What port do you want to run this on? ");
  scanf(" %d", &port);
  printf("What do you want the port password to be? ");
  scanf(" %s", password);

  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror("socket()");
      exit(1);
    }

  mine.sin_family = AF_INET;
  mine.sin_addr.s_addr = INADDR_ANY;
  mine.sin_port = htons(port);
  bzero(&(mine.sin_zero), 8);

  if (bind(sockfd, (struct sockaddr *)&mine, sizeof(struct sockaddr)) ==
-1)
   {
     perror("bind()");
     exit(1);
   }


  if (listen(sockfd, BACKLOG) == -1) {
   perror("listen()");
   exit(1);
  }
  printf("hhp-ms.c (Message System %s) - by: tGb Loaded! on port: %d\n",
VER, p
ort);
        if(fork())
         {
          exit(0);
         }
        else
 while(1)
   {
     ss = sizeof (struct sockaddr_in);
     if((newfd = accept(sockfd, (struct sockaddr *) &theirs, &sin_size))
== -1)
       {
         perror("accept()");
       }
         bytes = send(newfd, "Welcome to tGb's hhp-ms.c (Message System
v0.2)\n
Please enter your password: ", 77, 0);
         recv(newfd, passwd, 1024, 0);

         if((gp = strchr(passwd, 13)) != NULL)
           *(gp) = '\0';

         if(!strcmp (passwd, password))
           {
             send(newfd, "Access Granted!\n", 16, 0);
             message(dir);
           }
         else if (passwd != password)
           {
             send(newfd, "Access denied!\n", 15, 0);
             close(newfd);
           }
        }
     }


message(char *dir)
{
  FILE *log;
  char msg[BUF];
  char name[30];

  memset(msg, 0, BUF);
  memset(name, 0, 30);

  log = fopen(dir, "a+");
  send(newfd, "Name: ", 6, 0);
  recv(newfd, name, 30, 0);
  fprintf(log, "-----------------------------------\n");
  fprintf(log, "From: %s\n", name);

  send(newfd, "Your message: ", 14, 0);
  recv(newfd, msg, BUF, 0);

  fprintf(log, "%s\n", msg);
  fprintf(log, "-----------------------------------\n");
  fclose(log);

  send(newfd, "Message sent.\n", 14, 0);
  close(newfd);
}
