
// It has been added you complete vunerability on interfaces cgi,
// errors but recent like "Pollit", "Piranha" Backdoor RedHat, "bb-hostsv.s"
// bugs discovered by the Big brother, "Servlet" Java web server bug by Sun
// Microsystems the scanner makes the operation on the version of the HTTPd
// and later it makes under the cgi of webserver bugs vunerables to remote
// attacks by means of calls to cgi has been added 218


/* 
---------------------------------------------------------------------------- 
*/
/* 	CGI scanner by alt3kx_h3z
/*
/*	You complete common vunerability CGI
/* 	Tested on systems linux with kernel 2.0.35;RH 5.2(2.0.36);
/*      FreeBSD 2.2.2-3.1;IRIX 5.3
/*
/* 	(c) 2000 alt3kx_h3z <alt3kx_h3z@raza-mexicana.org>
/*
/*	w w w . r a z a - m e x i c a n a . o r g
/*	w w w . h e r t m x . o r g
/*
/*	Compile  : gcc -o scan scan.c
/*   	Usage	 : ./scan host
/*
/*      Greet to: Packet Storm and Ken, ADM crew, dr_fdisk^, Raregazz, X-ploit
/*
/*	Winner Vicente FOX
/*
/* 
---------------------------------------------------------------------------- 
*/

#include <string.h>
#include <netdb.h>
#include <ctype.h>
#include <arpa/nameser.h>
#include <sys/stat.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>



