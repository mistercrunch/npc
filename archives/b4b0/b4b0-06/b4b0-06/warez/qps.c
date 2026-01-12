/*  POPScan QPOP/UCB/SCO scanner by KKR
    kkr@engulf.com

    takes list of ip's from stdin
    
   The hosts gathered by this scanner are 
   almost 100% vulnerable to a remote
   root attack.  The exploits used to root
   the vulnerable machines can all be found by
   searching bugtraq.  UCB pop is 100% of the
   time vulnerable to the qpop exploit (it's a very
   old version of qpop).  The QPOP version is 
   filitered to make sure that non-vulnerable 
   versions do not show up in the scan.
   Common offsets for the bsd qpop exploit are:
    621, 1500, 500, 300, 900, 0

Example usage:

./z0ne -o ac.uk | ./pops > ac.uk.log &
would scan ac.uk for vulnerabilities.

much help from jsbach    


*/

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <signal.h>

int ADMtelnet (u_long, int port);
char domain[50];
int NUMCHILDREN = 150, currchilds = 0; /* change numchildren to taste */
char ip[16];
int temp1 = 0;
void scan(char *ip);
void alrm(void) { return; }

main()
{

   while( (fgets(ip, sizeof(ip), stdin)) != NULL)
          switch(fork()) {
            case 0: {
	    scan(ip); exit(0);
            }
            case -1: {
              printf("cannot fork so many timez@!@^&\n");
              exit(0);
              break;
              }
            default:
                {
                currchilds++;
                if (currchilds > NUMCHILDREN)
                  wait(NULL);
                break;
                }
	  }

}

void scan(char *ip)
{
char printip[16];
struct sockaddr_in addr;
int sockfd;
char buf[512];

bzero((struct sockaddr_in *)&addr, sizeof(addr));
sockfd = socket(AF_INET, SOCK_STREAM, 0);

addr.sin_addr.s_addr = inet_addr(ip);
addr.sin_port = htons(110);
addr.sin_family = AF_INET;
signal(SIGALRM, alrm);
alarm(5);
if ( (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) != -1))
{
recv(sockfd, (char *)buf, sizeof(buf), 0);

if ( (strstr(buf, "QPOP") ) != NULL && (strstr(buf, "2.5")) == NULL && (strstr(buf, "krb")) == NULL)
 {
 checkos(ip,1);
}

if((strstr(buf, "UCB")) != NULL)
checkos(ip,2);

if((strstr(buf, "SCO")) != NULL)
 {
  strcpy(printip, ip);
  if ((temp1=strrchr(printip, '\n')) != NULL)
   bzero(temp1, 1);
   printf("%s: SCO Unix box running SCO pop.\n",printip);
  } 
}
return;
}
// }


checkos(char *ip, int spl)
{
int temp2;
char printip[16];
unsigned long temp;
temp = inet_addr(ip);
temp2 = ADMtelnet(temp, 23);
  strcpy(printip, ip);
  if ((temp1=strrchr(printip, '\n')) != NULL)
   bzero(temp1, 1);

if ((temp2 == 1)&&(spl==1))
 printf("%s: OpenBSD box running vuln QPOP\n",printip);
if ((temp2 == 1)&&(spl==2))
 printf("%s: OpenBSD box running vuln UCB pop\n",printip);
if ((temp2 == 2)&&(spl==1))
 printf("%s: FreeBSD box running vuln QPOP\n",printip);
if ((temp2 == 2)&&(spl==2))
 printf("%s: FreeBSD box running vuln UCB pop\n",printip);
if ((temp2 == 3)&&(spl==1))
 printf("%s: BSDi box running vuln QPOP\n",printip);
if ((temp2 == 3)&&(spl==2))
 printf("%s: BSDi box running vuln UCB pop\n",printip);

}

int ADMtelnet (u_long ip, int port)
{
  struct sockaddr_in sin;
  u_char buf[4000];
  int dasock, len;
  int longueur = sizeof (struct sockaddr_in);

  dasock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);	/* gimme a socket */

  sin.sin_family = AF_INET;
  sin.sin_port = htons (port);
  sin.sin_addr.s_addr = ip;

  if (connect (dasock, (struct sockaddr *) &sin, longueur) == -1)
    return (-1);

  while (1)
    {
      memset (buf, 0, sizeof (buf));

      if ((len = read (dasock, buf, 1)) <= 0)
	break;

      if (*buf == (unsigned int) 255)
	{
	  read (dasock, (buf + 1), 2);
	  if (*(buf + 1) == (unsigned int) 253 && !(u_char) * (buf + 2));
	  else if ((u_char) * (buf + 1) == (unsigned int) 253)
	    {
	      *(buf + 1) = 252;
	      write (dasock, buf, 3);
	    }
	}
      else
	{
	  if (*buf != 0)
	    {
	      bzero (buf, sizeof (buf));
	      read (dasock, buf, sizeof (buf));
	      usleep(40000);

        if((strstr(buf, "OpenBSD") != NULL)) 
         return 1;
        if((strstr(buf, "FreeBSD") != NULL)) 
         return 2;
        if((strstr(buf, "BSDI") != NULL)) 
        return 3;
   
	      sleep (1);
	    }

	}

    }
  return 0;
}
