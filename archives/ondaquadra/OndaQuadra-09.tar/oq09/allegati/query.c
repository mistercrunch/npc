/*
 * Query Guessing
 * by eazy, eazy@ondaquadra.org
 *
 * This program analize the idle of an IRC user and try to guess
 * for possible query with other user. The query prediction isn't
 * alway true, anyway try it and have fun :)
 */

#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 6667
#define USER "USER prolipol 192.168.1.4 irc.azzurra.org :telnet\n"
#define NICK "NICK prolipol\n"
#define TIMEOUT 5
#define IDLE_THRESHOLD 120
#define TRUST 12
#define N 10

typedef struct nick {
	char           *nickname;
	long            idle;
	int             trust;
	struct nick    *next;
}               nick;

typedef struct chan {
	char           *channel;
	nick           *lnick;
	struct chan    *next;
}               chan;

int             sock_global, count = 0;
chan           *list_global, *update_global = NULL;
char           *nick_global;

ssize_t 
writen(int fd, const void *buf, size_t count)
{

	if (write(fd, buf, count) < 0) {
		printf("write error\n");
		exit(0);
	}
}

/*
 * Send a PONG message as reply to server PING
 *
 * From Request for Comments: 2812
 *
 * The PING command is used to test the presence of an active client or
 * server at the other end of the connection.  Servers send a PING
 * message at regular intervals if no other activity detected coming
 * from a connection.  If a connection fails to respond to a PING
 * message within a set amount of time, that connection is closed.  A
 * PING message MAY be sent even if the connection is active.
 *
 * When a PING message is received, the appropriate PONG message MUST be
 * sent as reply to <server1> (server which sent the PING message out)
 * as soon as possible.  If the <server2> parameter is specified, it
 * represents the target of the ping, and the message gets forwarded
 * there.
 */

int 
ping_pong(int sock, char *buff)
{

	char            temp[5];
	char            reply[100];

	strncpy(temp, buff, sizeof(char) * 4);
	temp[4] = 0;
	if (strcmp(temp, "PING") == 0) {
		strncpy(reply, "PONG", sizeof(reply));
		strncat(reply, buff + 4, sizeof(reply) - strlen(reply) - 1);
		writen(sock, reply, strlen(reply));
		return (1);
	}
	return (0);

}

/*
 * Buid a list of the nick in the channel from the
 * RPL_NAMREPLY message received from the server
 *
 * From Request for Comments: 2812
 *
 * 353    RPL_NAMREPLY
 *        "( "=" / "*" / "@" ) <channel>
 *         :[ "@" / "+" ] <nick> *( " " [ "@" / "+" ] <nick> )
 *
 * - "@" is used for secret channels, "*" for private
 *   channels, and "=" for others (public channels).
 *
 * - To reply to a NAMES message, a reply pair consisting
 *   of RPL_NAMREPLY and RPL_ENDOFNAMES is sent by the
 *   server back to the client.  If there is no channel
 *   found as in the query, then only RPL_ENDOFNAMES is
 *   returned.  The exception to this is when a NAMES
 *   message is sent with no parameters and all visible
 *   channels and contents are sent back in a series of
 *   RPL_NAMEREPLY messages with a RPL_ENDOFNAMES to mark
 *   the end.
 */

