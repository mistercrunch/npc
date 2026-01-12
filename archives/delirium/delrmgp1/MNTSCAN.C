/*
######################
# Mountscan V1.0     #
# (C)opyright 1993   #
# Bluesman           #
######################
*/

#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <fcntl.h>
 
int p1[2],p2[2];        
int writefd,readfd;        

#define PUTS(s) write(writefd,(s),strlen(s))
#define GETC(c)  read(readfd,&(c),1)
 

void pipesetup()
{
  if(pipe(p2)==-1 || pipe(p1)==-1)
  {
    printf("Error making pipes.\n");
    return;
  }
}


int execscan(address)
{
  int cpid;

  readfd=p1[0];  
  writefd=p2[1]; 

  fcntl(readfd,F_SETFL,O_NDELAY);

  switch((cpid=fork())) 
  {
    case 0 : dup2(p2[0],0);  
             dup2(p1[1],1);  
             execlp("showmount","showmount","-e",address,NULL);
             printf("Exec of showmount failed.\n");
             exit(-1);
    default: break;
  }

return cpid;
}


void main(argn,argv)
int argn;
char **argv[];
{
char *try=    (char *)malloc(100);
char *ipa=    (char *)malloc(100);
char *ipb=    (char *)malloc(100);
char *ip=     (char *)malloc(50);
char *buffer= (char *)malloc(2000);

int x;
char ch;
 
int ip1,ip2;
 
int cpid;
char stop=0;

long Start;
int counter;

  if(argn!=3) 
  {
    printf("\nMountScan Usage: %s <StartIP> <StopIP>\n",argv[0]);   
    printf("Output is written to terminal.\n\n");
    exit(1);
  }
  
  pipesetup();     
 
  sprintf(ip,"%s.%s",strtok(argv[1],"."),strtok(NULL,"."));
  ip1=atoi(strtok(NULL,"."));
  ip2=atoi(strtok(NULL,"."));
  strcat(ipb,argv[2]);

  while(!stop)
  {

    Start=time(NULL);

    sprintf(ipa,"%s.%d.%d",ip,ip1,ip2);
    printf("HOST: %s\n",ipa);

    cpid=execscan(ipa);

    counter=0;

    while (counter < 5)
    {
      counter=time(NULL)-Start;
      if(GETC(ch)>0) 
      {
        printf("%c",ch);
        buffer[x++]=ch;
      }          
    } 

    x=0;
    if(!strcmp(ipa,ipb)) stop=1; 
    ip2++;
    if(ip2==256) { ip1++; ip2=0;}
  
    kill(cpid,9);
    wait(NULL);
  }
}
 
