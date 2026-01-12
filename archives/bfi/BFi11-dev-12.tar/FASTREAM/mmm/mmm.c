
 /*** FASTREAM suite ********************************************
 *                                                              *
 *                  _ __ ___  _ __ ___  _ __ ___                *
 *                 | '_ ` _ \| '_ ` _ \| '_ ` _ \               *
 *                 | | | | | | | | | | | | | | | |              *
 *                 |_| |_| |_|_| |_| |_|_| |_| |_|              *
 *                                                              *
 *            tHE rECIdjVO - <recidjvo@s0ftpj.org>              *
 *                                                              *
 *               *** DEVEL/UNTESTED/UNSTABLE ***                *
 *                                                              *
 ******************************************** fastboys, 2002 ***/

#define VIDEOSTATION_AUTHCODE "WebDVD=STBsn=KR0000000000&MAC=00:00:00:00:00:00&IP=0.0.0.0&HR=HR0000000000000"
#define MULTI_HEADERS_FILE "/usr/local/etc/multicast"
#define EXEC_MULTI execl("/usr/local/bin/MultoKrast", "MultoKrast", host, cport, NULL)

#ifdef FASTREAM_DEBUG
        #define debug(x...) fprintf(stderr, x)
#else
        #define debug(x...)
#endif

#define HTTP_PROTO "http://"
#define HTTP_PORT 80
#define HEADER_AGENT "User-Agent: Mozilla/4.78 [en] (X11; U; Linux.2.4.12.i586; Nav)\n"
#define HEADER_AGENT_KEYLEN 12
#define HEADER_KEEPALIVE "Connection: Keep-Alive"
#define HEADER_COOKIE "Cookie: " VIDEOSTATION_AUTHCODE
#define HEADER_COOKIE_KEYLEN 8

#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(void);
void multicast_headers(void);
int resolv4(char *hostname, struct sockaddr_in *saddr);

int main()
{
        char firstline[1024], newline[1024], *host = (char *)NULL, *path = (char *)NULL, cport[6];
        short unsigned int port;

        int sock;
        struct sockaddr_in saddr;

        fgets(firstline, sizeof(firstline), stdin);

        if(strchr(firstline, ' ') == (char *)NULL) {
                debug("Only http proxy support.\n");
                exit(-1);
        }
        host = (char *)((int)strchr(firstline, ' ') + 1);
        if(strncasecmp(host, HTTP_PROTO, strlen(HTTP_PROTO))) {
                debug("Only http proxy support.\n");
                exit(-1);
        }
        host = strtok((char *)((int)host + strlen(HTTP_PROTO)), "/");
        if(strchr(host, ':') == (char *)NULL) {
                port = (u_short)HTTP_PORT;
        } else {
                port = (u_short)atoi((char *)((int)strchr(host, ':') + 1));
        }
        path = (char *)((int)host + strlen(host) + 1);
        host = strtok(host, ":");
        snprintf(newline, sizeof(newline), "%s /%s", strtok(firstline, " "), path);

        saddr.sin_family = AF_INET;
        saddr.sin_port = htons(port);
        if(resolv4(host, &saddr)) {
                debug("Unable to resolve %s\n", host);
                exit(-1);
        }

        if(IN_MULTICAST(ntohl(saddr.sin_addr.s_addr))) {
                debug("Multicast proxy request detected.\n");
                multicast_headers();
                sleep(1);
                snprintf(cport, sizeof(cport), "%d", port);
                EXEC_MULTI;
                exit(0);
        }

        debug("Unicast proxy request detected.\n");
        if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
                debug("socket()\n");
                exit(-1);
        }

        if(connect(sock, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
                debug("connect()\n");
                close(sock);
                exit(-1);
        }

        if(send(sock, newline, strlen(newline), 0) < 0) {
                debug("write()\n");
                close(sock);
                exit(-1);
        }

        port = 1;
        while(fgets(firstline, sizeof(firstline), stdin) != (char *)NULL) {
                if(!strncasecmp(firstline, HEADER_AGENT, HEADER_AGENT_KEYLEN)) {
                        strncpy(firstline, HEADER_AGENT, sizeof(firstline));
                } else if(!strncasecmp(firstline, HEADER_KEEPALIVE, strlen(HEADER_KEEPALIVE))) {
                        continue;
                } else if(!strncasecmp(firstline, HEADER_COOKIE, HEADER_COOKIE_KEYLEN)) {
                        send(sock, HEADER_COOKIE, strlen(HEADER_COOKIE), 0);
                        send(sock, ";", strlen(";"), 0);
                        send(sock, strstr(firstline, " "), strlen(strstr(firstline, " ")), 0);
                        port = 0;
                        continue;
                }

                if(!strcmp(firstline, "\r\n")) {
                        if(port) {
                                send(sock, HEADER_COOKIE, strlen(HEADER_COOKIE), 0);
                                send(sock, "\n", strlen("\n"), 0);
                        }

                        send(sock, firstline, strlen(firstline), 0);
                        break;
                }
                send(sock, firstline, strlen(firstline), 0);
        }

        while((port = read(sock, firstline, sizeof(firstline))) > 0) {
                write(STDOUT_FILENO, firstline, port);
        }

        close(sock);

        exit(0);
}

void multicast_headers()
{
        FILE *headers = (FILE *)NULL;
        char headerline[1024];

        if((headers = fopen(MULTI_HEADERS_FILE, "r")) == (FILE *)NULL) {
                debug("Error opening "  MULTI_HEADERS_FILE ".\n");
        }

        while(fgets(headerline, sizeof(headerline), headers) != (char *)NULL) {
                fprintf(stdout, "%s", headerline);
                fflush(stdout);
        }

        fclose(headers);

        fprintf(stdout, "\n");
        fflush(stdout);

        return;


}

int resolv4(char *hostname, struct sockaddr_in *saddr)
{
        struct hostent *host_data;

        if((saddr->sin_addr.s_addr = inet_addr(hostname)) != -1) {
                return(0);
        }

        if((host_data = gethostbyname(hostname)) == (struct hostent *)NULL) {
                return(-1);
        }
        memcpy(&saddr->sin_addr.s_addr, host_data->h_addr_list[0], host_data->h_length);
        return(0);
}

/* EOF */
