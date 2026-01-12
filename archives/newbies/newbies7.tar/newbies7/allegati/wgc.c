/*
                        WinGate Connector 1.00
                by tHE rECIdjVO <recidjvo@pkcrew.org>


This is an anonymous datapipe that uses WinGate addresses.
	For any report please write to: <recidjvo@pkcrew.org>

The concept is simple: it's a mix of datapiping and wingate use:
creating a datapipe that doesn't connect directly to the target
host, but bounce between some wg, we can estabilish an anonymous
connection using canonical programs.

Here it is a connection session example:

   POP-3 client
	   ||	    ______   _____   _____
	   \/	   //----\\ //---\\ //---\\
	localhost:110	wg1:23	wg2:23	target.com:110

You have to run:
	% ./wgc -l 110 -h target.com -p 110

and, if it's allright, connect to localhost:110.

It's simple, isn't it?!? ;)

To check your WinGate list just run wgc in --verify mode, and
it will try to connect to each address and recognize if there
a WinGate server running on port 23.
IMPORTANT: it estabilishes a complete connection to host, so,
	*** DON'T USE THIS FUNCTION FOR PORTSCANNING ***
or you'll receive an email asking you what damn are you doing.
It's recommended to run periodically the -v mode moving the
verified list into the check list, to reconfirm WinGates are
still working, because each scan append valid addresses at the
bottom, and don't rescan old good WinGates.

NOTE: This is about the concept of datapipe: it works only with
one-stream connection protocols, like telnet or smtp. Others,
like http or ftp are structured to use more than one stream, so
it's very hard to use them with wgc.

Please turn on your brain before using this program.
						tHE rECIdjVO

Personal thanks: |CyRaX|, Guybrush, cod, pbm, Gigi Sullivan.
Musical greets: Depeche Mode, Robert Miles, Dream Theater.

Compile with: % gcc -o wgc wgc.c

Usage:  wgc -l LOCAL_PORT -h REMOTE_HOST -p REMOTE_PORT [--high-security]
	wgc --help | --verify | --compact
								*/

/*		-=| START OF CONFIGURATION SECTION |=-		*/

#define FILELIST_IN "./wingates.lst"
#define FILELIST_OUT "./wingates.vrfy"
#define FILELIST_SWP "./wingates.swp"
#define TIMEOUT 10

/*		-=| END OF CONFIGURATION SECTION |=-		*/

// Const declaration
#define PROGRAM "wgc"
#define VERSION "1.00"
#define INFO "by tHE rECIdjVO"
#define CONTACT "<recidjvo@pkcrew.org>"
#define PORT 23
#define BOLD "\033[1m"
#define FLAT "\033[m"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Functions prototypes
int main(int argc, char *argv[]);
void usage(void);
void verify(void);
int vrfywgate(char *wingate_addr);
void compact(char *filename);
u_long resolv(char *hostname);
void wgconnect(int l_port, char *r_host, int r_port, int secure);

int main(int argc, char *argv[])
{
   printf("\n   " BOLD PROGRAM " v." VERSION FLAT " - " INFO " " CONTACT "\n\n");
   if (argc < 2) {
      printf("Syntax error. Try " PROGRAM " --help.\n\n");
      exit(1);
   }
   if (strcmp(argv[1], "--help") == 0) {
      usage();
   } else if ((strcmp(argv[1], "--verify") == 0) || (strcmp(argv[1], "-v") == 0)) {
      verify();
   } else if ((strcmp(argv[1], "--compact") == 0) || (strcmp(argv[1], "-c") == 0)) {
      compact(FILELIST_IN);
   } else {
      if (((argc != 7) && (argc != 8)) || ((strcmp(argv[1], "-l") != 0) || (strcmp(argv[3], "-h") != 0) || (strcmp(argv[5], "-p") != 0)) || ((argc == 8) && (strcmp(argv[7], "--high-security") != 0))) {
         printf("Syntax error. Try " PROGRAM " --help.\n\n");
         exit(1);
      }
      if (((atoi(argv[2]) < 1) || (atoi(argv[2]) > 65535)) || ((atoi(argv[6]) < 1) || (atoi(argv[6]) > 65535))) {
         printf("Invalid port. Range is 1...65535.\n\n");
         exit(1);
      }
      if (argc == 7) {
         wgconnect(atoi(argv[2]), argv[4], atoi(argv[6]), 2);
      } else {
         wgconnect(atoi(argv[2]), argv[4], atoi(argv[6]), 3);
      }
   }
   exit(0);	
}

