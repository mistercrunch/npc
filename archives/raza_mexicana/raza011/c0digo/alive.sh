#!/bin/bash
#########################################################################
## ALiVE v1.0                                                          ##
##                                                                     ##
## A little 'hax0r' utility (hehe) to check what machines are alive    ##
## on a network (of 'C' classes).                                      ##
##                                                                     ##
##-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-##
## C0DED BY [Hellraiser] under the GNU/GPL user terms.                 ##
## http://www.raza-mexicana.org                                        ##
## hell@raza-mexicana.org                                              ##
#########################################################################

# -------------------[ VaRiAbLe SeTtInGs ]----------------------------- #
NMAP=/usr/bin/nmap
# --------------------------------------------------------------------- #

#### COLORES ######
ROJO="[0;31m"
AZUL="[0;34m"
VERDE="[0;32m"
GRIS="[0;37m"
VERDE2="[1;32m"
AMARILLO="[1;33m"
BLANCO="[1;37m"
DEFAULT="[0;39m"
BLINK="[5m"
###################

# -------------------[ PrOgRaM ]-------------------------------------- #

if [ $# != 1 ]; then
    echo -n "$VERDE                               ALiVE$ROJO 1.0$DEFAULT"    
    printf "\n\n"
    echo "$BLANCO Usage: $0 [ip] (C class only)$DEFAULT"
    echo "$BLANCO Example: $0 192.168.0  (Scans the C class network)$DEFAULT"
    printf "\n"
    echo "$AMARILLO      Programmed by$VERDE2 [Hellraiser]$BLANCO ->$GRIS$BLINK hell@raza-mexicana.org$DEFAULT"
    echo "$AMARILLO    This program runs under the$BLANCO GNU/GPL$AMARILLO license.$BLANCO ($AZUL c$BLANCO ) 2000$DEFAULT"
    exit
elif [ $# = 1 ]; then
    echo "$VERDE                               ALiVE$ROJO 1.0$DEFAULT"    
    printf "\n"
    echo "$AMARILLO Results:"
    printf "\n"
    echo -n "$BLANCO"
    $NMAP -sP $1.* | grep $1
    echo -n "$DEFAULT"
    printf "\n"
    echo "$AMARILLO      Programmed by$VERDE2 [Hellraiser]$BLANCO ->$GRIS$BLINK hell@raza-mexicana.org$DEFAULT"
    echo "$AMARILLO    This program runs under the$BLANCO GNU/GPL$AMARILLO license.$BLANCO ($AZUL c$BLANCO ) 2000$DEFAULT"
    exit
fi