int 
nick_list(char *buff, chan * list)
{

	int             flag, first, exit = 0;
	char           *s, *temp, *token, *channel, *next;
	char          **p = &next;
	chan           *index;
	nick           *index_nick, *index_nick_old;

	flag = 0;
	s = (char *) calloc(strlen(buff) + 1, sizeof(char));
	temp = (char *) calloc(strlen(buff) + 1, sizeof(char));
	strcpy(temp, buff);

	/*
         * Parse the input string to verify if it is a RPL_NAMREPLY
         */
	if ((token = (char *) strtok_r(temp, " ", p)) != NULL) {
		if (strcmp(token, "353") == 0) {
			strcpy(s, *p);
			flag = 1;
		}
		while (flag == 0 && (token = (char *) strtok_r(NULL, " ", p)) != NULL) {
			if (strcmp(token, "353") == 0) {
				strcpy(s, *p);
				flag = 1;
			}
		}
	}
	/*
         * If the parsed string is a RPL_NAMREPLY, remove useless token
         * from it
         */
	if (flag == 1 && (char *) strtok(s, " ") != NULL) {
		if ((char *) strtok(NULL, " ") == NULL) {
			free(s);
			free(temp);
			return (0);
		}
		/*
	         * Save the channel name from the parsed string and search for it
	         * in the channel list already build by channel_sampler()
	         */
		if ((channel = (char *) strtok(NULL, " ")) == NULL) {
			free(s);
			free(temp);
			return (0);
		}
		index = list;
		while (index != NULL && strcmp(channel, index->channel) != 0)
			index = index->next;
		if (index == NULL) {
			free(s);
			free(temp);
			return (0);
		}
		first = 1;
		index_nick_old = index->lnick;

		/*
	         * Build a list of nick in the channel. The while loop end when
	         * strtok() return NULL or at the first token that begin or end
	         * with a line feed or carriage return
	         */
		while (((token = (char *) strtok(NULL, " ")) != NULL) && (token[0] != '\n' && token[0] != '\r') && (exit == 0)) {
			if (first) {
				index->lnick = (nick *) malloc(sizeof(nick));

				/*
			         * Remove op and voice symbols from the nickname
			         */
				if (token[0] == ':')
					token++;
				if (token[0] == '@' || token[0] == '+')
					token++;

				/*
			         * If the token end with a line feed or carriage return remove
			         * the last character and break the while loop
			         */
				if (token[strlen(token) - 1] == '\n' || token[strlen(token) - 1] == '\r') {
					token[strlen(token) - 1] = 0;
					exit = 1;
				}
				index->lnick->nickname = (char *) calloc(strlen(token) + 1, sizeof(char));
				strcpy(index->lnick->nickname, token);
				//use strncpy() instead
					index->lnick->idle = 0;
				index->lnick->trust = 0;
				index->lnick->next = NULL;
				index_nick = index->lnick;
				first = 0;
			} else {
				index_nick->next = (nick *) malloc(sizeof(nick));
				if (token[0] == ':')
					token++;
				if (token[0] == '@' || token[0] == '+')
					token++;
				if (token[strlen(token) - 1] == '\n' || token[strlen(token) - 1] == '\r') {
					token[strlen(token) - 1] = 0;
					exit = 1;
				}
				index_nick->next->nickname = (char *) calloc(strlen(token) + 1, sizeof(char));
				strcpy(index_nick->next->nickname, token);
				//use strncpy() instead
					index_nick->next->idle = 0;
				index_nick->next->trust = 0;
				index_nick->next->next = NULL;
				index_nick = index_nick->next;
			}
		}
		if (index->lnick != NULL) {

			/*
		         * Copy in the new nick list the values of idle and trust from
		         * the old one
		         */
			if (index_nick_old != NULL) {
				while (index_nick_old != NULL) {
					index_nick = index->lnick;
					while (index_nick != NULL) {
						if (strcmp(index_nick_old->nickname, index_nick->nickname) == 0) {
							index_nick->idle = index_nick_old->idle;
							index_nick->trust = index_nick_old->trust;
						}
						index_nick = index_nick->next;
					}
					index_nick_old = index_nick_old->next;
				}
			}
			free(s);
			free(temp);
			return (1);
		}
	}
	free(s);
	free(temp);
	return (0);
}

/*
 * Parse and sample the idle contained in the RPL_WHOISIDLE
 * message received in responce of the WHOIS issued by
 * idle_prober() function
 *
 * From Request for Comments: 2812
 *
 * 317    RPL_WHOISIDLE
 *        "<nick> <integer> :seconds idle"
 *
 * - Replies 311 - 313, 317 - 319 are all replies
 *   generated in response to a WHOIS message.
 */