void usage()
{
	// Just print out usage

	printf("   usage:   " PROGRAM " -l LOCAL_PORT -h REMOTE_HOST -p REMOTE_PORT [--high-security]\n");
	printf("\t    " PROGRAM " --help | --verify | --compact\n\n");
	printf("   Command line parameters:\n\n");
	printf(BOLD "   -l LOCAL_PORT" FLAT "\t\tPort number for listening.\n");
	printf(BOLD "   -h REMOTE_HOST" FLAT "\t\tRemote host to connect.\n"); 
	printf(BOLD "   -p REMOTE_PORT" FLAT "\t\tRemote port to connect.\n"); 
	printf(BOLD "   \t--high-security" FLAT "\t\tUse three WinGates to bounce instead of two.\n\n"); 
	printf(BOLD "   \t--help" FLAT "\t\t\tShow this help and exit.\n");
	printf(BOLD "   -v,\t--verify" FLAT "\t\tVerify WinGates addresses in " FILELIST_IN ".\n");
        printf(BOLD "   -c,\t--compact" FLAT "\t\tRemove duplicate addresses in " FILELIST_IN ".\n");
	printf("\n");
}

void verify()
{
	// Verify WinGates list

	int i = 0, goodwg = 0, allwg = 0;
	char wingate_addr[0xFF], line[256], *s;
	FILE *list_in, *list_out;
	long fpos;

	// Open source file for read
	if ((list_in = fopen(FILELIST_IN, "r")) == NULL) {
		printf(PROGRAM ": Unable to read " FILELIST_IN "\n");
		exit(1);
	}

	// Open destination file for appending good addresses
	if ((list_out = fopen(FILELIST_OUT, "a")) == NULL) {
                printf(PROGRAM ": Unable to write " FILELIST_OUT "\n");
                exit(1);
	}
	fpos = ftell(list_out);

	// Try each address in list
	do {
		i = fscanf(list_in, "%s", wingate_addr);
		if (i != EOF) {
			printf("Trying %s... ", wingate_addr);
			fflush(stdout);
			if (vrfywgate(wingate_addr) == 1) {
				goodwg++;
				fprintf(list_out, "%s\n", wingate_addr);
				fflush(list_out);
			}
			allwg++; 
		}
	} while (i != EOF);
	fclose(list_in);
	fclose(list_out);
	
	// View results
	list_out = fopen(FILELIST_OUT, "r");
	fseek(list_out, fpos, SEEK_SET);

	printf("\n[ -> " BOLD "%d" FLAT " of %d] WinGates found (added in " FILELIST_OUT "):\n", goodwg, allwg);
	do {
		s = fgets(line, 255, list_out);
		if (s != NULL) { printf("\t%s", line); }
} while (s != NULL);
	printf("\n");
	compact(FILELIST_OUT);
	fclose(list_out);

	exit(0);
}

int vrfywgate(char *wingate_addr)
{
	// This funcion will return 0 if WinGate doesn't work, 1 otherwise

	int sock, connection, nb, i;
	struct sockaddr_in saddr;
	char data[0xFF], wingate_str[0xFF];

	int retval;
	fd_set wfds;
	struct timeval tv;

	// Create the socket
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf(PROGRAM ": socket() error.\n");
		close(sock);
		return(0);
	}

	// Set the non-blocking mode
	if ((nb = fcntl(sock, F_SETFL, O_NONBLOCK)) == -1) {
		printf(PROGRAM ": fcntl() error.\n");
		close(sock);
		return(0);
	}

	// Prepare sockaddr_in struct
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(PORT);
	if ((saddr.sin_addr.s_addr = resolv(wingate_addr)) == 0) {
		printf("\n" PROGRAM ": gethostbyname() error.\n");
		close(sock);
		return(0);
	}

	// Set timeout for select()
	tv.tv_sec = TIMEOUT;
	tv.tv_usec = 0;

	// Try to connect to WinGate
	connection = connect(sock, &saddr, sizeof(saddr));

	FD_ZERO(&wfds);
	FD_SET(sock, &wfds);

	// Waiting for connect() to conclude
	retval = select(sock + 1, NULL, &wfds, NULL, &tv);
	if (retval == 0) {
		printf("connection timed out :(\n");
		close(sock);
		return(0);
	}

	// Reget the right errno code
	connection = connect(sock, &saddr, sizeof(saddr));

	if ((errno == ENETUNREACH) || (errno == ENETDOWN) || (errno == ECONNREFUSED) || (errno == EHOSTUNREACH) || (errno == EHOSTDOWN)) {
		printf("unable to connect :(\n");
		close(sock);
		return(0);
	}

	printf(BOLD "connected" FLAT ". Hmm does WinGate work? ");
	fflush(stdout);

	// Wait for data
	sleep(TIMEOUT);
	memset(data, 0, 256);
	recv(sock, data, 256, 0);

	// Check if it's really a WinGate
	strcpy(wingate_str, "WinGate>");
	for(i = 9; i <= 16; i++) {
		if(data[i] != wingate_str[i - 9]) {
			printf("nope :(\n");
			close(sock);
			return(0);
		}
	}
	printf(BOLD "weee ;)\n" FLAT);
	close(sock);
	return(1);
}

