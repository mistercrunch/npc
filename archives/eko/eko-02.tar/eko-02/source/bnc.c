
/* Simple bnc written by me, vilest vilest@gotroots.com http://gotroots.com/~vilest */
/* if you get a socket error or someshit, please just change the port*/


#define    MY_PASSWORD    "vuduownsyou"
#define    SERV_TCP_PORT  31337

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
 
#define    QLEN           5
 
char sbuf[2048], cbuf[2048];
extern int errno;
extern char *sys_errlist[];
void reaper();
int main();
void telcli();
 
int main(argc, argv)
int argc;
char *argv[];
{
    int srv_fd, rem_fd, rem_len, opt = 1;
    struct sockaddr_in rem_addr, srv_addr;
 
    bzero((char *) &rem_addr, sizeof(rem_addr));
    bzero((char *) &srv_addr, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    srv_addr.sin_port = htons(SERV_TCP_PORT);
    srv_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (bind(srv_fd, (struct sockaddr *) &srv_addr, sizeof(srv_addr)) == -1) {
        perror("bind");
        exit(-1);
    }
    listen(srv_fd, QLEN);
    close(0); close(1); close(2);
#ifdef TIOCNOTTY
    if ((rem_fd = open("/dev/tty", O_RDWR)) >= 0) {
        ioctl(rem_fd, TIOCNOTTY, (char *)0);
        close(rem_fd);
    }
#endif
    if (fork()) exit(0);
    while (1) {
    rem_len = sizeof(rem_addr);
        rem_fd=accept(srv_fd, (struct sockaddr *) &rem_addr, &rem_len);
        if (rem_fd < 0) {
            if (errno == EINTR) continue;
            exit(-1);
        }
        switch(fork()) {
        case 0:                             /* child process w00t w00t*/
            close(srv_fd);                  /* close original socket */
            telcli(rem_fd);                 /* process the request */
            close(rem_fd);
            exit(0);
            break;
        default: 
            close(rem_fd);                  /* parent process */
            if (fork()) exit(0);            /* let init worry about children */
            break;
        case -1:
            fprintf(stderr, "\n\rfork: %s\n\r", sys_errlist[errno]);
            break;
        }
    }
}
 
void telcli(source)
int source;
{
    int dest;
    int found;
    struct sockaddr_in sa;
    struct hostent *hp;
    struct servent *sp;
    char gethost[100];
    char getport[100];
    char string[100];
 
    bzero(gethost, 100);
    read(source, gethost, 100);
    sprintf(string, "");
    write(source, string, strlen(string));
    read(source, gethost, 100);
    gethost[(strlen(gethost)-2)] = '\0'; /* kludge alert - kill the \r\n */
    if (strcmp(gethost, MY_PASSWORD) != 0) {
        close(source);
        exit(0);
    }
    do {
		found = 0;
		bzero(gethost,100);
        sprintf(string, "bnc ready.\n");
        write(source, string, strlen(string));
        sprintf(string, "Host: ");
        write(source, string, strlen(string));
        read(source, gethost, 100);
        gethost[(strlen(gethost)-2)] = '\0';
        hp = gethostbyname(gethost);
        if (hp) {
            found++;
#if !defined(h_addr)        /* In 4.3, this is a #define */
#if defined(hpux) || defined(NeXT) || defined(ultrix) || defined(POSIX)
            memcpy((caddr_t)&sa.sin_addr, hp->h_addr_list[0], hp->h_length);
#else
            bcopy(hp->h_addr_list[0], &sa.sin_addr, hp->h_length);
#endif
#else /* defined(h_addr) */
#if defined(hpux) || defined(NeXT) || defined(ultrix) || defined(POSIX)
            memcpy((caddr_t)&sa.sin_addr, hp->h_addr, hp->h_length);
#else
            bcopy(hp->h_addr, &sa.sin_addr, hp->h_length);
#endif
#endif /* defined(h_addr) */
            sprintf(string, "Found address for %s\n", hp->h_name);
            write(source, string, strlen(string));
        } else {
            if (inet_addr(gethost) == -1) {
                found = 0;
                sprintf(string, "Didnt find address for %s\n", gethost);
                write(source, string, strlen(string));
            } else {
                found++;
                sa.sin_addr.s_addr = inet_addr(gethost);
            }
        }
    } while (!found);
    sa.sin_family = AF_INET;
    sprintf(string, "Port: ");
    write(source, string, strlen(string));
    read(source, getport, 100);
    gethost[(strlen(getport)-2)] = '\0';
    sa.sin_port = htons((unsigned) atoi(getport));
    if (sa.sin_port == 0) {
        sp = getservbyname(getport, "tcp");
        if (sp)
            sa.sin_port = sp->s_port;
        else {
            sprintf(string, "%s: bad port number\n", getport);
            write(source, string, strlen(string));
            return;
        }
    }
    sprintf(string, "Trying %s...\n", (char *) inet_ntoa(sa.sin_addr));
    write(source, string, strlen(string));
    if ((dest = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("telcli: socket");
        exit(1);
    }
    connect(dest, (struct sockaddr *) &sa, sizeof(sa));
    sprintf(string, "Connected to %s port %d...\n", inet_ntoa(sa.sin_addr),
                                                          ntohs(sa.sin_port));
    write(source, string, strlen(string));
#ifdef FNDELAY
    fcntl(source,F_SETFL,fcntl(source,F_GETFL,0)|FNDELAY);
    fcntl(dest,F_SETFL,fcntl(dest,F_GETFL,0)|FNDELAY);
#else
    fcntl(source,F_SETFL,O_NDELAY);
    fcntl(dest,F_SETFL,O_NDELAY);
#endif
    communicate(dest,source);
    close(dest);
    exit(0);
}
 
communicate(sfd,cfd)    {
    char *chead, *ctail, *shead, *stail;
    int num, nfd, spos, cpos;
    extern int errno;
    fd_set rd, wr;
 
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