int 
idle_sampler(char *buff, nick * list)
{

	int             flag;
	char           *s, *temp, *token, *nickname, *idle, *next;
	char          **p = &next;
	nick           *index;

	flag = 0;
	s = (char *) calloc(strlen(buff) + 1, sizeof(char));
	temp = (char *) calloc(strlen(buff) + 1, sizeof(char));
	strcpy(temp, buff);

	/*
         * Parse the input string to verify if it is a valid
         * RPL_WHOISIDLE message
         */
	if ((token = (char *) strtok_r(temp, " ", p)) != NULL) {
		if (strcmp(token, "317") == 0) {
			strcpy(s, *p);
			flag = 1;
		}
		while (flag == 0 && (token = (char *) strtok_r(NULL, " ", p)) != NULL) {
			if (strcmp(token, "317") == 0) {
				strcpy(s, *p);
				flag = 1;
			}
		}
	}
	/*
         * If the parsed string is a RPL_WHOISIDLE message, remove
         * useless token from it
         */
	if (flag == 1 && (char *) strtok(s, " ") != NULL) {

		/*
	         * Save the nickname from the parsed string and search for it
	         * in the nick list already build by nick_list()
	         */
		if ((nickname = (char *) strtok(NULL, " ")) == NULL) {
			free(s);
			free(temp);
			return (0);
		}
		index = list;
		while ((index != NULL) && (strcmp(nickname, index->nickname) != 0))
			index = index->next;
		if (index == NULL) {
			free(s);
			free(temp);
			return (0);
		} else {

			/*
		         * If the nickname contained in the RPL_WHOISIDLE is found in
		         * the nick list, the value of the idle is stored in the nick
		         * struct
		         */
			if ((idle = (char *) strtok(NULL, " ")) == NULL) {
				free(s);
				free(temp);
				return (0);
			}
			index->idle = atol(idle);
		}
		free(s);
		free(temp);
		return (1);
	}
	free(s);
	free(temp);
	return (0);
}

/*
 * Build a list of the channel that a specific nickname has joined.
 * The list is derived from the RPL_WHOISCHANNELS message received
 * from the server in response to a WHOIS message for that nickname
 *
 * From Request for Comments: 2812
 *
 * 319    RPL_WHOISCHANNELS
 *        "<nick> :*( ( "@" / "+" ) <channel> " " )"
 *
 * - Replies 311 - 313, 317 - 319 are all replies
 *   generated in response to a WHOIS message.  Given that
 *   there are enough parameters present, the answering
 *   server MUST either formulate a reply out of the above
 *   numerics (if the query nick is found) or return an
 *   error reply.  The '*' in RPL_WHOISUSER is there as
 *   the literal character and not as a wild card.  For
 *   each reply set, only RPL_WHOISCHANNELS may appear
 *   more than once (for long lists of channel names).
 *   The '@' and '+' characters next to the channel name
 *   indicate whether a client is a channel operator or
 *   has been granted permission to speak on a moderated
 *   channel.  The RPL_ENDOFWHOIS reply is used to mark
 *   the end of processing a WHOIS message.
 */

int 
channel_sampler(char *buff, chan ** list, char *nick)
{

	int             flag, first = 1, exit = 0;
	char           *s, *temp, *token, *nickname, *next;
	char          **p = &next;
	chan           *index;

	flag = 0;
	s = (char *) calloc(strlen(buff) + 1, sizeof(char));
	temp = (char *) calloc(strlen(buff) + 1, sizeof(char));
	strcpy(temp, buff);

	/*
         * Parse the input string and verify if it is a valid
         * RPL_WHOISCHANNELS message
         */
	if ((token = (char *) strtok_r(temp, " ", p)) != NULL) {
		if (strcmp(token, "319") == 0) {
			strcpy(s, *p);
			flag = 1;
		}
		while (flag == 0 && (token = (char *) strtok_r(NULL, " ", p)) != NULL) {
			if (strcmp(token, "319") == 0) {
				strcpy(s, *p);
				flag = 1;
			}
		}
	}
	/*
         * If the parsed string is a RPL_WHOISCHANNELS message, remove the
         * useless token and save the nickname which the reply refer to.
         * After that, compare the nickname saved from the string with the
         * one supplied as argv of the main()
         */
	if (flag == 1 && (char *) strtok(s, " ") != NULL) {
		if ((nickname = (char *) strtok(NULL, " ")) == NULL) {
			free(s);
			free(temp);
			return (0);
		}
		if (strcmp(nickname, nick) == 0) {

			/*
		         * Build a list of the channel that the supplied nickname has
		         * joined. If the strcmp() in the previous statment return 0,
		         * the while loop parse the RPL_WHOISCHANNELS message until
		         * strtok() return NULL or return a token that begin or end
		         * with a line feed or carriage return
		         */
			while (((token = (char *) strtok(NULL, " ")) != NULL) && (token[0] != '\n' && token[0] != '\r') && (exit == 0)) {
				if (first) {
					*list = (chan *) malloc(sizeof(chan));

					/*
			                 * Remove op and voice symbols from the nickname
			                 */
					if (token[0] == ':')
						token++;
					if (token[0] == '@' || token[0] == '+')
						token++;

					/*
			                 * If the token end with a line feed or carriage return remove
			                 * the last character and break the while loop
			                 */
					if (token[strlen(token) - 1] == '\n' || token[strlen(token) - 1] == '\r') {
						token[strlen(token) - 1] = 0;
						exit = 1;
					}
					(*list)->channel = (char *) calloc(strlen(token) + 1, sizeof(char));
					strcpy((*list)->channel, token);
					//use strncpy() instead
						(*list)->lnick = NULL;
					(*list)->next = NULL;
					index = *list;
					first = 0;
				} else {
					index->next = (chan *) malloc(sizeof(nick));
					if (token[0] == ':')
						token++;
					if (token[0] == '@' || token[0] == '+')
						token++;
					if (token[strlen(token) - 1] == '\n' || token[strlen(token) - 1] == '\r') {
						token[strlen(token) - 1] = 0;
						exit = 1;
					}
					index->next->channel = (char *) calloc(strlen(token) + 1, sizeof(char));
					strcpy(index->next->channel, token);
					//use strncpy() instead
						index->next->lnick = NULL;
					index->next->next = NULL;
					index = index->next;
				}
			}
			if (*list != NULL) {
				free(s);
				free(temp);
				return (1);
			}
		}
	}
	free(s);
	free(temp);
	return (0);
}

