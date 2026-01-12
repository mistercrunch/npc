# VSKI - Very Simple Kstat Installer
# Author: FuSyS [fusys@s0ftpj.org, fusys@sikurezza.org]
# Tested on: Slackware 10 Linux 2.6.7 #6 i686 unknown

#!/bin/bash

echo -e "\nBigBoo install script"
echo    "======================="
echo ""
read -p "Quale device ext2 fara' da casa a BigBoo ? " EXT2
read -p "Quale device di swap fara' da casa a BigBoo ? " SWAP
echo ""
echo -e "\t--------------------------------"
echo -e "\t|  BigBoo richiede la libreria |"
echo -e "\t|   ext2fs. Se non e' ancora   |"
echo -e "\t| installata, questo e' il tuo |"
echo -e "\t|   momento di premere CTRL-C  |"
echo -e "\t| Altrimenti premi solo ENTER  |"
echo -e "\t--------------------------------"
echo ""
read

gcc -Wall -Werror -lext2fs -o metaboo metaboo.c
./metaboo $EXT2 $SWAP
gcc -Wall -Werror -lext2fs -o BigBoo bigboo.c

# Did someone say MD5 ?!