void compact(char *filename)
{
	// Find and delete duplicate addresses in a file

	char *i, *j, addr_in[256], addr_out[256];
	FILE *list_in, *list_out;

	printf("Compacting duplicates in %s... ", filename);

	// Open files for reading and writing results
	list_in = fopen(filename, "r");
	list_out = fopen(FILELIST_SWP, "w+");

	// Compare each address in the list with these in the result list
	do {
		i = fgets(addr_in, 255, list_in);
		if (i != NULL) {
			do {
				j = fgets(addr_out, 255, list_out);
				if (j != NULL) {	
					if (strcmp(addr_in, addr_out) == 0) { 
						goto escape;
					}
				}
			} while (j != NULL);
			fprintf(list_out, "%s", addr_in);
			fflush(list_out);
			escape:
			rewind(list_out);
		}
        } while (i != NULL);

	fclose(list_in);
	fclose(list_out);

	// Overwrite the filelist with the swap containing the results
	rename(FILELIST_SWP, filename);

	printf("done.\n\n");
	return;
}

u_long resolv(char *hostname) {
	// Resolve hostnames to IPv4

	u_long ipb;
	struct hostent *hp;

	if((hp = gethostbyname(hostname)) == NULL) {
		return(0);
	}
	memcpy(&ipb, hp->h_addr, hp->h_length);
	return(ipb);
}