/*
 * For every nick in all channel issue a WHOIS that elicit a
 * RPL_WHOISIDLE message in response, the value of the idle
 * contained in this message will be parsed and stored in the
 * nick struct by the idle_sampler() function
 */

void 
idle_prober(int sock, chan * list)
{

	int             r;
	chan           *index = list;
	nick           *index_nick;
	char            buff[1000], probe[256];
	fd_set          readset;
	struct timeval  time;

	while (index != NULL) {
		index_nick = index->lnick;
		while (index_nick != NULL) {
			FD_ZERO(&readset);
			FD_SET(sock, &readset);
			time.tv_sec = TIMEOUT;
			time.tv_usec = 0;

			strncpy(probe, "WHOIS ", sizeof(probe));
			strncat(probe, index_nick->nickname, sizeof(probe) - strlen(probe) - 1);
			strncat(probe, " ", sizeof(probe) - strlen(probe) - 1);
			strncat(probe, index_nick->nickname, sizeof(probe) - strlen(probe) - 1);
			strncat(probe, "\n", sizeof(probe) - strlen(probe) - 1);
			writen(sock, probe, strlen(probe));

			/*
		         * Set a timeout on the I/O operation performed by read()
		         * It's possible to redefine TIMEOUT to modify the amount of
		         * time that select() must wait a response before return
		         */
			if (select(sock + 1, &readset, NULL, NULL, &time) > 0) {
				if ((r = read(sock, buff, sizeof(buff) - 1)) < 0) {
					printf("read error\n");
					exit(0);
				}
				buff[r] = 0;
			} else {
				buff[0] = 0;
				printf("read timeout\n");
			}

			/*
		         * Parse and sample the idle contained in the RPL_WHOISIDLE
		         * message received in responce of the WHOIS issued above
		         */
			if (idle_sampler(buff, index->lnick));
			else
				(ping_pong(sock, buff));
			reset_trust(buff, list, nick_global);
			index_nick = index_nick->next;
		}
		index = index->next;
	}
}

/*
 * Calculate and update channel statistics based on the values
 * sampled by idle_sampler()
 */

