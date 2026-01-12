#!bin/ksh 
#
# [Matando los procesos de nuestros usuarios] 
# [roots@ezkracho.com.ar] [roots@dalnet] 
#
# Tenemos un servidor de cuentas shell y uno de nuestros usuarios esta usando mas de dos procesos # en background en su cuenta de DOS bp's...Una posible solución sería cancelar su account, (lo 
# cual sería perder dinero) o matándole sus procesos para luego avisarle..
# unf.sh es un script en shell cuyo propósito es matar los procesos de nuestros usuarios,  
# enviándoles una señal -15 para luego de tres segundos enviar una señal -9 asi nos asseguramos  # de que todos sus procesos fueron matados. 
#
# Uso: unf usuario id .. remueve todos los procesos de un usuario del sistema.
#

if [ "$#" = "1" ]
then 
  echo "unf requiere que introduzca un usuario"
exit 
fi
#
ps -elf | awk -v usr=$1 ' { if ($3==usr) print $4 } ' xargs -t -i kill -15 {}
sleep 3 
ps -elf | awk -v usr=$1 ' { if ($3==usr) print $4 } ' xargs -t -i kill -9 {}
exit 

# Matar a *TODOS* nuestros usuarios, excepto root.
# if [ $name = "root" ]
# then
#  continue;
# fi
# echo "matando a usuario $name."
# Le enviamos una señal -15 (ps -elf | awk -v usr=$1 ' { if ($3==usr) print $4 } ' xargs -t -i   # kill -15 {} >/dev/null 2>&1
# Luego una señal -9 para estar seguros de que no quedan bp's ..

# Visiten la página de #unixos en dalnet: [http://www.ezkracho.com.ar/unixos] 
