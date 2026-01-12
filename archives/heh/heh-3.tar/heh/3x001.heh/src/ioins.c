/*
** IOINS.C - (c) 1998 - Maruja
** Muestra (en forma entendible) los datos capturados en operaciones IN
**
** Modo de uso:  IOINS <file>
** Donde <file> es el nombre del archivo con los datos
*/


#include <stdio.h>
#include <stdlib.h>


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
printf ("\nArchivo: '%s'", arg[1]);
for (;;) {

	/* Leer estructura */
	if (!fread (&i, sizeof (TRAP), 1, f)) break;

	/* Mostrar en forma humana */
	if (!TEST_OUT(i.flags)) {
		
		printf ("\n%04X:%04X\tIN\t%03X, ", i.cs, i.ip, 
							GET_PORT(i.flags));

		if (TEST_WORD(i.flags))

			printf ("%04X\t(WORD)", i.data);

		else printf ("%02X  \t(BYTE)", i.data & 255);

		}

	}

printf ("\n");
return 0;
}
