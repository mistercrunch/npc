// Este programa es un simple ejemplo para que se vea la facilidad de trabajo
con las entradas del puerto paralelo

// Se definen la librerias: tipico :)
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <io.h>
#include <dos.h>
#include <process.h>
// Se definen las variables y puerto a utilizar
int lpt1;
int valor,mascara[7],resul,resul2,i,total[4];
#define data (lpt1)
#define estado (lpt1 + 1)
#define ctrl (lpt1 + 2)
main ()
{
	clrscr();
           // Se define las mascaras que usaremos para saber el bit que esta
activado
	mascara[0] = 64;
	mascara[1] = 32;
	mascara[2] = 16;
	mascara[3] = 8;
           // Definimos la direccion del puerto paralelo
	lpt1=peek(0x40,0x08);
           // Comprobamos que esa direccion existe
	if(lpt1 ==0)
	printf("No ahy puerto paralelo melon xDD");
	valor = 0;
           // Comienza el bucle para comprobar los bits activos
	do
	{
                      // Se van mostrando por pantalla los bits que estan
activos
                      // Esto solo se hace para saber lo que estamos metiendo
por las lineas de entrada pero se puede omitir
		delay(50);
		textcolor(2);
		valor=inportb (estado);
		resul=valor&mascara[0];
		if (resul==0)
			cprintf("El bit 1 esta inactivo\n\r");
		else
			cprintf("EL bit 1 esta activo\n\r");
		total[0]= resul;
		resul=valor&mascara[1];
		if (resul==0)
			cprintf("El bit 2 esta inactivo\n\r");
		else
			cprintf("EL bit 2 esta activo\n\r");
		total[1]= resul;
		resul=valor&mascara[2];
		if (resul==0)
			cprintf("El bit 3 esta inactivo\n\r");
		else
			cprintf("EL bit 3 esta activo\n\r");
		total[2]= resul;
		resul=valor&mascara[3];
		if (resul==0)
			cprintf("El bit 4 esta inactivo\n\r");
		else
			cprintf("EL bit 4 esta activo\n\r");
		total[3]= resul;
		resul=valor&mascara[4];
		clrscr();
		resul2=0;
		for(i=0;i<=3;i++)
		{
			printf("%d",total[i]);
			resul2=resul2+total[i];
		}
                      // Muestra en decimal el numero introduccido
		printf("\n\rNumero total: %d\n\r",resul2);
                      // Dependiendo del numero metido mira que instruccion
ejecutar
                      /* Para cambiar el programa a ejecutar solo hay que variar
la instruccion que hay entre comillas
                      despues de cada system */
		if (resul2==64){
		system("dir");
		exit(-1);}
		if (resul2==32){
		system("edit");
		exit(-1);}
		if (resul2==16){
		system("cd c:\dos");
		system ("cd..");
		system("edit autoexec.bat");
		exit(-1);}
		if (resul2==8){
		system ("cd c:\dos");
		system("cd..");
		system("edit config.sys");
		exit(-1);}
		if (resul2==24){
		system("dir/p");
		exit(-1);}
		if (resul2==56){
		system("cls");
		exit(-1);}
	}while (kbhit()==0);
}

// Fin del programa