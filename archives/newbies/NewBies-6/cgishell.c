/**************************************************************************
 * cGiScAnNeR ver 1.0 
 * coded by ^cThUlHu^
 *
 * Allows u to scan anonimously for cgi vulnerabilities
 * it uses a proxy server, so your ip has not been logged 
 * Only the proxy server will figure in access_log!!!
 *
 * Compile with :
 * gcc cgiscanner.c -o cgiscanner
 *
 * Syntax :
 * ./cgiscanner <target> <proxy> <port>
 *
 * ThAnKs To : DanteAlig,Kobeb,Zebra3,MarlboroL,Gaizka,caesar79
 *
 * Please report bugs,injuries and what u want at cthulhu_dou@hotmail.com
 *
 * -- Have a nice hack! --
 *************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<netdb.h>
#include<string.h>

struct sockaddr_in proc;
struct hostent *hs;
int port,sock,i;
char *okmsg="200 OK";                           /*Positive response from the proxy*/
char *request = "GET http://";
char tryfor[256];
  
void Scanning(char *target,char *proxy,char *prt) {
  char cgibuff[1024];
  char *buff[100];   
  char *cginame[100];
  buff[1] = "/cgi-bin/unlg1.1 HTTP/1.0\n\n"; 
  buff[2] = "/cgi-bin/rwwwshell.pl HTTP/1.0\n\n";
  buff[3] = "/cgi-bin/phf HTTP/1.0\n\n";
  buff[4] = "/cgi-bin/Count.cgi HTTP/1.0\n\n";
  buff[5] = "/cgi-bin/test-cgi HTTP/1.0\n\n";
  buff[6] = "/cgi-bin/nph-test-cgi HTTP/1.0\n\n";
  buff[7] = "/cgi-bin/nph-publish HTTP/1.0\n\n";
  buff[8] = "/cgi-bin/php.cgi HTTP/1.0\n\n";
  buff[9] = "/cgi-bin/handler HTTP/1.0\n\n";
  buff[10] = "/cgi-bin/webgais HTTP/1.0\n\n";
  buff[11] = "/cgi-bin/websendmail HTTP/1.0\n\n";
  buff[12] = "/cgi-bin/webdist.cgi HTTP/1.0\n\n";
  buff[13] = "/cgi-bin/faxsurvey HTTP/1.0\n\n";
  buff[14] = "/cgi-bin/htmlscript HTTP/1.0\n\n";
  buff[15] = "/cgi-bin/pfdispaly.cgi HTTP/1.0\n\n";
  buff[16] = "/cgi-bin/perl.exe HTTP/1.0\n\n";
  buff[17] = "/cgi-bin/wwwboard.pl HTTP/1.0\n\n";
  buff[18] = "/cgi-bin/www-sql HTTP/1.0\n\n";
  buff[19] = "/cgi-bin/view-source HTTP/1.0\n\n";
  buff[20] = "/cgi-bin/campas HTTP/1.0\n\n";
  buff[21] = "/cgi-bin/aglimpse HTTP/1.0\n\n";
  buff[22] = "/cgi-bin/glimpse HTTP/1.0\n\n";
  buff[23] = "/cgi-bin/man.sh HTTP/1.0\n\n";
  buff[24] = "/cgi-bin/AT-admin.cgi HTTP/1.0\n\n";
  buff[25] = "/cgi-bin/filemail.pl HTTP/1.0\n\n";
  buff[26] = "/cgi-bin/maillist.pl HTTP/1.0\n\n";
  buff[27] = "/cgi-bin/jj HTTP/1.0\n\n";
  buff[28] = "/cgi-bin/info2www HTTP/1.0\n\n";
  buff[29] = "/cgi-bin/files.pl HTTP/1.0\n\n";
  buff[30] = "/cgi-bin/finger HTTP/1.0\n\n";
  buff[31] = "/cgi-bin/bnbform.cgi HTTP/1.0\n\n";
  buff[32] = "/cgi-bin/survey.cgi HTTP/1.0\n\n";
  buff[33] = "/cgi-bin/AnyForm2 HTTP/1.0\n\n";
  buff[34] = "/cgi-bin/textcounter.pl HTTP/1.0\n\n";
  buff[35] = "/cgi-bin/classifieds.cgi HTTP/1.0\n\n";
  buff[36] = "/cgi-bin/environ.cgi HTTP/1.0\n\n";
  buff[37] = "/cgi-bin/wrap HTTP/1.0\n\n";
  buff[38] = "/cgi-bin/cgiwrap HTTP/1.0\n\n";
  buff[39] = "/cgi-bin/guestbook.cgi HTTP/1.0\n\n";
  buff[40] = "/cgi-bin/edit.pl HTTP/1.0\n\n";
  buff[41] = "/cgi-bin/perlshop.cgi HTTP/1.0\n\n";
  buff[42] = "/_vti_inf.html HTTP/1.0\n\n";
  buff[43] = "/_vti_pvt/service.pwd HTTP/1.0\n\n";
  buff[44] = "/_vti_pvt/users.pwd HTTP/1.0\n\n";
  buff[45] = "/_vti_pvt/authors.pwd HTTP/1.0\n\n";
  buff[46] = "/_vti_pvt/administrators.pwd HTTP/1.0\n\n";
  buff[47] = "/_vti_bin/shtml.dll HTTP/1.0\n\n";
  buff[48] = "/_vti_bin/shtml.exe HTTP/1.0\n\n";
  buff[49] = "/cgi-dos/args.bat HTTP/1.0\n\n";
  buff[50] = "/cgi-win/uploader.exe HTTP/1.0\n\n";
  buff[51] = "/cgi-bin/rguest.exe HTTP/1.0\n\n";
  buff[52] = "/cgi-bin/wguest.exe HTTP/1.0\n\n";
  buff[53] = "/scripts/issadmin/bdir.htr HTTP/1.0\n\n";
  buff[54] = "/scripts/CGImail.exe HTTP/1.0\n\n";
  buff[55] = "/scripts/tools/newdsn.exe HTTP/1.0\n\n";
  buff[56] = "/scripts/fpcount.exe HTTP/1.0\n\n";
  buff[57] = "/cfdocs/expelval/openfile.cfm HTTP/1.0\n\n";
  buff[58] = "/cfdocs/expelval/exprcalc.cfm HTTP/1.0\n\n";
  buff[59] = "/cfdocs/expelval/displayopenedfile.cfm HTTP/1.0\n\n";
  buff[60] = "/cfdocs/expelval/sendmail.cfm HTTP/1.0\n\n";
  buff[61] = "/iissamples/exair/howitworks/codebrws.asp HTTP/1.0\n\n";
  buff[62] = "/iissamples/sdk/asp/docs/codebrws.asp HTTP/1.0\n\n";
  buff[63] = "/msads/Samples/SELECTOR/showcode.asp HTTP/1.0\n\n";
  buff[64] = "/search97.vts HTTP/1.0\n\n";
  buff[65] = "/carbo.dll HTTP/1.0\n\n";
 
  cginame[1] = "UnlG - backd00r ";
  cginame[2] = "THC - backd00r  ";
  cginame[3] = "phf             ";
  cginame[4] = "Count.cgi       ";
  cginame[5] = "test-cgi        ";
  cginame[6] = "nph-test-cgi    ";
  cginame[7] = "nph-publish     ";
  cginame[8] = "php.cgi         ";
  cginame[9] = "handler         ";
  cginame[10] = "webgais         ";
  cginame[11] = "websendmail     ";
  cginame[12] = "webdist.cgi     ";
  cginame[13] = "faxsurvey       ";
  cginame[14] = "htmlscript      ";
  cginame[15] = "pfdisplay       ";
  cginame[16] = "perl.exe        ";
  cginame[17] = "wwwboard.pl     ";
  cginame[18] = "www-sql         ";
  cginame[19] = "view-source     ";
  cginame[20] = "campas          ";
  cginame[21] = "aglimpse        ";
  cginame[22] = "glimpse         ";
  cginame[23] = "man.sh          ";
  cginame[24] = "AT-admin.cgi    ";
  cginame[25] = "filemail.pl     ";
  cginame[26] = "maillist.pl     ";
  cginame[27] = "jj              ";
  cginame[28] = "info2www        ";
  cginame[29] = "files.pl        ";
  cginame[30] = "finger          ";
  cginame[31] = "bnbform.cgi     ";
  cginame[32] = "survey.cgi      ";
  cginame[33] = "AnyForm2        ";
  cginame[34] = "textcounter.pl  ";
  cginame[35] = "classifields.cgi";
  cginame[36] = "environ.cgi     ";
  cginame[37] = "wrap            ";
  cginame[38] = "cgiwrap         ";
  cginame[39] = "guestbook.cgi   ";
  cginame[40] = "edit.pl         ";
  cginame[41] = "perlshop.cgi    ";
  cginame[42] = "_vti_inf.html   ";
  cginame[43] = "service.pwd     ";
  cginame[44] = "users.pwd       ";
  cginame[45] = "authors.pwd     ";
  cginame[46] = "administrators  ";
  cginame[47] = "shtml.dll       ";
  cginame[48] = "shtml.exe       ";
  cginame[49] = "args.bat        ";
  cginame[50] = "uploader.exe    ";
  cginame[51] = "rguest.exe      ";
  cginame[52] = "wguest.exe      ";
  cginame[53] = "bdir - samples  ";
  cginame[54] = "CGImail.exe     ";
  cginame[55] = "newdsn.exe      ";
  cginame[56] = "fpcount.exe     ";
  cginame[57] = "openfile.cfm    ";
  cginame[58] = "exprcalc.cfm    ";
  cginame[59] = "dispopenedfile  ";
  cginame[60] = "sendmail.cfm    ";
  cginame[61] = "codebrws.asp    ";
  cginame[62] = "codebrws.asp 2  ";
  cginame[63] = "showcode.asp    ";
  cginame[64] = "search97.vts    ";
  cginame[65] = "carbo.dll       ";

  port = atoi(prt);
  printf("Looking Up %s\n",proxy);
  hs = gethostbyname(proxy);
   if(!hs) {
    fprintf(stderr,"Unknown host %s!\n",proxy);
    exit(0);
   }
   else {
    for(i=1;i<66;i++) {
     memset(&proc,0,sizeof(proc));
     memcpy((char*)&proc.sin_addr,hs->h_addr,hs->h_length);
     proc.sin_family = AF_INET;
     proc.sin_port = htons(port);
     sprintf(tryfor,"%s%s%s",request,target,buff[i]);
     sock = socket(AF_INET,SOCK_STREAM,0);
      if(sock<0) {
       fprintf(stderr,"Error!!!Can't Connect to %s\n",proxy);
       exit(0);
      }
     if(connect(sock,(struct sockaddr *) &proc,sizeof(proc)) ==-1) {
      fprintf(stderr,"Error!!!Can't Connect to %s\n",proxy);
      exit(0);
     }
     printf("Checking for %s",cginame[i]);
     send(sock,tryfor,strlen(tryfor),0);
     recv(sock,cgibuff,sizeof(cgibuff),0); 
     if(strstr(cgibuff,okmsg)) {
      printf("Found !!\n");
     }
     else {
      printf("Not Found\n");
     }     
    }
   close(sock);
   exit(0);
  }  
}