void wgconnect(int l_port, char *r_host, int r_port, int secure)
{
	// Start bouncing :)

	int i = 0, j = 0, naddr[3];
	char *s, wg1[256], wg2[256] , wg3[256];
	char connstr[256], buf[4096], port[6];
	float rndnum;
	FILE *fp;
	int sock, connection, servsock, servbind, servlisten, clientsock;
	struct sockaddr_in saddr, servaddr, clientaddr;
	int clientaddrlen = sizeof(clientaddr);
	fd_set fdsr, fdse;

	// Display connection info
	printf("\tLocal port = " BOLD "%d" FLAT "\n", l_port);
	printf("\tRemote host = " BOLD "%s" FLAT "\n", r_host);
	printf("\tRemote port = " BOLD "%d" FLAT "\n", r_port);
	printf("\tWinGates to bounce = " BOLD "%d" FLAT "\n\n", secure);

	// Select random WinGates
	if ((fp = fopen(FILELIST_OUT, "r")) == NULL) {
		printf(PROGRAM ": unable to open " FILELIST_OUT ".\n");
		exit(1);
	}

	do {
		s = fgets(wg1, 255, fp);
		if (s != NULL) {
			i++;
		}
	} while (s != NULL);

	if (i < secure) {
		printf(PROGRAM ": not enought WinGates in " FILELIST_OUT ".\n");
		exit(1);
	}

	printf("Selecting WinGate addresses... ");
	fflush(stdout);

	// Generate random numbers
	srand(time (NULL));

	rndnum = rand();
	rndnum = ((int) (rndnum / RAND_MAX * i));
	naddr[0] = ((int) rndnum) + 1;
		
	do {
		srand(time (NULL));

		rndnum = rand();
		rndnum = ((int) (rndnum / RAND_MAX * i));
		naddr[1] = ((int) rndnum) + 1;
	} while (naddr[0] == naddr[1]);

	if (secure == 3) {
		do {
			srand(time (NULL));

			rndnum = rand();
			rndnum = ((int) (rndnum / RAND_MAX * i));
			naddr[2] = ((int) rndnum) + 1;
		} while ((naddr[2] == naddr[0]) || (naddr[2] == naddr[1]));
	} else {
		naddr[2] = 0;
	}

	rewind(fp);
	for (j = 0; j < naddr[0]; j++) {
		i = fscanf(fp, "%s", wg1);
	}
	
	rewind(fp);
	for (j = 0; j < naddr[1]; j++) {
		i = fscanf(fp, "%s", wg2);
	}

	if (secure == 3) {
		rewind(fp);
		for (j = 0; j < naddr[2]; j++) {
			i = fscanf(fp, "%s", wg3);
		}
	}
	
	fclose(fp);
	printf("done.\n");

	// Create socket for connection
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf(PROGRAM ": socket() error.\n");
		close(sock);
		exit(1);
	}

	// Prepare sockaddr_in struct
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(23);
	if ((saddr.sin_addr.s_addr = resolv(wg1)) == 0) {
		printf(PROGRAM ": gethostbyname() error.\n");
		close(sock);
		exit(1);
	}

	// Connect tho the first WinGate address
	printf("Connecting (#1) to [" BOLD "%s" FLAT "]... ", wg1);
	fflush(stdout);
	connection = connect(sock, &saddr, sizeof(saddr));

        if ((errno == ENETUNREACH) || (errno == ENETDOWN) || (errno == ECONNREFUSED) || (errno == EHOSTUNREACH) || (errno == EHOSTDOWN)) {
                printf("unable to connect :(\n");
                close(sock);
                exit(1);
        }

	printf(BOLD "connected" FLAT ".\n");

	// Bounce through WinGates
	for (j = 2; j <= secure; j++) {
		printf("Bouncing   (#%d) to [" BOLD "%s" FLAT "]... ", j, wg2);
	        fflush(stdout);
		send(sock, wg2, strlen(wg2), 0);
		send(sock, "\n", strlen("\n"), 0);

		// Wait the connection to succeed
		i = 0;
		while (i < 2) {
			bzero(connstr, 256);
			recv(sock, connstr, 256, 0);
			if (strstr(connstr, "Connected") != NULL) {
				i = 1;
			} else if (((strstr(connstr, "WinGate>") != NULL) && (i == 1)) || ((strstr(connstr, "Connected") != NULL) && (strstr(connstr, "WinGate>") != NULL))) {
				i = 2;
			}
		}
		printf(BOLD "connected" FLAT ".\n");
		fflush(stdout);
		bzero(wg2, 256);
		strcpy(wg2, wg3);
	}

	// Finally connect to target host
	printf("\nConnecting to [" BOLD "%s" FLAT " - port: " BOLD "%d" FLAT "]... ", r_host, r_port);
	fflush(stdout);
        send(sock, r_host, strlen(r_host), 0);
	send(sock, " ", strlen(" "), 0);
	bzero(port, 6);
	sprintf(port, "%i", r_port);
	send(sock, port, strlen(port), 0);
        send(sock, "\n", strlen("\n"), 0);

	// Wait for connection to target
	i = 0;
	while (i < 1) {
		bzero(connstr, 256);
		recv(sock, connstr, 256, 0);
		if (strstr(connstr, "Connected") != NULL) {
			i = 1;
                	printf(BOLD "connected" FLAT ".\n");
                }
                fflush(stdout);
	}

	// Create the sock for the server side of datapipe
	if ((servsock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf(PROGRAM ": socket() error.\n");
		close(servsock);
		exit(1);
	}

	// Prepare sockaddr_in structure
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(l_port);
	servaddr.sin_addr.s_addr = htonl(0);

	// Bind on a local port
	if ((servbind = bind(servsock, &servaddr, sizeof(servaddr))) < 0) {
		printf(PROGRAM ": bind() error.\n");
		close(servsock);
		close(sock);
		exit(1);
	}

	// listen for an incoming connection
	if ((servlisten = listen(servsock, 1)) < 0) {
		printf(PROGRAM ": listen() error.\n");
		close(servsock);
		close(sock);
		exit(1);
	}

	// Fork and go in the background
	printf("Listening for incoming connections on port: " BOLD "%d" FLAT ".\n\n", l_port);
	printf("\t\tlocalhost:%d " BOLD "->" FLAT " %s:%d\n\n", l_port, r_host, r_port);
	printf("Going in the background...\n\n");
	i = fork();
	if (i != 0) {
		exit(0);
	}

	// Accept a connection
	clientsock = accept(servsock, &clientaddr, &clientaddrlen);

	// Datapipe code
	// This is my interpretation of a piece of code from datapipe.c
	// coded by Todd Vierling
	while (1) {
		FD_ZERO(&fdsr);
		FD_ZERO(&fdse);
		FD_SET(clientsock,&fdsr);
		FD_SET(clientsock,&fdse);
		FD_SET(sock,&fdsr);
		FD_SET(sock,&fdse);
		if (select(20, &fdsr, NULL, &fdse, NULL) == -1) {
			printf(PROGRAM ": select() error.\n");
			close(clientsock);
			close(servsock);
			close(sock);
			exit(1);
		}
		if (FD_ISSET(clientsock, &fdsr) || FD_ISSET(clientsock, &fdse)) {
			if ((i = read(clientsock, buf, 4096)) <= 0) {
				close(clientsock);
				close(servsock);
				close(sock);
				exit(1);
			}
			if ((write(sock, buf, i)) <= 0) {
				close(clientsock);
				close(servsock);
				close(sock);
				exit(1);
			}
		} else if (FD_ISSET(sock, &fdsr) || FD_ISSET(sock, &fdse)) {
			if ((i = read(sock, buf, 4096)) <= 0) {
				close(clientsock);
				close(servsock);
				close(sock);
				exit(1);
			}
			if ((write(clientsock, buf, i)) <= 0) {
				close(clientsock);
				close(servsock);
				close(sock);
				exit(1);
			}
		}
	}
}

/*				EOF wgc.c			*/
