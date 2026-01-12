#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <errno.h>
#define ERRO -1
#define SIM 1
#define NAO 0

typedef char boolean;

char ip[20];
char final_ip[20];
char *protocolo="tcp";
char *proto_udp="udp";
struct hostent *p;

char ip_classe(int);
boolean portscan(int, char *);
void sair(void);
void ajuda(char *);

time_t t1, t2;

void main(int argc, char *argv[]){

int campo1,
    campo2,
    campo3,
    campo4,
    i,
    i1,
    i2,
    i3,
    i4,
    port,
    port_inicial=1,
    port_final=1024;

boolean nao_scan=NAO,
        flag=0,
        no_msg=SIM;

unsigned long host;
struct servent *servico;

 if(argc<2){
   printf("elektro@null.net\n");
   printf("Uso: %s [-opcoes] <ip_inferior> [ip_superior]\n", argv[0]);
   exit(1);
   }

 for(i=1; i<argc; i++){
    if(argv[i][0]=='-')
      switch(argv[i][1]){

      case 'i':
	       {
		port_inicial=atoi(&argv[i][2]);
                break;
	       }
      case 'f':
	       {
		port_final=atoi(&argv[i][2]);
		break;
	       }
      case 'n':
	       {
		nao_scan=SIM;
		break;
	       }
      case 'h':
	       {
		ajuda(argv[0]);
		exit(0);
	       }
      case 'u':
	       {
                protocolo=proto_udp;
                break;
	       }
      case 'm':
	       {
                no_msg=NAO;
                break;
	       }
      default:
	      {
	       printf("Opcao errada!\n");
	       exit(1);
	      }
      }
    else{
      if(!flag){
        flag=1;
        strncpy(ip, argv[i], 20);
	strncpy(final_ip, argv[i], 20);
        }
      else
	strncpy(final_ip, argv[i], 20);
    }
 }
     
if(sscanf(ip, "%d.%d.%d.%d", &campo1, 
                             &campo2, 
                             &campo3, 
	                     &campo4)!=4)
{
printf("O ip deve ser escrito na forma XXX.XXX.XXX.XXX\n");
exit(1);
}                           

time(&t1);

printf("*************\n");
printf("* Protocolo *: %s\n", protocolo);
printf("*************\n");

for(i1=campo1; i1<=255; i1++){
  for(i2=campo2; i2<=255; i2++){
    for(i3=campo3; i3<=255; i3++){
      for(i4=campo4; i4<=255; i4++){

      sprintf(ip, "%d.%d.%d.%d", i1, i2, i3, i4);
      if((host=inet_addr(ip))==ERRO)
       {
	printf("%s (endereco invalido)\n", ip);
	exit(0);
	}
      if((p=gethostbyaddr((char *) &host, sizeof(long), AF_INET))==NULL){
       if(no_msg)
        herror(ip);
        }
	else{
         printf("\n\nIP: %s\n", ip);
         printf("DNS: %s\n", p->h_name);
         printf("IP classe: %c\n", ip_classe(i1));
         printf("Aliases:");
         i=0;
         while(p->h_aliases[i]!=NULL){
          printf(" %s,", p->h_aliases[i]);
          i++;
	  }
         if(!nao_scan){
           printf("ports abertas:\n\n");	     
           for(port=port_inicial; port<=port_final; port++)
	     if(portscan(port, ip)){
               printf("%4d ", port);
               if((servico=getservbyport(htons(port), protocolo))!=NULL)
                 printf(" --> servico: %s\n", servico->s_name);
                 else
		  printf(" --> servico: DESCONHECIDO\n");
	     }
	 }
      }

      if(!strcmp(ip, final_ip)){
         time(&t2);
	 sair();
         }
	 }
       }
    }
  }
}
 

char ip_classe(int i){

 if(i>=0 && i<=127)
   return('A');
	       
 if(i>=128 && i<=191)
   return('B');

 if(i>=192 && i<=223)
   return('C');

 if(i>=224 && i<=239)
   return('D');

 if(i>=240 && i<=247)
   return('E');

}

boolean portscan(int port, char *ip_addr){
int r,
    sock;
struct sockaddr_in addr;


 if((sock=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP))==ERRO){
     perror("socket");
     exit(1);
     }

    addr.sin_family = PF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip_addr);

    r=connect(sock,(struct sockaddr *) &addr, sizeof(addr));

    close(sock);
    if (r==ERRO)
       return (NAO);
  
       return (SIM);
}

void sair(void){

printf("\n\n");
printf("Iniciado em:  %s", ctime(&t1));
printf("Terminado em: %s", ctime(&t2));
exit(0);

}

void ajuda(char *s){

printf("Contacto: elektro@null.net\n");
printf("Uso: %s [-opcoes] <ip inicial> [ip final]\n", s);
printf("            -h imprime esta ajuda e sai\n");
printf("            -iXX altera a port inicial para XX (def. 1)\n");
printf("            -fXX altera a port final para XX (def. 1024)\n");
printf("            -n nao faz port scanning\n");
printf("            -u altera a pesquisa dos servicos para udp (def. tcp)\n");
printf("            -m nao imprime mensagens de erro\n");

}








