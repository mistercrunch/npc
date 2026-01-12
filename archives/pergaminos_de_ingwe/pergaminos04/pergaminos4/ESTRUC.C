/*                         EJEMPLO DE USO DE ESTRUCTURAS
				 IngweSoft by Ingwe
			   http://ingwesoft.tripod.com.ar  */
#include<stdio.h>
#include<conio.h>
struct agenda
{
 char nombre[20];
 char apellido[20];
 int codpos;
};

typedef agenda *ingwe;

void main()
{
 ingwe variable;
 clrscr();
 printf("Ingrese nombre: ");
 gets(variable->nombre);
 printf("Ingrese apellido: ");
 gets(variable->apellido);
 printf("Ingrese codigo postal: ");
 scanf("%d",&variable->codpos);
 printf("\nEl nombre es: %s\n",variable->nombre);
 printf("El apellido es: %s\n",variable->apellido);
 printf("El codigo postal es: %d\n",variable->codpos);
 getch();
}