void 
idle_stat(chan * list, char *nickname)
{

	chan           *index = list;
	nick           *index_nick;
	long            nickname_idle, add_idle;

	while (index != NULL) {
		index_nick = index->lnick;

		/*
	         * Search in the nick list for the nickname supplied as argv
	         * of the main()
	         */
		while (index_nick != NULL && strcmp(nickname, index_nick->nickname) != 0)
			index_nick = index_nick->next;

		/*
	         * If the nickname supplied as argument in the command line is
	         * not found in the channel, set the idle for that nick to a
	         * dummy value. This cause the trust value of all the nick in
	         * the channel to be set to 0
	         */
		if (index_nick == NULL)
			nickname_idle = (IDLE_THRESHOLD * 2) + 1;
		else {

			/*
		         * Save the the idle of the nickname supplied as argument in
		         * the command line if its value is less than or equal to
		         * IDLE_THRESHOLD
		         */
			if (index_nick->idle <= IDLE_THRESHOLD)
				nickname_idle = index_nick->idle;

			/*
		         * Else, set the idle for that nick to a dummy value. This cause
		         * the trust value of all the nick in the channel to be set to 0
		         */
			else
				nickname_idle = (IDLE_THRESHOLD * 2) + 1;
		}
		index_nick = index->lnick;
		while (index_nick != NULL) {
			add_idle = nickname_idle + index_nick->idle;

			/*
		         * If the sum of the idle of the nickname specified from the
		         * command line and the one retrived from the nick list is bigger
		         * than IDLE_THRESHOLD * 2, then the trust value of the retrived
		         * nick will be set to 0
		         */
			if (add_idle > IDLE_THRESHOLD * 2) {
				index_nick->trust = 0;
				index_nick = index_nick->next;
				continue;
			}
			/*
		         * The trust for the nick in the channel will be increased
		         * proportionally to the value of the sum calcolated as described
		         * above
		         */
			else if (add_idle <= IDLE_THRESHOLD * 2 / 3)
				index_nick->trust += 3;
			else if (add_idle <= IDLE_THRESHOLD * 4 / 3)
				index_nick->trust += 2;
			else
				index_nick->trust++;
			index_nick = index_nick->next;
		}
		index = index->next;
	}
	index = list;
	while (index != NULL) {
		index_nick = index->lnick;
		while (index_nick != NULL) {
			printf("%s: %d %d\n", index_nick->nickname, index_nick->idle, index_nick->trust);
			index_nick = index_nick->next;
		}
		index = index->next;
	}
}

/*
 * Check channel statistics and prompt for possible query
 */

void 
idle_check(chan * list, char *nickname)
{

	chan           *index = list;
	nick           *index_nick;

	while (index != NULL) {
		index_nick = index->lnick;
		while (index_nick != NULL) {

			/*
		         * If the trust value of a nickname in the channel has reached
		         * the value specified by the TRUST constant, prompt the user
		         * for possible query between this user and the one specified
		         * as argument of the command line
		         */
			if (index_nick->trust >= TRUST && strcmp(nickname, index_nick->nickname) != 0)
				printf("possibile query con %s\n", index_nick->nickname);
			index_nick = index_nick->next;
		}
		index = index->next;
	}
	printf("\n");

}

/*
 * Reset trust value of nickname that speak in chennel
 */

int 
reset_trust(char *buff, chan * list, char *nickname)
{

	int             i = 0;
	char           *temp, *token;
	chan           *index;
	nick           *index_nick;

	temp = (char *) calloc(strlen(buff) + 1, sizeof(char));
	strcpy(temp, buff);

	/*
         * Parse and save the nickname that speak in channel
         */
	if ((token = (char *) strtok(temp, " ")) != NULL && token[0] == ':') {
		token++;
		while (token[i] != '!' && token[i] != 0)
			i++;
		if (token[i] == '!') {
			token[i] = 0;
			index = list;

			/*
		         * The trust value of the saved nickname will be reset
		         */
			if (strcmp(nickname, token) != 0) {
				while (index != NULL) {
					index_nick = index->lnick;
					while (index_nick != NULL && strcmp(token, index_nick->nickname) != 0)
						index_nick = index_nick->next;

					/*
			                 * To reset the nickname we assign a trust value of -3
			                 * because in the next idle probe its value will be
			                 * incremented and will reach the 0
			                 */
					if (index_nick != NULL)
						index_nick->trust = -3;
					index = index->next;
				}
			}
			/*
		         * If the saved nickname is the same of the nick supplied from
		         * the command line, the trust value of all nick in the channel
		         * will be reset
		         */
			else {
				while (index != NULL) {
					index_nick = index->lnick;
					while (index_nick != NULL) {
						index_nick->trust = -3;
						index_nick = index_nick->next;
					}
					index = index->next;
				}
			}
		}
	}
	free(temp);
}

