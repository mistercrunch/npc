/*  Extracted from CatHack #007 ( www.sindominio.net/cathack )   */

/*
 *  vmcrack.c -- exemple de crack LD_PRELOAD per a cathack#07
 *  Autor: azathoth <izzu@linuxupc.upc.es>
 *  (c) 2001 CatHack!
 */

/*
 *  Descripció
 *
 *  Aquest crack intercepta les crides a les rutines time() i gettimeofday() de
 *  la llibreria libc i modifica el seu comportament lleugerament. La demostració
 *  esta escrita per l'aplicació vmware. El crack en si es bastant generic i
 *  hauria de funcionar amb la majoria d'aplicacions que empren llicencies
 *  temporals
 */

/*
 *  Us:
 *
 *  1) creeu el fitxer /etc/vmstart amb els continguts produits per la comanda
 *     'date -d any/dia/mes +%s' a on any/dia/mes es la data que es va adquirir
 *     la llicencia temporal
 *
 *  2) Carregueu aquesta llibreria amb la variable d'entorn LD_PRELOAD així
 *     LD_PRELOAD=/full_path_to/vmcrack.so vmware
 *
 *  Si voleu evitar que la llibreria escrigui a la tty definiu la variable
 *  d'entorn VMQUIET
 */

/*
 *  Compilar:
 *  gcc vmCrack.c -o vmCrack.so -shared -nostartfiles -ldl
 */

/*
 *  Comentaris:
 *  - aquest software és per a ús educatiu
 *  - l'autor no es responsable de l'ús que s'en fagi
 *  - recordeu sempre de pegar una ullada a qualsevol codi que us baixeu de la xarxa
 *    abans d'emprar-lo
 *  - per als curiosos, el funcionament d'aquesta llibreria l'he comprovat
 *    emprant una llicencia vàlida
 */

/* USE AT YOUR OWN RISK! */

#include <dlfcn.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


static void *libc        =   NULL;
static long time_offset  =   0;
static FILE *log         =   NULL;

/* identificació a nivell ELF :-) */
static const char ident[] __attribute__ ((section(".catHack"))) = "azathoth";

int _init(int argc, char *argv[])
{
  FILE *stlic;
  char *stlicfile;

  if(getenv("VMQUIET"))
    log = fopen("/dev/null", "w");
  else
    log = stderr;

  fprintf(log, "usant vmcrack.so -- per azathoth <izzu@linuxupc.upc.es> \n");
  fprintf(log, "aquest software és per a ús educatiu!!!\n");
  fprintf(log, "consulta la teva llicencia (ELA) per a qüestions legals\n");
  fprintf(log, "(c) 2001 CatHack!\n");

  stlicfile = getenv("VMSTARTFILE");
  if(!stlicfile)
    stlicfile = "/etc/vmstart";

  stlic = fopen(stlicfile, "r");
  if(!stlic){
    fprintf(log, "vmCrack: error al intentar obrir el fitxer: %s\n", stlicfile);
    _exit(1);
    }

  libc = dlopen("libc.so.6", RTLD_LAZY);


/*********************** computem la nova data virtual ************************/

  {
  time_t vt; /* temps virtual */
  int result;
  time_t ct;
  struct tm * bt;

  result = fscanf(stlic, "%u", &vt);

  /* afegim un dia per assegurar que no acabem emprant una data anterior al dia
   * d'installació. Aixó no es un problema donat que la llicencia es per uns 40 dies */
  vt += 60*60*24;

  /* vt ara conté la data que anem a simular */
  ct = time(NULL);

  /* i ara ct conté el temps real (sense offset) */
  time_offset = vt - ct;

  }


/*****************************************************************************/

}


/* aqui comença la resolució aritraria de simbols */


/****************************** time() *******************************/

static time_t __init_time(time_t *);
static time_t (*__libc_time)(time_t *) = __init_time;


static time_t __init_time(time_t *buffer)
{
  __libc_time = (time_t (*)(time_t *)) dlsym(libc, "time");
  return __libc_time(buffer);
}


time_t time(time_t *buffer)
{
  time_t ret = __libc_time(buffer);
  ret += time_offset;
  if(buffer)
    *buffer = ret;
  return ret;
}


/****************************** gettimeofday() *******************************/

static int __init_gettimeofday(struct timeval *, struct timezone *);
static int (*__libc_gettimeofday) (struct timeval *, struct timezone *) = __init_gettimeofday;


static int __init_gettimeofday(struct timeval *tv, struct timezone *tz)
{
  __libc_gettimeofday = (int (*)(struct timeval *, struct timezone *)) dlsym(libc, "gettimeofday");
  return __libc_gettimeofday(tv, tz);
}


int gettimeofday(struct timeval *tv, struct timezone *tz)
{
  int ret = __libc_gettimeofday(tv, tz);

  tv->tv_sec += time_offset;
  return ret;
}

int __gettimeofday  (struct timeval *, struct timezone *) __attribute__ ((alias("gettimeofday")));

