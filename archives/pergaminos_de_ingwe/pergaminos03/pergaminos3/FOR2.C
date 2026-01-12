#include<stdio.h>
#include<conio.h>
main()
{
 int i,nota,rep=0,reg=0,prom=0;
 /*rep, reg y prom sirven como contadores, por eso deben ser inicializados*/
 clrscr();
 for(i=1;i<=10;i++)
 {
  printf("Ingrese la nota del alumno de legajo %d: ",i);
  scanf("%d",&nota);
  switch(nota)
  {
   case 1:
	  printf("Reprob¢\n");
	  rep++; /*Le suma 1 a rep*/
	  break;
   case 2:
	  printf("Regulariz¢\n");
	  reg++;
	  break;
   case 3:
	  printf("Promocion¢\n");
	  prom++;
	  break;
   default:
	   printf("Te dije que de 1 a 3 gil\n");
	   break;
  }
 }
 printf("Cantidad de reprobados: %d\n",rep);
 printf("Cantidad de regulares: %d\n",reg);
 printf("Cantidad de promocionados: %d\n",prom);
 getch();
}