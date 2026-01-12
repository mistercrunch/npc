/*
** DBIN.C - (c) 1998 - Maruja
** Muestra (en forma de codigo para ensamblador) los datos 
** capturados en operaciones IN en orden de aparicion
**
** Modo de uso:  DBIN <file>
** Donde <file> es el nombre del archivo con los datos
**
** IMPORTANTE: LOS DATOS DEBEN SER TODOS BYTES!! (IN AL, XX)
*/


#include <stdio.h>
#include <stdlib.h>


/* Bytes por linea */
#define COLUMNAS		10


/* Teseteo de Flags */
#define TEST_OUT(flag)		(flag & 32768)		/* bit 15 = OUT */
#define TEST_WORD(flag)		(flag & 16384)		/* bit 14 = WORD */
#define GET_PORT(flag)		(flag & 2047)


/* Estructura basica */
typedef struct {

    unsigned int	cs, 
			ip, 
			flags, 
			data;

    } TRAP;



int main (int can, char *arg[])
{
FILE	*f;
TRAP	i;
int	cols,
	datos;


/* Verificar que se encuentre el nombre del file */
if (can < 2) {

	fprintf (stderr, 
		 "%s: Falta el nombre del archivo con los datos capturados\n\r",
		 arg[0]);

	exit (-1);

	}


/* Abrir archivo de datos */
if (! (f = fopen (arg[1], "rb")) ) {

	fprintf (stderr, "%s: ", arg[0]);
	perror (arg[1]);
	exit (-1);

	}


/* Mostrar informacion */
datos = 0;
cols = 0;
for (;;) {

    /* Leer estructura */
    if (!fread (&i, sizeof (TRAP), 1, f)) break;


    /* Solamente si es IN */
    if (!TEST_OUT(i.flags) ) {

	/* Contador de cantidad de INs */
	datos++;

	/* Iniciar linea */
	if (!cols) printf ("\n\t\tdb\t");

	/* Mostrar en decimal */
	printf ("%03u", i.data & 255);
	if (++cols == COLUMNAS) cols = 0;
	else printf (", ");

	}

    }

/* Mostrar cantidad de datos */
printf ("\n\nKEYBUF\t\tEQU\t%u\n", datos);
return 0;
}
