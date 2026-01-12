/*
                Simple Scanner on NONblocking sockets.
                                <c> Free_Hunt

compile: gcc portscan.c -o portscan 
*/

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <err.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <fcntl.h>

int MS=50;  //Number of Sockets to use.

main (int argc, char *argv[])
{

        struct usock_descr{
                int s;                     // socket
                int state;                 // current socket state
                long timestamp;            // time, when sockets open in ms.
                unsigned short remoteport; // remote port
        };
        struct usock_descr sockets[MS];
        fd_set rfds, wfds;
        unsigned long start;
        struct hostent *nametocheck;
        struct sockaddr_in serv_addr;
        struct in_addr attack;
        struct timeval tv={0,0};
        char data[1024];
        int flags, max_fd ;
        int k,i,j,JB,JE,readb;
        int MAXTIME;


        if(argv[1] != NULL){start=inet_addr(argv[1]);} else usage();    
        if(argv[2] != NULL){JB=atoi(argv[2]);} else usage();    
        if(argv[3] != NULL){JE=atoi(argv[3])+1;} else usage();
        if(argv[4] != NULL){MAXTIME=atoi(argv[4]);} else usage();

        memset (&serv_addr, 0, sizeof (serv_addr));
        serv_addr.sin_family=AF_INET;
        serv_addr.sin_addr.s_addr = htonl(ntohl(start));
        j=JB;for (i=0;i<MS;i++){sockets[i].state=0;};
        
        printf("Scan - scanner on NONblock sockets.\nRunning...\n");
        k=1;
    while((j<JE)||(k==1)){
          for (i=0;(i<MS) && (j<JE);i++){
               if (sockets[i].state==0) {
                    if((sockets[i].s = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP))==-1){printf("error in socket %d\n",errno);return 1;};
                    flags = fcntl(sockets[i].s, F_GETFL, 0);
                    if(
                    fcntl(sockets[i].s, F_SETFL, flags | O_NONBLOCK) == -1){printf("error in NONBLOCK set\n");return 1;};
                    sockets[i].state=1;
               }
          };            
          
          for (i=0;(i<MS) && (j<JE);i++){
                if (sockets[i].state==1) {
                    serv_addr.sin_port = ntohs(j);
                    if(connect (sockets[i].s, (struct sockaddr *)&serv_addr, sizeof (serv_addr))==-1)  {
                             if(errno != EINPROGRESS)
                              {
//                                  printf("error in connecting(not einproress)port: %d\terrno:%d\n",j,errno);
                                  shutdown(sockets[i].s,2);
                                  close(sockets[i].s);
                                  sockets[i].state=0;
                              }else{sockets[i].state=2;};
                    }
                    else
                      {
                         printf ("CONNECT %d \n",j);
                         sockets[i].state=3;
                      };
                    sockets[i].timestamp=time(NULL);
                    sockets[i].remoteport=j;
                    j++;
                };
          }; 

          FD_ZERO(&rfds); 
          FD_ZERO(&wfds);
          max_fd = -1;

          for (i=0;i<MS;i++){
                if (sockets[i].state==2) {
                       FD_SET(sockets[i].s, &wfds);
                       FD_SET(sockets[i].s, &rfds);
                       if(sockets[i].s > max_fd) max_fd = sockets[i].s;
                };
          };

          select(max_fd + 1, &rfds, &wfds, NULL, &tv);

          for (i=0;(i<MS);i++){
               if (sockets[i].state==2) {       
                      if(FD_ISSET(sockets[i].s, &wfds) || FD_ISSET(sockets[i].s, &rfds))
                      {
                              socklen_t err_len;
                              int error;
                              err_len = sizeof(error);
                              if(getsockopt(sockets[i].s, SOL_SOCKET, SO_ERROR, &error, &err_len) < 0 || error != 0)
                                             {
//                                                printf("error in connecting- getsocksopt %d\n",sockets[i].remoteport);
                                                  shutdown(sockets[i].s,2);
                                                  close(sockets[i].s);
                                                  sockets[i].state=0;
                                             }
                                             else  
                                             {
                                                  printf ("CONNECT %d \n",sockets[i].remoteport);
                                                  sockets[i].state=3;
                                             };
                      }
                      else
                      { if ( (time(NULL)-sockets[i].timestamp) > MAXTIME)                     
                                       {  
//                                        printf("error in connect - timeout\n");
                                          shutdown(sockets[i].s,2);
                                          close(sockets[i].s);
                                          sockets[i].state=0;   
                                       };
                      };
               };
          };

//state 3         -------------------------------------------
          
          FD_ZERO(&rfds); 
          max_fd = -1;

          for (i=0;i<MS;i++){
                if (sockets[i].state==3){
                       FD_SET(sockets[i].s, &rfds);
                       if(sockets[i].s > max_fd) max_fd = sockets[i].s;
                };
          };

          select(max_fd + 1, &rfds, NULL, NULL, &tv);

          for (i=0;(i<MS);i++){
               if (sockets[i].state==3) {       
                      if(FD_ISSET(sockets[i].s, &rfds))
                      {
                              socklen_t err_len;
                              int error;
                              err_len = sizeof(error);
                              if(getsockopt(sockets[i].s, SOL_SOCKET, SO_ERROR, &error, &err_len) < 0 || error != 0)
                                             {
                                                  shutdown(sockets[i].s,2);
                                                  close(sockets[i].s);
                                                  sockets[i].state=0;
                                             }
                                             else  
                                             {
                                                  printf("read data %d \n",sockets[i].remoteport);
                                                  memset(data,0,sizeof(data));
                                                  readb=read(sockets[i].s,&data,1024);
                                                  printf("%s\n",&data);
                                                  sockets[i].state=3;
                                                  if (readb==0){
                                                  sockets[i].state=4;
                                                  
                                                      };
                                             };
                      }
                      else
                      { if ( (time(NULL)-sockets[i].timestamp) > MAXTIME)                     
                                       {  
                                                  sockets[i].state=4;
                                       };
                      };
               };
          };
//state 4
          FD_ZERO(&wfds);
          max_fd = -1;

          for (i=0;i<MS;i++){
                if (sockets[i].state==4) {
                       FD_SET(sockets[i].s, &wfds);
                       if(sockets[i].s > max_fd) max_fd = sockets[i].s;
                };
          };

          select(max_fd + 1, NULL, &wfds, NULL, &tv);

          for (i=0;(i<MS);i++){
               if (sockets[i].state==4) {       
                      if(FD_ISSET(sockets[i].s, &wfds))
                      {
                              socklen_t err_len;
                              int error;
                              err_len = sizeof(error);
                              if(getsockopt(sockets[i].s, SOL_SOCKET, SO_ERROR, &error, &err_len) < 0 || error != 0)
                                             {
                                                  shutdown(sockets[i].s,2);
                                                  close(sockets[i].s);
                                                  sockets[i].state=0;
                                             }
                                             else  
                                             {
                                                  printf("write data %d \n",sockets[i].remoteport);
                                                  write(sockets[i].s,"test\n",5);
                                                  sockets[i].state=5;
                                             };
                      }
                      else
                      { if ( (time(NULL)-sockets[i].timestamp) > MAXTIME)                     
                                       {  
                                          shutdown(sockets[i].s,2);
                                          close(sockets[i].s);
                                          sockets[i].state=0;   
                                       };
                      };
               };
          };

//state 5         -------------------------------------------
          
          FD_ZERO(&rfds); 
          max_fd = -1;

          for (i=0;i<MS;i++){
                if (sockets[i].state==5){
                       FD_SET(sockets[i].s, &rfds);
                       if(sockets[i].s > max_fd) max_fd = sockets[i].s;
                };
          };

          select(max_fd + 1, &rfds, NULL, NULL, &tv);

          for (i=0;(i<MS);i++){
               if (sockets[i].state==5) {       
                      if(FD_ISSET(sockets[i].s, &rfds))
                      {
                              socklen_t err_len;
                              int error;
                              err_len = sizeof(error);
                              if(getsockopt(sockets[i].s, SOL_SOCKET, SO_ERROR, &error, &err_len) < 0 || error != 0)
                                             {
                                                  shutdown(sockets[i].s,2);
                                                  close(sockets[i].s);
                                                  sockets[i].state=0;
                                             }
                                             else  
                                             {
                                                  memset(data,0,sizeof(data));
                                                  readb=read(sockets[i].s,&data,1024);
                                                  if (readb<1){
                                                  shutdown(sockets[i].s,2);
                                                  close(sockets[i].s);
                                                  sockets[i].state=0;
                                                  }else{
                                                  printf("read data %d \n",sockets[i].remoteport);
                                                  printf("%s\n",&data);
                                                       };
                                                  sockets[i].state=5;
                                             };
                      }
                      else
                      { if ( (time(NULL)-sockets[i].timestamp) >2*MAXTIME)                    
                                       {  
                                          shutdown(sockets[i].s,2);
                                          close(sockets[i].s);
                                          sockets[i].state=0;
                                       };
                      };
               };
          };
          k=0;
          for (i=0;(i<MS);i++){if(sockets[i].state>1){k=1;};};
    };
    
        printf("End.\n");
return 0;
};


int usage()
{
        printf("Syntax:  portscan <ip> <port_begin> <port_end> <timeout in sec>\n");
        printf("Example: ./portscan 127.0.0.1 80 139 5\n");
        exit(1);
}
