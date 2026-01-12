/*  Extracted from CatHack #007 ( www.sindominio.net/cathack )   */

/* Exemple monitor de UDP emprant PF_PACKET i SOCK_RAW */
/* 2 Dec 2000 by izzu <azathoth@zxmail.com> */

/*
 *  No em faig responsable de qualsevol ús d'aquest software,
 *  bó o dolent.
 *
 *  Per a compilar podeu fer 'make usniff'
 */


/*
 *  Headers
 */

/* Basics */
#include <sys/socket.h>
#include <sys/ioctl.h>

/* PF_PACKET */
#include <linux/if_ether.h>
#include <netpacket/packet.h>
#include <net/if.h>

/* INET basics */
#include <arpa/inet.h>
#include <netinet/ether.h>
#include <netinet/in.h>

/* protocols */
#include <netinet/ip.h>
#include <netinet/udp.h>

/* per comoditat */
typedef struct ethhdr ETH;
typedef struct iphdr  IP;
typedef struct udphdr UDP;


/* Altres */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>


/*
 * 	process_packet()
 *
 *	processa un packet IP a nivell 2 (amb capçalera ETH) i
 *	imprimeix informacio a la sortida
 */

static int
process_packet(ETH *pkt_ETH, struct sockaddr_ll *addr)
{
  char hw_src[20], hw_dest[20];
  struct in_addr *in_src, *in_dest;
  IP *pkt_IP;
  UDP *pkt_UDP;
  void *data_UDP;

  pkt_IP = (IP *) (pkt_ETH + 1);

 /*
  * Si el paquet no es UDP el descartem d'entrada
  */

  if(pkt_IP->protocol != IPPROTO_UDP)
     return 0;

  pkt_UDP = (UDP *)((void *) pkt_IP + pkt_IP->ihl*4);
  data_UDP = pkt_UDP+1;

  printf("\n");

 /*
  *  escriu interficie per la que ha vingut el paquet,
  *  el tipus de paquet, el protocol, el tipus de ARP
  *  i la longitud de la direccio fisica
  */

  printf("IF: %u, TYPE: %u, PROTOCOL 0x%x, ARPTYPE %u, LENGTH %u",
         addr->sll_ifindex, addr->sll_pkttype, ntohs(addr->sll_protocol),
         addr->sll_hatype, addr->sll_halen);

 /*
  *  escriu el protocol i les direccions hardware de font i destinacio
  */

  ether_ntoa_r((struct ether_addr *) pkt_ETH->h_source, hw_src);
  ether_ntoa_r((struct ether_addr *) pkt_ETH->h_dest, hw_dest);

  printf(", LLPROTO 0x%x", ntohs(pkt_ETH->h_proto));
  printf(", HWS: %s, HWD: %s", hw_src, hw_dest);

 /*
  *  escriu parell IP - PORT tant de font com de destinacio
  */

  in_src = (struct in_addr *) &pkt_IP->saddr;
  in_dest = (struct in_addr *) &pkt_IP->daddr;

  printf(", IPS: %s, PS %u", inet_ntoa(*in_src),  ntohs(pkt_UDP->source));
  printf(", IPD: %s, PD %u", inet_ntoa(*in_dest), ntohs(pkt_UDP->dest));
  printf(", LEN: %u", ntohs(pkt_UDP->len));

 /*
  *  finalment escrivim les dades del paquet
  */

  printf(", DATA: "); fflush(stdout);
  return write(1, data_UDP, ntohs(pkt_UDP->len));
}


/*
 *  dades globals del programa
 */

static int sockfd;
struct ifreq dev_flags;

/*
 *  finalitzacio del programa (només amb senyals: TERM, QUIT i INT)
 *  [Codi despres de main()]
 */

static void
exit_handler(int)
      __attribute__((noreturn));

/*
 *	main()
 *
 *	Inicialitzacio i bucle principal
 */


