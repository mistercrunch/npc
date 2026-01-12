
 /*** FASTREAM suite ********************************************
 *         __  __       _ _        _  __              _         *
 *        |  \/  |_   _| | |_ ___ | |/ /_ __ __ _ ___| |_       *
 *        | |\/| | | | | | __/ _ \| ' /| '__/ _` / __| __|      *
 *        | |  | | |_| | | || (_) | . \| | | (_| \__ \ |_       *
 *        |_|  |_|\__,_|_|\__\___/|_|\_\_|  \__,_|___/\__|      *
 *                                                              *
 *            tHE rECIdjVO - <recidjvo@s0ftpj.org>              *
 *              (unerasable thanks to nextie :>)                *
 *                                                              *
 *               *** DEVEL/UNTESTED/UNSTABLE ***                *
 *                                                              *
 ******************************************** fastboys, 2002 ***/

#ifdef FASTREAM_DEBUG
        #define debug(x...) fprintf(stderr, x)
#else
        #define debug(x...)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char *argv[]);
void scanexit(int signaln);

int main(int argc, char *argv[])
{
        int sock;
        struct sockaddr_in saddr;
        struct ip_mreq mr;
        int dlen, slen = sizeof(struct sockaddr_in);
        char buf[(16 * 1024)];

        debug("MultoKrast devel-version, fastboys 2002.\n");

        if(argc < 3) {
                debug("usage: %s <multicast_ip> <port> [scan_time]\n", argv[0]);
                exit(-1);
        }

        if(!IN_MULTICAST(ntohl(inet_addr(argv[1])))) {
                debug("usage: %s <multicast_ip> <port>\n", argv[0]);
                exit(-1);
        }

        if((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP)) < 0) {
                debug("socket()\n");
                exit(-1);
        }
        saddr.sin_family = AF_INET;
        saddr.sin_addr.s_addr = inet_addr(argv[1]);
        saddr.sin_port = htons(atoi(argv[2]));
        if(bind(sock, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
                debug("bind()\n");
                exit(-1);
        }

        mr.imr_multiaddr = saddr.sin_addr;
        mr.imr_interface.s_addr = htonl(INADDR_ANY);
        if(setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *)&mr, sizeof(mr)) < 0) {
                debug("setsockopt()\n");
                exit(-1);
        }

        if(argc == 4) {
                signal(SIGALRM, scanexit);
                alarm(atoi(argv[3]));
                debug("Scanning mode: %s %s for %d seconds.     ", argv[1], argv[2], atoi(argv[3]));
        }

        while(1) {
                if((dlen = recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr *)&saddr, &slen)) <= 0) {
                        debug("recvfrom()\n");
                        exit(-1);
                }
                debug("\b\b\b\bDATA");
                write(STDOUT_FILENO, buf, dlen);
        }

        debug("\n");

        exit(0);
}

void scanexit(int signaln)
{
        debug("\n");
        exit(0);
}

/* EOF */
