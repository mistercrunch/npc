#!/usr/bin/perl
# Cart32.exe Unencrypter by Yo_Soy <yo_soy@raza-mexicana.org>
# http://www.raza-mexicana.org - <staff@raza-mexicana.org>
#
print ("\nCart32.exe Password Unencrypter by Yo_Soy - <yo_soy\@raza-mexicana.org>\n");
print ("Based on Xato Security Advisory XATO-112000-01.txt - <www.xato.net>\n\n");
print ("Encrypted code ----> ");

$cadena = <STDIN>; chomp ($cadena);
die ("WRONG CODE\n") if (length($cadena) < 13);

$plano = chr(ord(substr($cadena,  7)) - 12) .
         chr(ord(substr($cadena,  4)) -  8) .
         chr(ord(substr($cadena,  2)) - 16) .
         chr(ord(substr($cadena, 14)) - 15) .
         chr(ord(substr($cadena,  8)) -  9) .
         chr(ord(substr($cadena,  0)) - 12) .
         chr(ord(substr($cadena,  3)) -  3) .
         chr(ord(substr($cadena, 10)) -  5) .
         chr(ord(substr($cadena, 12)) - 11) .
         chr(ord(substr($cadena,  5)) -  5) .
         chr(ord(substr($cadena,  1)) -  1) .
         chr(ord(substr($cadena,  1)) -  1) .
         chr(ord(substr($cadena, 13)) - 13) .
         chr(ord(substr($cadena, 11)) - 10) .
         chr(ord(substr($cadena,  9)) -  6) .
         chr(ord(substr($cadena,  6)) -  8);

print ("Unencrypted code --> $plano\n");
