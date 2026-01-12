#!/usr/bin/perl
# C0dEd bY Marc0 DaeMonT3
# Proudly Object Oriented Programmer
$reset="\033[0;37m";
if(@ARGV != 1) { die "Uso: $0 archivo1:archivo2:archivo3\n"; }
@args=split(/:/,$ARGV[0]);
for($h=0;$h<=@args;$h++){
open HEX,$args[$h];
while(<HEX>){
print "Anytime hit x or q to quit!\n";
for($a=0;$a<23;$a++){
chop($lala=<HEX>); 
print "\033[1;34m#\033[1;33m#\033[1;37m$lala\n";}
print "\033[2;33m#" x 80;
$q=getc;
if($q eq "q" or $q eq "x" or $q eq "X" or $q eq "Q")
{close HEX; die "$reset Thanks for use HeX! Reader\n";}}
close HEX;}
print "Bye Bye! \n $reset";
