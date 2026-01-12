#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define MAX_PORT 1024

int s;
struct sockaddr_in addr;
char rmt_host[100];

int skan(port)
int port;
{
 int r;
    s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s < 0) {
       printf("[0;36mE[1mRROR[34m: [37m[0msocket() failed\n");
       exit(0);
    }
    addr.sin_family = PF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(rmt_host);
    r = connect(s,(struct sockaddr *) &addr, sizeof(addr));
    close(s);
    if (r==-1) {
       return (1 == 0);
    }
    return (1 == 1);
}

main(argc,argv) 
int argc;
char *argv[];
{
 int a,b,c,d,e,f;
 struct hostent *foo;
 struct servent *death;
   if (argc < 2) {
      fprintf(stderr,"\n[0;31mu[1msage[0m: [1m%s [0;5;34m<[0;1;34mhost[0;5;34m> [[0;1;34mhighest port[0;5;34m][0;1m[0m\n\n",argv[0]);
      fprintf(stderr, "The Death Knights Port Scanner [6m2.0[0m by Cacaio!\n");
      fprintf(stderr, "[1mh[0mttp://[1mw[0mww.[1md[0meatknights.[1mc[0mom         [1mCacaio[0m@[1md[0meathknights.[1mc[0mom\n\n");
      exit(0);
   }
   if (sscanf(argv[1],"%d.%d.%d.%d",&a,&b,&c,&d) != 4) {
      foo = gethostbyname(argv[1]);
      if (foo == NULL) {
         fprintf(stderr,"\n[0;36me[1mrror[0m: [1mc[0ma[1;30mnnot [37mr[0me[1;30msolve [37mh[0mo[1;30mst [0;31m[1m%s[0;0m[1m.[0m\n\n",argv[1]);
         fprintf(stderr, "The Death Knights Port Scanner [6m2.0[0m by Cacaio!\n");
         fprintf(stderr, "[1mh[0mttp://[1mw[0mww.[1md[0meatknights.[1mc[0mom         [1mCacaio[0m@[1md[0meathknights.[1mc[0mom\n\n");
         exit(0);
      }
      sprintf(rmt_host,"%d.%d.%d.%d",(unsigned char )foo->h_addr_list[0][0],
              (unsigned char ) foo->h_addr_list[0][1], 
              (unsigned char ) foo->h_addr_list[0][2], 
              (unsigned char ) foo->h_addr_list[0][3]);
   } else {
      strncpy(rmt_host,argv[1],99);
   }
   if (argc > 2) {
      f = atoi(argv[2]);
   } else
      f = MAX_PORT;
      fprintf(stdout,"\n[0;31mS[1mcanning [0;31mh[1most [37m%s [30m- [0;36mT[1mCP [0;36mp[1morts [37m1 [0;36mt[1mhrough [37m[1m%d\n\n",rmt_host,f);
      fprintf(stdout, "The Death Knights Port Scanner [6m2.0[0m by Cacaio!\n");
      fprintf(stdout, "[1mh[0mttp://[1mw[0mww.[1md[0meatknights.[1mc[0mom         [1mCacaio[0m@[1md[0meathknights.[1mc[0mom\n\n");
      fprintf(stdout, "If appears an [32mUNKNOWN[0m in any port, that is because that port is not listed\n");
      fprintf(stdout, "in [1myour[0m /etc/services.[0m\n\n");
   for (e =1;e<=f;e++) {
    char serv[100];
      if (skan(e)) {
         death = getservbyport(htons(e),"tcp"); 
         printf("[1m%d [0m([32m%s[0m) [9Ci[0ms [1mr[0mu[1;30mnning[37m.[1m\n",e,(death == NULL) ? "UNKNOWN" : 
         death->s_name); 
      }
   }
}