main(int argc,char **argv) {
  system("clear");
  printf("\033[9;37m\n###############################################\n");
  printf("\033[9;37m#\033[9;36m cGiScAnNeR vEr 1.0 ........................ \033[9;37m#\n");
  printf("\033[9;37m#\033[9;36m cOdEd bY cThUlHu .......................... \033[9;37m#\n");
  printf("\033[9;37m###############################################\n\n");
  if(argc !=4) {
   printf(" Wrong number of Arguments !!!\n\n");
   printf("\033[9;37m###############################################\n");
   printf("\033[9;37m#\033[9;31m Usage\033[9;37m : ................................... #\n");
   printf("\033[9;37m#\033[9;31m ./cgiscanner <target> <proxy> <proxy port>.\033[9;37m #\n");
   printf("\033[9;37m# ........................................... #\n");
   printf("\033[9;37m#\033[9;31m <target> host name you want scan ..........\033[9;37m #\n");
   printf("\033[9;37m#\033[9;31m <proxy>  proxy name .......................\033[9;37m #\n");
   printf("\033[9;37m#\033[9;31m <proxy port> on what port runs the proxy? .\033[9;37m #\n");
   printf("\033[9;37m###############################################\n\n"); 
  }
  else {
   Scanning(argv[1],argv[2],argv[3]);
  }
}
