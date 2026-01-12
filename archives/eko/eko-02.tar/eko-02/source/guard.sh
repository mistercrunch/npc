#!/bin/sh

# guard.sh roots [roots@gotroots.com] http://gotroots.com
# monitors tcpdump for possible DdoS attacks.. then activates ipfwadm
# Copyright (C) March 2001, roots.
# Remember: Mc'Donalds Owns You, And Ronald Is The King!

clear
echo "(guard): welcome to guard. setup will now determine the"
echo "(  ): appropriate interface/s to monitor..."
if [ "`whoami'" != "root" ]; then
 echo "(  ): tcpdump requires root access.. sorry"
 exit 0
fi

ifconfig | grep ppp0 >/dev/null 2>&1
if [ $? -eq 0 ]; then
 echo "(guard): ppp0 is up! (ifconfig | grep ppp0)"
 echo "(  ):    network monitoring started.. "
 iface="ppp0"
fi

ifconfig | grep eth0 >/dev/null 2>&1
if [ $? -eq 0 ]; then
 if [ "$iface" = "ppp0" ]; then
  echo "(guard): hrm.. ppp0 and eth0 seem to be up.. "
  echo "(  ):    network monitoring resumed.. "
  iface="ppp0"
 fi
 if [ "$iface" != "ppp0" ]; then
  echo "(guard): eth0 is up! (ifconfig | grep eth0)"
  echo "(  ):    network monitoring started.."
  iface="eth0"
 fi
fi

echo ""
bval="0"
if [ "$iface" = "" ]; then
 iface="lo"
 echo "(guard): no suitable interfaces found, using lo"
fi
sleep 5
while :; do sleep 1
bval=`expr $bval + 1`
clear
if [ "$bval" = "1" ]; then
 bchar="*    "
fi
if [ "$bval" = "2" ]; then
 bchar="**   "
fi
if [ "$bval" = "3" ]; then
 bchar="***  "
fi
if [ "$bval" = "4" ]; then
 bchar="**** "
fi
if [ "$bval" = "5" ]; then
 bchar="*****"
fi
if [ "$bval" = "6" ]; then
 bchar="**** "
fi
if [ "$bval" = "7" ]; then
 bchar="***  "
fi
if [ "$bval" = "8" ]; then
 bchar="**   "
fi
if [ "$bval" = "9" ]; then
 bchar="*    "
 bval="1"
fi
echo "[$bchar] !x! (guard): monitoring $iface @ `date`"
tcpdump -i $iface 1>.guard.tmp 2>.guard.tmp &
sleep 5
killall -9 tcpdump 1>/dev/null 2>/dev/null
tail .guard.tmp | grep frag >/dev/null 2>&1
if [ $? -eq 0 ]; then
if [ -f .guard2.tmp ]; then
  echo "(guard): possible fragmentation attack detected! (ping -f -s >50000 maybe?)"  
  echo "(  ):     ipfwadm will now deny all ICMP traffic for 10 minutes."
  rm .guard2.tmp
  ipfwadm -I -a deny -P icmp -o -S 0.0.0.0/0 0:65535
  echo "sleep 30" 1>.unf.tmp 2>.unf.tmp &
  echo "ipfwadm -I -a accept -P icmp -S 0.0.0.0/0 0:65535" >>.unf.tmp
  chmod +x .unf.tmp
  ./.unf.tmp 1>/dev/null 2>/dev/null &
  sleep 5
 fi

 if [ ! -f .guard2.tmp ]; then
  echo "(  ): possible fragmentation attack detected.. " 1>.nG2.tmp 2>.nG2.tmp &
  sleep 5
 fi
 if [ -f .nG2.tmp ]; then
  echo "(guard): possible fragmentation attack detected! (ping -f -s >50000 maybe?)."
  echo "(  ):     ipfwadm will now deny all ICMP traffic for 10 minutes."
  rm .guard2.tmp
  ipfwadm -I -a deny -P icmp -o -S 0.0.0.0/0 0:65535
  echo "sleep 30" 1>.unf.tmp 2>.unf.tmp &
  echo "ipfwadm -I -a accept -P icmp -S 0.0.0.0/0 0:65535" >>.unf.tmp
  chmod +x .unf.tmp
  ./.unf.tmp 1>/dev/null 2>/dev/null &
  sleep 5
 fi
fi
done