/*
 * The signal handler function is called every time the timer
 * set by alert(), default 30 seconds, reach 0
 * The sig_handler() function call other function that probe
 * and sample idle value, update nick list, calculate statistics
 * and check for possible query
 */

static void 
sig_handler(int signo)
{

	if (count == N) {
		update_global = list_global;
		count = 0;
	}
	idle_prober(sock_global, list_global);
	idle_stat(list_global, nick_global);
	idle_check(list_global, nick_global);
	count++;
	if (alarm(30) != 0)
		printf("alarm was already set\n");

}

int 
main(int argc, char **argv)
{

	struct sockaddr_in servaddr;
	int             sock, r, first = 1;
	char            buff[1000];
	chan           *list = NULL, *join = NULL, *index;
	nick           *index_nick;
	struct sigaction act, oldact;

	act.sa_handler = sig_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	act.sa_flags |= SA_RESTART;
	if (sigaction(SIGALRM, &act, &oldact) < 0)
		printf("sigaction error\n");

	if (argc != 3) {
		printf("Usage: %s <ip address> <nickname>\n", argv[0]);
		exit(0);
	}
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("socket error\n");
		exit(0);
	}
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
		printf("inet_pton error\n");
		exit(0);
	}
	servaddr.sin_port = htons(PORT);

	if (connect(sock, (struct sockaddr *) & servaddr, sizeof(servaddr)) < 0) {
		printf("connect error\n");
		exit(0);
	}
	strncpy(buff, USER, sizeof(buff));
	writen(sock, buff, strlen(buff));

	strncpy(buff, NICK, sizeof(buff));
	writen(sock, buff, strlen(buff));

	buff[0] = 0;
	sleep(10);

	while (1) {

		/*
	         * Build a list of the channel that the nickname supplied
	         * from the command line has joined
	         */
		if (list == NULL) {
			while (!channel_sampler(buff, &list, argv[2])) {
				strncpy(buff, "WHOIS ", sizeof(buff));
				strncat(buff, argv[2], sizeof(buff) - strlen(buff) - 1);
				strncat(buff, "\n", sizeof(buff) - strlen(buff) - 1);
				writen(sock, buff, strlen(buff));
				if ((r = read(sock, buff, sizeof(buff) - 1)) < 0) {
					printf("read error\n");
					exit(0);
				}
			}
			list_global = list;
			//passa la lista al signal handler
				sock_global = sock;
			nick_global = argv[2];
			join = list;
		}
		/*
	         * Join all the channel in the list already build by
	         * channel_sampler() function
	         */
		if (join != NULL) {
			sleep(3);
			strncpy(buff, "JOIN ", sizeof(buff));
			strncat(buff, join->channel, sizeof(buff) - strlen(buff) - 1);
			strncat(buff, "\n", sizeof(buff) - strlen(buff) - 1);
			writen(sock, buff, strlen(buff));
			join = join->next;
		}
		/*
	         * Query the IRC server for the list of nick in all channels
	         * The response will be handled by nick_list() that update
	         * the nick list
	         */
		if (update_global != NULL) {
			sleep(3);
			strncpy(buff, "NAMES ", sizeof(buff) - 1);
			strncat(buff, update_global->channel, sizeof(buff) - strlen(buff) - 1);
			strncat(buff, "\n", sizeof(buff) - strlen(buff) - 1);
			writen(sock, buff, strlen(buff));
			update_global = update_global->next;
		}
		if ((r = read(sock, buff, sizeof(buff) - 1)) < 0) {
			printf("read error\n");
			exit(0);
		}
		buff[r] = 0;

		/*
	         * Buid or update the list of the nick in the channel from
	         * the RPL_NAMREPLY message received from the server
	         */
		if (nick_list(buff, list)) {
			if (first) {

				/*
			         * Set the first alarm() that elicit a SIGALRM signal
			         * handled by sig_handler()
			         */
				if (alarm(30) != 0)
					printf("alarm was already set\n");
				first = 0;
			}
			index = list;
			while (index != NULL) {
				index_nick = index->lnick;
				while (index_nick != NULL) {
					printf("%s\n", index_nick->nickname);
					index_nick = index_nick->next;
				}
				index = index->next;
			}
		} else
			ping_pong(sock, buff);
		reset_trust(buff, list, argv[2]);
	}

}