int
main(int argc, char *argv[])
{
  int (*process)(void *, struct sockaddr_ll *) =
  (int (*)(void *, struct sockaddr_ll *)) process_packet;

  void *buffer;				/* packet buffer */
  struct sockaddr_ll addr_ll; 		/* link layer address */
  size_t size_ll = sizeof(addr_ll);     /* addr_ll size for bind() & recvfrom() */

  struct ifreq dev_req = {'0'};

  printf("USNIFF UDP MONITOR version 0.1\n");
  if(argc == 1){
     printf("ERROR: Argument missing - Please specify an interface\n");
     exit(1);
     }

  if(strstr(argv[1], "ppp")){
    printf("PPP devices are not compliant with the linux-2.4 PF_PACKET & SOCK_RAW standard\n"
           "This is likely to change in the future\n"
           "Sorry :(, you may use a link layer independent packet sniffer (SOCK_DGRAM for instance)\n");
    exit(1);
    }

  /*
   *  Construccio de l'entorn d'execucio
   */

  if(getuid()){
    printf("ERROR: You don't have permission to execute this application!\n");
    exit(0);
    }

  signal(SIGTERM, exit_handler);
  signal(SIGINT , exit_handler);
  signal(SIGQUIT, exit_handler);

  /*
   *   Inicialitzacio del socket
   */

  sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

  /*
   *   Inicialització de l'interficie:
   *   Primer hem d'esbrinar si l'interficie existeix
   *   En cas contrari retornarem amb un error
   *
   *   Tot aquest proces es realitza per que no volem escoltar
   *   a totes les interficies. Si fos el cas no caldria cridar
   *   a la funcio bind()
   */

  errno = 0;
  strncpy(dev_req.ifr_name, argv[1], IFNAMSIZ);
  ioctl(sockfd, SIOCGIFINDEX, &dev_req);

  if(errno){
  	printf("ERROR: The interface '%s' does not exist!\n", argv[1]);
  	exit(2);
  	}

  addr_ll.sll_family   = AF_PACKET;             /* packet socket */
  addr_ll.sll_protocol = htons(ETH_P_IP);       /* IP packets */
  addr_ll.sll_ifindex  = dev_req.ifr_ifindex;   /* specified interface */

  bind(sockfd, (struct sockaddr *) &addr_ll, size_ll);

  /*
   *  Ara es el moment de canviar el mode del dispositiu i passar a mode
   *  promiscu. Hi han dos maneres de fer aixó: 1) amb setsockopt() ó 2) amb
   *  una ioctl(). Emprarem ioctl() per a minimitzar nombre d'interficies.
   *  Per a informació sobre com activar el mode promiscu amb setsockopt()
   *  mirar packet(7).
   *
   *  El procés es extremadament senzill. Primer cridem una ioctl() per
   *  a guardar la paraula d'estat del dispositiu. A continuació activem
   *  el bit corresponent al mode promiscu i tot seguit activem la nova
   *  paraula d'estat
   */

    strcpy(dev_flags.ifr_name, argv[1]);
    ioctl(sockfd, SIOCGIFFLAGS, &dev_flags);

    dev_req.ifr_flags = dev_flags.ifr_flags;
    dev_req.ifr_flags |= IFF_PROMISC;

    ioctl(sockfd, SIOCSIFFLAGS, &dev_req);

  /*
   *  El tamany màxim d'una trama IP es de 64K. Aquest tamany no
   *  se sol assolir mai, ja que pràcticament cap canal ho suporta.
   *  Per exemple les trames 802.3 només poden arribar a 1538 bytes.
   *
   *  Donat que aquest tamany no es un impediment per a la memoria
   *  reservem un buffer de 64K i no ens hem de preocupar de possibles
   *  overflows
   */

  buffer = malloc(1<<16);

  /*
   *  Finalment entrem en el bucle d'execució
   */

  while(1){
     recvfrom(sockfd, buffer, 1<<16, 0, (struct sockaddr *) &addr_ll, &size_ll);
     process(buffer, &addr_ll);
     }

} /* end of main()*/


/*
 *  Quan arribi una interrupció voldrem restaurar l'estat original
 *  del dispositiu. Després tanquem el socket i cridem a exit()
 *  per a terminar el procés
 */


static void
exit_handler(int signum)
{

  ioctl(sockfd, SIOCSIFFLAGS, &dev_flags);
  close(sockfd);

  exit(0);
}