void main(int argc, char *argv[])
{

  //initializing variales
  char *raregazz;
  char buffer[1024];
  char msg[] = "200";
  int h3z=0;
  int raza;
  int alt3kx=0;
  char cgibuff[20];
  char *buff[300];
  char *bug[300];


  int sock,debugm=0;
  struct in_addr addr;
  struct sockaddr_in sin;
  struct hostent *he;
  unsigned long inicio;
  unsigned long end;
  unsigned long fox;


  buff[1] = "GET /cgi-bin/unlg1.1 HTTP/1.0\n\n";
  buff[2] = "GET /cgi-bin/rwwwshell.pl HTTP/1.0\n\n";
  buff[3] = "GET /cgi-bin/phf HTTP/1.0\n\n";
  buff[4] = "GET /cgi-bin/Count.cgi HTTP/1.0\n\n";
  buff[5] = "GET /cgi-bin/test-cgi HTTP/1.0\n\n";
  buff[6] = "GET /cgi-bin/nph-test-cgi HTTP/1.0\n\n";
  buff[7] = "GET /cgi-bin/nph-publish HTTP/1.0\n\n";
  buff[8] = "GET /cgi-bin/php.cgi HTTP/1.0\n\n";
  buff[9] = "GET /cgi-bin/handler HTTP/1.0\n\n";
  buff[10] = "GET /cgi-bin/webgais HTTP/1.0\n\n";
  buff[11] = "GET /cgi-bin/websendmail HTTP/1.0\n\n";
  buff[12] = "GET /cgi-bin/webdist.cgi HTTP/1.0\n\n";
  buff[13] = "GET /cgi-bin/faxsurvey HTTP/1.0\n\n";
  buff[14] = "GET /cgi-bin/htmlscript HTTP/1.0\n\n";
  buff[15] = "GET /cgi-bin/pfdispaly.cgi HTTP/1.0\n\n";
  buff[16] = "GET /cgi-bin/perl.exe HTTP/1.0\n\n";
  buff[17] = "GET /cgi-bin/wwwboard.pl HTTP/1.0\n\n";
  buff[18] = "GET /cgi-bin/www-sql HTTP/1.0\n\n";
  buff[19] = "GET /cgi-bin/view-source HTTP/1.0\n\n";
  buff[20] = "GET /cgi-bin/campas HTTP/1.0\n\n";
  buff[21] = "GET /cgi-bin/aglimpse HTTP/1.0\n\n";
  buff[22] = "GET /cgi-bin/glimpse HTTP/1.0\n\n";
  buff[23] = "GET /cgi-bin/man.sh HTTP/1.0\n\n";
  buff[24] = "GET /cgi-bin/AT-admin.cgi HTTP/1.0\n\n";
  buff[25] = "GET /cgi-bin/filemail.pl HTTP/1.0\n\n";
  buff[26] = "GET /cgi-bin/maillist.pl HTTP/1.0\n\n";
  buff[27] = "GET /cgi-bin/jj HTTP/1.0\n\n";
  buff[28] = "GET /cgi-bin/info2www HTTP/1.0\n\n";
  buff[29] = "GET /cgi-bin/files.pl HTTP/1.0\n\n";
  buff[30] = "GET /cgi-bin/finger HTTP/1.0\n\n";
  buff[31] = "GET /cgi-bin/bnbform.cgi HTTP/1.0\n\n";
  buff[32] = "GET /cgi-bin/survey.cgi HTTP/1.0\n\n";
  buff[33] = "GET /cgi-bin/AnyForm2 HTTP/1.0\n\n";
  buff[34] = "GET /cgi-bin/textcounter.pl HTTP/1.0\n\n";
  buff[35] = "GET /cgi-bin/classifieds.cgi HTTP/1.0\n\n";
  buff[36] = "GET /cgi-bin/environ.cgi HTTP/1.0\n\n";
  buff[37] = "GET /cgi-bin/wrap HTTP/1.0\n\n";
  buff[38] = "GET /cgi-bin/cgiwrap HTTP/1.0\n\n";
  buff[39] = "GET /cgi-bin/guestbook.cgi HTTP/1.0\n\n";
  buff[40] = "GET /cgi-bin/edit.pl HTTP/1.0\n\n";
  buff[41] = "GET /cgi-bin/perlshop.cgi HTTP/1.0\n\n";
  buff[42] = "GET /_vti_inf.html HTTP/1.0\n\n";
  buff[43] = "GET /_vti_pvt/service.pwd HTTP/1.0\n\n";
  buff[44] = "GET /_vti_pvt/users.pwd HTTP/1.0\n\n";
  buff[45] = "GET /_vti_pvt/authors.pwd HTTP/1.0\n\n";
  buff[46] = "GET /_vti_pvt/administrators.pwd HTTP/1.0\n\n";
  buff[47] = "GET /_vti_bin/shtml.dll HTTP/1.0\n\n";
  buff[48] = "GET /_vti_bin/shtml.exe HTTP/1.0\n\n";
  buff[49] = "GET /cgi-dos/args.bat HTTP/1.0\n\n";
  buff[50] = "GET /cgi-win/uploader.exe HTTP/1.0\n\n";
  buff[51] = "GET /cgi-bin/rguest.exe HTTP/1.0\n\n";
  buff[52] = "GET /cgi-bin/wguest.exe HTTP/1.0\n\n";
  buff[53] = "GET /scripts/issadmin/bdir.htr HTTP/1.0\n\n";
  buff[54] = "GET /scripts/CGImail.exe HTTP/1.0\n\n";
  buff[55] = "GET /scripts/tools/newdsn.exe HTTP/1.0\n\n";
  buff[56] = "GET /scripts/fpcount.exe HTTP/1.0\n\n";
  buff[57] = "GET /cfdocs/expelval/openfile.cfm HTTP/1.0\n\n";
  buff[58] = "GET /cfdocs/expelval/exprcalc.cfm HTTP/1.0\n\n";
  buff[59] = "GET /cfdocs/expelval/displayopenedfile.cfm HTTP/1.0\n\n";
  buff[60] = "GET /cfdocs/expelval/sendmail.cfm HTTP/1.0\n\n";
  buff[61] = "GET /iissamples/exair/howitworks/codebrws.asp HTTP/1.0\n\n";
  buff[62] = "GET /iissamples/sdk/asp/docs/codebrws.asp HTTP/1.0\n\n";
  buff[63] = "GET /msadc/Samples/SELECTOR/showcode.asp HTTP/1.0\n\n";
  buff[64] = "GET /search97.vts HTTP/1.0\n\n";
  buff[65] = "GET /carbo.dll HTTP/1.0\n\n";
  buff[66] = "GET /cfdocs/zero.cfm HTTP/1.0\n\n";
  buff[67] = "GET /cfdocs/root.cfm HTTP/1.0\n\n";
  buff[68] = "GET /cfdocs/expressions.cfm HTTP/1.0\n\n";
  buff[69] = "GET /cfdocs/TOXIC.CFM HTTP/1.0\n\n";
  buff[70] = "GET /cfdocs/MOLE.CFM  HTTP/1.0\n\n";
  buff[71] = "GET /cfdocs/expeval/exprcalc.cfm HTTP/1.0\n\n";
  buff[72] = "GET /cfdocs/expeval/sendmail.cfm HTTP/1.0\n\n";
  buff[73] = "GET /cfdocs/expeval/eval.cfm HTTP/1.0\n\n";
  buff[74] = "GET /cfdocs/expeval/openfile.cfm HTTP/1.0\n\n";
  buff[75] = "GET /cfdocs/expeval/displayopenedfile.cfm HTTP/1.0\n\n";
  buff[76] = "GET /cfdocs/exampleapp/publish/admin/addcontent.cfm HTTP/1.0\n\n";
  buff[77] = "GET /cfdocs/exampleapp/email/getfile.cfm?filename=c:\boot.ini HTTP/1.0\n\n";
  buff[78] = "GET /cfdocs/exampleapp/publish/admin/application.cfm HTTP/1.0\n\n";
  buff[79] = "GET /cfdocs/exampleapp/email/application.cfm HTTP/1.0\n\n";
  buff[80] = "GET /cfdocs/exampleapp/docs/sourcewindow.cfm HTTP/1.0\n\n";
  buff[81] = "GET /cfdocs/examples/parks/detail.cfm HTTP/1.0\n\n";
  buff[82] = "GET /cfdocs/examples/cvbeans/beaninfo.cfm HTTP/1.0\n\n";
  buff[83] = "GET /cfdocs/cfmlsyntaxcheck.cfm HTTP/1.0\n\n";
  buff[84] = "GET /cfdocs/snippets/viewexample.cfm HTTP/1.0\n\n";
  buff[85] = "GET /cfdocs/snippets/gettempdirectory.cfm HTTP/1.0\n\n";
  buff[86] = "GET /cfdocs/snippets/fileexists.cfm HTTP/1.0\n\n";
  buff[87] = "GET /cfdocs/snippets/evaluate.cfm HTTP/1.0\n\n";
  buff[88] = "GET /cfappman/index.cfm HTTP/1.0\n\n";
  buff[89] = "GET /cfusion/cfapps/forums/forums_.mdb HTTP/1.0\n\n";
  buff[90] = "GET /cfusion/cfapps/security/realm_.mdb HTTP/1.0\n\n";
  buff[91] = "GET /cfusion/cfapps/forums/data/forums.mdb HTTP/1.0\n\n";
  buff[92] = "GET /cfusion/cfapps/security/data/realm.mdb HTTP/1.0\n\n";
  buff[93] = "GET /cfusion/database/cfexamples.mdb HTTP/1.0\n\n";
  buff[94] = "GET /cfusion/database/cfsnippets.mdb HTTP/1.0\n\n";
  buff[95] = "GET /cfusion/database/smpolicy.mdb HTTP/1.0\n\n";
  buff[96] = "GET /cfusion/database/cypress.mdb HTTP/1.0\n\n";
  buff[97] = "GET /DataBase/ HTTP/1.0\n\n";
  buff[98] = "GET /database.nsf/e1466a8590/6048076233?OpenDocument HTTP/1.0\n\n";
  buff[99] = "GET /database.nsf/e146fa8590/6148076233?EditDocument HTTP/1.0\n\n";
  buff[100] = "GET /database.nsf/e146fa8590/6148076233?DeleteDocument HTTP/1.0\n\n";
  buff[101] = "GET /domcfg.nsf/?open HTTP/1.0\n\n";
  buff[102] = "GET /domcfg.nsf/URLRedirect/?OpenForm. HTTP/1.0\n\n";
  buff[103] = "GET /domcfg.nsf/ HTTP/1.0\n\n";
  buff[104] = "GET /domlog.nsf/ HTTP/1.0\n\n";
  buff[105] = "GET /log.nsf/  HTTP/1.0\n\n";
  buff[106] = "GET /catalog.nsf/ HTTP/1.0\n\n";
  buff[107] = "GET /names.nsf/   HTTP/1.0\n\n";
  buff[108] = "GET /database.nsf/ HTTP/1.0\n\n";
  buff[109] = "GET /today.nsf/ HTTP/1.0\n\n";
  buff[110] = "GET /cgi-bin/cgi-lib.pl HTTP/1.0\n\n";
  buff[111] = "GET /cgi-bin/imagemap.c HTTP/1.0\n\n";
  buff[112] = "GET /cgi-bin/imagemap.exe HTTP/1.0\n\n";
  buff[113] = "GET /cgi-bin/minimal.exe HTTP/1.0\n\n";
  buff[114] = "GET /cgi-bin/redir.exe HTTP/1.0\n\n";
  buff[115] = "GET /cgi-bin/stats.prg HTTP/1.0\n\n";
  buff[116] = "GET /cgi-bin/statsconfig HTTP/1.0\n\n";
  buff[117] = "GET /cgi-bin/visadmin.exe HTTP/1.0\n\n";
  buff[118] = "GET /cgi-bin/visitor.exe HTTP/1.0\n\n";
  buff[119] = "GET /cgi-bin/htmldocs HTTP/1.0\n\n";
  buff[120] = "GET /cgi-bin/logs HTTP/1.0\n\n";
  buff[121] = "GET /scripts/    HTTP/1.0\n\n";
  buff[122] = "GET /Default.asp HTTP/1.0\n\n";
  buff[123] = "GET /_vti_bin  HTTP/1.0\n\n";
  buff[124] = "GET /_vti_bin/_vti_adm HTTP/1.0\n\n";
  buff[125] = "GET /_vti_bin/_vti_aut HTTP/1.0\n\n";
  buff[126] = "GET /srchadm  HTTP/1.0\n\n";
  buff[127] = "GET /iisadmin   HTTP/1.0\n\n";
  buff[128] = "GET /_AuthChangeUrl? HTTP/1.0\n\n";
  buff[129] = "GET /_vti_inf.html HTTP/1.0\n\n";
  buff[130] = "GET /?PageServices HTTP/1.0\n\n";
  buff[131] = "GET /html/?PageServices  HTTP/1.0\n\n";
  buff[132] = "GET /scripts/cpshost.dll HTTP/1.0\n\n";
  buff[133] = "GET /scripts/uploadn.asp HTTP/1.0\n\n";
  buff[134] = "GET /scripts/uploadx.asp HTTP/1.0\n\n";
  buff[135] = "GET /scripts/upload.asp  HTTP/1.0\n\n";
  buff[136] = "GET /scripts/repost.asp  HTTP/1.0\n\n";
  buff[137] = "GET /scripts/postinfo.asp HTTP/1.0\n\n";
  buff[138] = "GET /scripts/run.exe HTTP/1.0\n\n";
  buff[139] = "GET /scripts/iisadmin/bdir.htr HTTP/1.0\n\n";
  buff[140] = "GET /scripts/iisadmin/samples/ctgestb.htx HTTP/1.0\n\n";
  buff[141] = "GET /scripts/iisadmin/samples/ctgestb.idc HTTP/1.0\n\n";
  buff[142] = "GET /scripts/iisadmin/samples/details.htx HTTP/1.0\n\n";
  buff[143] = "GET /scripts/iisadmin/samples/details.idc HTTP/1.0\n\n";
  buff[144] = "GET /scripts/iisadmin/samples/query.htx HTTP/1.0\n\n";
  buff[145] = "GET /scripts/iisadmin/samples/query.idc HTTP/1.0\n\n";
  buff[146] = "GET /scripts/iisadmin/samples/register.htx HTTP/1.0\n\n";
  buff[147] = "GET /scripts/iisadmin/samples/register.idc HTTP/1.0\n\n";
  buff[148] = "GET /scripts/iisadmin/samples/sample.htx HTTP/1.0\n\n";
  buff[149] = "GET /scripts/iisadmin/samples/sample.idc HTTP/1.0\n\n";
  buff[150] = "GET /scripts/iisadmin/samples/sample2.htx HTTP/1.0\n\n";
  buff[151] = "GET /scripts/iisadmin/samples/viewbook.htx HTTP/1.0\n\n";
  buff[152] = "GET /scripts/iisadmin/samples/viewbook.idc HTTP/1.0\n\n";
  buff[153] = "GET /scripts/iisadmin/tools/ct.htx HTTP/1.0\n\n";
  buff[154] = "GET /scripts/iisadmin/tools/ctss.idc HTTP/1.0\n\n";
  buff[155] = "GET /scripts/iisadmin/tools/dsnform.exe HTTP/1.0\n\n";
  buff[156] = "GET /scripts/iisadmin/tools/getdrvrs.exe HTTP/1.0\n\n";
  buff[157] = "GET /scripts/iisadmin/tools/mkilog.exe HTTP/1.0\n\n";
  buff[158] = "GET /scripts/iisadmin/tools/newdsn.exe HTTP/1.0\n\n";
  buff[159] = "GET /IISADMPWD/achg.htr HTTP/1.0\n\n";
  buff[160] = "GET /IISADMPWD/aexp.htr HTTP/1.0\n\n";
  buff[161] = "GET /IISADMPWD/aexp2.htr HTTP/1.0\n\n";
  buff[162] = "GET /IISADMPWD/aexp2b.htr HTTP/1.0\n\n";
  buff[163] = "GET /IISADMPWD/aexp3.htr HTTP/1.0\n\n";
  buff[164] = "GET /IISADMPWD/aexp4.htr HTTP/1.0\n\n";
  buff[165] = "GET /IISADMPWD/aexp4b.htr HTTP/1.0\n\n";
  buff[166] = "GET /IISADMPWD/anot.htr  HTTP/1.0\n\n";
  buff[167] = "GET /IISADMPWD/anot3.htr HTTP/1.0\n\n";
  buff[168] = "GET /_vti_pvt/writeto.cnf HTTP/1.0\n\n";
  buff[169] = "GET /_vti_pvt/svcacl.cnf HTTP/1.0\n\n";
  buff[170] = "GET /_vti_pvt/services.cnf HTTP/1.0\n\n";
  buff[171] = "GET /_vti_pvt/service.stp  HTTP/1.0\n\n";
  buff[172] = "GET /_vti_pvt/service.cnf  HTTP/1.0\n\n";
  buff[173] = "GET /_vti_pvt/access.cnf   HTTP/1.0\n\n";
  buff[174] = "GET /_private/registrations.txt HTTP/1.0\n\n";
  buff[175] = "GET /_private/registrations.htm HTTP/1.0\n\n";
  buff[176] = "GET /_private/register.txt  HTTP/1.0\n\n";
  buff[177] = "GET /_private/register.htm  HTTP/1.0\n\n";
  buff[178] = "GET /_private/orders.txt  HTTP/1.0\n\n";
  buff[179] = "GET /_private/orders.htm  HTTP/1.0\n\n";
  buff[180] = "GET /_private/form_results.htm HTTP/1.0\n\n";
  buff[181] = "GET /_private/form_results.txt HTTP/1.0\n\n";
  buff[182] = "GET /admisapi/fpadmin.htm HTTP/1.0\n\n";
  buff[183] = "GET /scripts/Fpadmcgi.exe HTTP/1.0\n\n";
  buff[184] = "GET /_vti_bin/_vti_aut/author.dll HTTP/1.0\n\n";
  buff[185] = "GET /_vti_bin/_vti_adm/admin.dll  HTTP/1.0\n\n";
  buff[186] = "GET /msads/Samples/selector/showcode.asp HTTP/1.0\n\n";
  buff[187] = "GET /scripts/perl? HTTP/1.0\n\n";
  buff[188] = "GET /scripts/proxy/w3proxy.dll HTTP/1.0\n\n";
  buff[189] = "GET /iissamples/sdk/asp/docs/codebrws.asp HTTP/1.0\n\n";
  buff[190] = "GET /iissamples/exair/howitworks/codebrws.asp HTTP/1.0\n\n";
  buff[191] = "GET /scripts/CGImail.exe HTTP/1.0\n\n";
  buff[192] = "GET /AdvWorks/equipment/catalog_type.asp HTTP/1.0\n\n";
  buff[193] = "GET /scripts/iisadmin/default.htm HTTP/1.0\n\n";
  buff[194] = "GET /msadc/samples/adctest.asp HTTP/1.0\n\n";
  buff[195] = "GET /msadc/msadcs.dll HTTP/1.0\n\n";
  buff[196] = "GET /adsamples/config/site.csc HTTP/1.0\n\n";
  buff[197] = "GET /scripts/../../cmd.exe HTTP/1.0\n\n";
  buff[198] = "GET /scripts/cpshost.dll HTTP/1.0\n\n";
  buff[199] = "GET /scripts/convert.bas HTTP/1.0\n\n";
  buff[200] = "GET .html/...... HTTP/1.0\n\n";
  buff[201] = "GET /publisher/ HTTP/1.0\n\n";
  buff[202] = "GET ..../Windows/Admin.pwl HTTP/1.0\n\n";
  buff[203] = "GET ....../......ida/con HTTP/1.0\n\n";
  buff[204] = "GET /iissamples/issamples/oop/qfullhit.htw HTTP/1.0\n\n";
  buff[205] = "GET /iissamples/issamples/oop/qsumrhit.htw HTTP/1.0\n\n";
  buff[206] = "GET /iissamples/exair/search/qfullhit.htw HTTP/1.0\n\n";
  buff[207] = "GET /iissamples/exair/search/qsumrhit.htw HTTP/1.0\n\n";
  buff[208] = "GET /iishelp/iis/misc/iirturnh.htwa HTTP/1.0\n\n";
  buff[209] = "GET /servlet/ HTTP/1.0\n\n";
  buff[210] = "GET /cgi-bin/htsearch HTTP/1.0\n\n";
  buff[211] = "GET /cgi-bin/sawmill5 HTTP/1.0\n\n";
  buff[212] = "GET /cgi-bin/dnewsweb HTTP/1.0\n\n";
  buff[213] = "GET /cgi-bin/infosrch.cgi HTTP/1.0\n\n";
  buff[214] = "GET /piranha/secure/passwd.php3 HTTP/1.0\n\n";
  buff[215] = "GET /cgi-bin/data/nicks HTTP/1.0\n\n";
  buff[216] = "GET /cgi-bin/pollit/Poll_It_SSI_v2.0.cgi HTTP/1.0\n\n";
  buff[217] = "GET /admin-serv/config/admpw HTTP/1.0\n\n";
  buff[218] = "GET /cgi-bin/bb-hostsvc.sh HTTP/1.0\n\n";

  bug[1] = "/cgi-bin/unlg1.1  ";
  bug[2] = "/cgi-bin/rwwwshell.pl  ";
  bug[3] = "/cgi-bin/phf  ";
  bug[4] = "/cgi-bin/Count.cgi  ";
  bug[5] = "/cgi-bin/test-cgi  ";
  bug[6] = "/cgi-bin/nph-test-cgi  ";
  bug[7] = "/cgi-bin/nph-publish  ";
  bug[8] = "/cgi-bin/php.cgi  ";
  bug[9] = "/cgi-bin/handler  ";
  bug[10] = "/cgi-bin/webgais  ";
  bug[11] = "/cgi-bin/websendmail  ";
  bug[12] = "/cgi-bin/webdist.cgi  ";
  bug[13] = "/cgi-bin/faxsurvey  ";
  bug[14] = "/cgi-bin/htmlscript  ";
  bug[15] = "/cgi-bin/pfdispaly.cgi  ";
  bug[16] = "/cgi-bin/perl.exe  ";
  bug[17] = "/cgi-bin/wwwboard.pl  ";
  bug[18] = "/cgi-bin/www-sql        ";
  bug[19] = "/cgi-bin/view-source    ";
  bug[20] = "/cgi-bin/campas         ";
  bug[21] = "/cgi-bin/aglimpse       ";
  bug[22] = "/cgi-bin/glimpse        ";
  bug[23] = "/cgi-bin/man.sh          ";
  bug[24] = "/cgi-bin/AT-admin.cgi    ";
  bug[25] = "/cgi-bin/filemail.pl     ";
  bug[26] = "/cgi-bin/maillist.pl    ";
  bug[27] = "/cgi-bin/jj              ";
  bug[28] = "/cgi-bin/info2www       ";
  bug[29] = "/cgi-bin/files.pl        ";
  bug[30] = "/cgi-bin/finger          ";
  bug[31] = "/cgi-bin/bnbform.cgi     ";
  bug[32] = "/cgi-bin/survey.cgi     ";
  bug[33] = "/cgi-bin/AnyForm2       ";
  bug[34] = "/cgi-bin/textcounter.pl  ";
  bug[35] = "/cgi-bin/classifieds.cgi";
  bug[36] = "/cgi-bin/environ.cgi     ";
  bug[37] = "/cgi-bin/wrap           ";
  bug[38] = "/cgi-bin/cgiwrap        ";
  bug[39] = "/cgi-bin/guestbook.cgi   ";
  bug[40] = "/cgi-bin/edit.pl        ";
  bug[41] = "/cgi-bin/perlshop.cgi    ";
  bug[42] = "/_vti_inf.html  ";
  bug[43] = "/_vti_pvt/service.pwd    ";
  bug[44] = "/_vti_pvt/users.pwd       ";
  bug[45] = "/_vti_pvt/authors.pwd     ";
  bug[46] = "/_vti_pvt/administrators.pwd ";
  bug[47] = "/_vti_bin/shtml.dll";
  bug[48] = "/_vti_bin/shtml.exe";
  bug[49] = "/cgi-dos/args.bat       ";
  bug[50] = "/cgi-win/uploader.exe    ";
  bug[51] = "/cgi-bin/rguest.exe      ";
  bug[52] = "/cgi-bin/wguest.exe      ";
  bug[53] = "scripts/issadmin/bdir.htr  ";
  bug[54] = "/scripts/CGImail.exe    ";
  bug[55] = "/scripts/tools/newdsn.exe     ";
  bug[56] = "/scripts/fpcount.exe    ";
  bug[57] = "/cfdocs/expelval/openfile.cfm    ";
  bug[58] = "/cfdocs/expelval/exprcalc.cfm   ";
  bug[59] = "/cfdocs/expelval/displayopenedfile.cfm  ";
  bug[60] = "/cfdocs/expelval/sendmail.cfm    ";
  bug[61] = "/iissamples/exair/howitworks/codebrws.asp   ";
  bug[62] = "/iissamples/sdk/asp/docs/codebrws.asp  ";
  bug[63] = "/msadc/Samples/SELECTOR/showcode.asp    ";
  bug[64] = "/search97.vts    ";
  bug[65] = "/carbo.dll       ";
  bug[66] = "/cfdocs/zero.cfm";
  bug[67] = "/cfdocs/root.cfm";
  bug[68] = "/_vti_pvt/writeto.cnf";
  bug[69] = "/cfdocs/TOXIC.CFM";
  bug[70] = "/cfdocs/MOLE.CFM ";
  bug[71] = "/cfdocs/expeval/exprcalc.cfm";
  bug[72] = "/cfdocs/expeval/sendmail.cfm";
  bug[73] = "/cfdocs/expeval/eval.cfm";
  bug[74] = "/cfdocs/expeval/openfile.cfm";
  bug[75] = "/cfdocs/expeval/displayopenedfile.cfm";
  bug[76] = "/cfdocs/exampleapp/publish/admin/addcontent.cfm";
  bug[77] = "/cfdocs/exampleapp/email/getfile.cfm?filename=c:\boot.ini";
  bug[78] = "/cfdocs/exampleapp/publish/admin/application.cfm";
  bug[79] = "/cfdocs/exampleapp/email/application.cfm";
  bug[80] = "/cfdocs/exampleapp/docs/sourcewindow.cfm";
  bug[81] = "/cfdocs/examples/parks/detail.cfm";
  bug[82] = "/cfdocs/examples/cvbeans/beaninfo.cfm";
  bug[83] = "/cfdocs/cfmlsyntaxcheck.cfm";
  bug[84] = "/cfdocs/snippets/viewexample.cfm";
  bug[85] = "/cfdocs/snippets/gettempdirectory.cfm";
  bug[86] = "/cfdocs/snippets/fileexists.cfm";
  bug[87] = "/cfdocs/snippets/evaluate.cfm";
  bug[88] = "/cfappman/index.cfm";
  bug[89] = "/cfusion/cfapps/forums/forums_.mdb";
  bug[90] = "/cfusion/cfapps/security/realm_.mdb";
  bug[91] = "/cfusion/cfapps/forums/data/forums.mdb";
  bug[92] = "/cfusion/cfapps/security/data/realm.mdb";
  bug[93] = "/cfusion/database/cfexamples.mdb";
  bug[94] = "/cfusion/database/cfsnippets.mdb";
  bug[95] = "/cfusion/database/smpolicy.mdb";
  bug[96] = "/cfusion/database/cypress.mdb";
  bug[97] = "/DataBase/";
  bug[98] = "/database.nsf/e1466a8590/6048076233?OpenDocument";
  bug[99] = "/database.nsf/e146fa8590/6148076233?EditDocument";
  bug[100] = "/database.nsf/e146fa8590/6148076233?DeleteDocument";
  bug[101] = "/domcfg.nsf/?open";
  bug[102] = "/domcfg.nsf/URLRedirect/?OpenForm.";
  bug[103] = "/domcfg.nsf/";
  bug[104] = "/domlog.nsf/";
  bug[105] = "/log.nsf/";
  bug[106] = "/catalog.nsf/";
  bug[107] = "/names.nsf/";
  bug[108] = "/database.nsf/";
  bug[109] = "/today.nsf/";
  bug[110] = "/cgi-bin/cgi-lib.pl";
  bug[111] = "/cgi-bin/imagemap.c";
  bug[112] = "/cgi-bin/imagemap.exe";
  bug[113] = "/cgi-bin/minimal.exe";
  bug[114] = "/cgi-bin/redir.exe";
  bug[115] = "/cgi-bin/stats.prg";
  bug[116] = "/cgi-bin/statsconfig";
  bug[117] = "/cgi-bin/visadmin.exe";
  bug[118] = "/cgi-bin/visitor.exe";
  bug[119] = "/cgi-bin/htmldocs";
  bug[120] = "/cgi-bin/logs";
  bug[121] = "/scripts/";
  bug[122] = "/Default.asp";
  bug[123] = "/_vti_bin";
  bug[124] = "/_vti_bin/_vti_adm";
  bug[125] = "/_vti_bin/_vti_aut";
  bug[126] = "/srchadm";
  bug[127] = "/iisadmin";
  bug[128] = "/_AuthChangeUrl?";
  bug[129] = "/_vti_inf.html";
  bug[130] = "/?PageServices";
  bug[131] = "/html/?PageServices";
  bug[132] = "/scripts/cpshost.dll";
  bug[133] = "/scripts/uploadn.asp";
  bug[134] = "/scripts/uploadx.asp";
  bug[135] = "/scripts/upload.asp";
  bug[136] = "/scripts/repost.asp";
  bug[137] = "/scripts/postinfo.asp";
  bug[138] = "/scripts/run.exe";
  bug[139] = "/scripts/iisadmin/bdir.htr";
  bug[140] = "/scripts/iisadmin/samples/ctgestb.htx";
  bug[141] = "/scripts/iisadmin/samples/ctgestb.idc";
  bug[142] = "/scripts/iisadmin/samples/details.htx";
  bug[143] = "/scripts/iisadmin/samples/details.idc";
  bug[144] = "/scripts/iisadmin/samples/query.htx";
  bug[145] = "/scripts/iisadmin/samples/query.idc";
  bug[146] = "/scripts/iisadmin/samples/register.htx";
  bug[147] = "/scripts/iisadmin/samples/register.idc";
  bug[148] = "/scripts/iisadmin/samples/sample.htx";
  bug[149] = "/scripts/iisadmin/samples/sample.idc";
  bug[150] = "/scripts/iisadmin/samples/sample2.htx";
  bug[151] = "/scripts/iisadmin/samples/viewbook.htx";
  bug[152] = "/scripts/iisadmin/samples/viewbook.idc";
  bug[153] = "/scripts/iisadmin/tools/ct.htx";
  bug[154] = "/scripts/iisadmin/tools/ctss.idc";
  bug[155] = "/scripts/iisadmin/tools/dsnform.exe";
  bug[156] = "/scripts/iisadmin/tools/getdrvrs.exe";
  bug[157] = "/scripts/iisadmin/tools/mkilog.exe";
  bug[158] = "/scripts/iisadmin/tools/newdsn.exe";
  bug[159] = "/IISADMPWD/achg.htr";
  bug[160] = "/IISADMPWD/aexp.htr";
  bug[161] = "/IISADMPWD/aexp2.htr";
  bug[162] = "/IISADMPWD/aexp2b.htr";
  bug[163] = "/IISADMPWD/aexp3.htr";
  bug[164] = "/IISADMPWD/aexp4.htr";
  bug[165] = "/IISADMPWD/aexp4b.htr";
  bug[166] = "/IISADMPWD/anot.htr";
  bug[167] = "/IISADMPWD/anot3.htr";
  bug[168] = "/_vti_pvt/writeto.cnf";
  bug[169] = "/_vti_pvt/svcacl.cnf";
  bug[170] = "/_vti_pvt/services.cnf";
  bug[171] = "/_vti_pvt/service.stp";
  bug[172] = "/_vti_pvt/service.cnf";
  bug[173] = "/_vti_pvt/access.cnf";
  bug[174] = "/_private/registrations.txt";
  bug[175] = "/_private/registrations.htm";
  bug[176] = "/_private/register.txt";
  bug[177] = "/_private/register.htm";
  bug[178] = "/_private/orders.txt";
  bug[179] = "/_private/orders.htm";
  bug[180] = "/_private/form_results.htm";
  bug[181] = "/_private/form_results.txt";
  bug[182] = "/admisapi/fpadmin.htm";
  bug[183] = "/scripts/Fpadmcgi.exe";
  bug[184] = "/_vti_bin/_vti_aut/author.dll";
  bug[185] = "/_vti_bin/_vti_adm/admin.dll";
  bug[186] = "/msads/Samples/selector/showcode.asp";
  bug[187] = "/scripts/perl?";
  bug[188] = "/scripts/proxy/w3proxy.dll";
  bug[189] = "/iissamples/sdk/asp/docs/codebrws.asp";
  bug[190] = "/iissamples/exair/howitworks/codebrws.asp";
  bug[191] = "/scripts/CGImail.exe";
  bug[192] = "/AdvWorks/equipment/catalog_type.asp";
  bug[193] = "/scripts/iisadmin/default.htm";
  bug[194] = "/msadc/samples/adctest.asp";
  bug[195] = "/msadc/msadcs.dll";
  bug[196] = "/adsamples/config/site.csc";
  bug[197] = "/scripts/../../cmd.exe";
  bug[198] = "/scripts/cpshost.dll";
  bug[199] = "/scripts/convert.bas";
  bug[200] = ".html/......";
  bug[201] = "/publisher/";
  bug[202] = "..../Windows/Admin.pwl";
  bug[203] = "....../......ida/con";
  bug[204] = "/iissamples/issamples/oop/qfullhit.htw";
  bug[205] = "/iissamples/issamples/oop/qsumrhit.htw";
  bug[206] = "/iissamples/exair/search/qfullhit.htw";
  bug[207] = "/iissamples/exair/search/qsumrhit.htw";
  bug[208] = "/iishelp/iis/misc/iirturnh.htwa";
  bug[209] = "/servlet/";
  bug[210] = "/cgi-bin/htsearch";
  bug[211] = "/cgi-bin/sawmill5";
  bug[212] = "/cgi-bin/dnewsweb";
  bug[213] = "/cgi-bin/infosrch.cgi";
  bug[214] = "/piranha/secure/passwd.php3";
  bug[215] = "/cgi-bin/data/nicks";
  bug[216] = "/cgi-bin/pollit/Poll_It_SSI_v2.0.cgi";
  bug[217] = "/admin-serv/config/admpw";
  bug[218] = "/cgi-bin/bb-hostsvc.sh";



if (argc<2)
  {
    printf("\n\t [-- CGI bugs by alt3kx (c) 2000 --]");
    printf("\n\t Usage : %s host \n\n",argv[0]);

    exit(0);
  }

  if ((he=gethostbyname(argv[1])) == NULL) //error de host
  {
    herror("gethostbyname"); //first error
    exit(0);
  }
  system("clear");
  printf("\n\n\t\t[-- CGI bugs Scan by alt3kx (c) 2000 --] ");
  printf("\n\n\t[------ w w w . r a z a - m e x i c a n a . o r g ------] 
\n");

  inicio=inet_addr(argv[1]);

  fox=ntohl(inicio);

  sock=socket(AF_INET, SOCK_STREAM, 0);
  bcopy(he->h_addr, (char *)&sin.sin_addr, he->h_length);
  sin.sin_family=AF_INET;
  sin.sin_port=htons(80);    /* you can changue it :X */

  if (connect(sock, (struct sockaddr*)&sin, sizeof(sin))!=0)
  {
    perror("connect"); //second error
  }
  printf("\n\n\t [ HTTPd version press intro...... ]\n");
  getchar();
  system("clear");
  printf("HTTPd version for %s\n\n", argv[1]);

  send(sock, "HEAD / HTTP/1.0\n\n",17,0);

  recv(sock, buffer, sizeof(buffer),0);
  printf("%s",buffer);
  close(sock);
  printf("\n\t [ Search bugs CGI press intro...... ]\n");
  getchar();
  system("clear");
  printf("Searching.....\n\n");

    while(h3z++ < 218)
{
    sock=socket(AF_INET, SOCK_STREAM, 0);
    bcopy(he->h_addr, (char *)&sin.sin_addr, he->h_length);
    sin.sin_family=AF_INET;
    sin.sin_port=htons(80);
    if (connect(sock, (struct sockaddr*)&sin, sizeof(sin))!=0)
    {
      perror("connect");
    }

   for(raza=0;raza < 20;raza++) //counter

    {
      	cgibuff[raza] = '\0';
    }

    send(sock, buff[h3z],strlen(buff[h3z]),0);
    recv(sock, cgibuff, sizeof(cgibuff),0);

    raregazz = strstr(cgibuff,msg);

    if( raregazz != NULL)
    {
     printf("%s : ",bug[h3z]);
     printf("  k00l !! :X\n");++alt3kx;
    }
     close(sock);
  }

  if (alt3kx)
   {
    printf("\nFinish scan for %s\n", argv[1]);
    printf("\n...GOod L00k... :-)\n\n");
   }
  else printf ("\n...hmm...sucks!!!\n\n");

} //void main
