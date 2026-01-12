#!/bin/sh
# this shell script manage option because:
# - sabbia port must be pseudo randomly generated
# - key directory is virtually linked
# - anonymous peer has address resolved
# - run openvpn before sabbia software

if [ ! $4 ]; then
	echo -e "I require some argument:"
	echo -e "1) ip address or hostname of remote peer"
	echo -e "2) number of packet each second"
	echo -e "3) unique number identify of local peer"
	echo -e "4) unique number identify of remote peer"
	echo -e "\nTHIS IS TEST-DEMO-ALPHA of SABBIA anonymous protocol"
	echo -e "here is not support any defense from active attack"
	echo -e "\ncode and protocol study: vecna@s0ftpj.org - www.s0ftpj.org\n"
	echo -e "this is fast code and lazy implementation: killall openvpn!\n"
	exit;
fi

check=`echo $1 | grep '[a-z]'`
if [ $check ]; then
	ipaddr=`host $1 | awk {'print $4'}`
else
	ipaddr=$1
fi

lport=$RANDOM
rport=$RANDOM

echo "starting software, I'm killing old openvpn and SABBIA instance"
killall SABBIAex 2>/dev/null
killall openvpn 2>/dev/null

echo "starting SABBIA example software...\n"
./SABBIAex $rport $2 $ipaddr $((6000+$3)) $((6000+$4)) &


echo "starting openvpn..."
#openvpn --remote 127.0.0.1 --proto tcp-client --cipher none --lport $lport --rport $rport --dev tun$4 --ifconfig 10.123.123.$3 10.123.123.$4 --daemon --ping 360000
openvpn --remote 127.0.0.1 --proto udp --cipher none --lport $lport --rport $rport --dev tun$4 --ifconfig 10.123.123.$3 10.123.123.$4 --daemon --ping 360000


# as local vpn port is endpoint VPN, for this reason I'm using $rport
echo "done!"
