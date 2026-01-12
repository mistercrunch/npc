#include<stdio.h>
#include<conio.h>
main()
{
 int opc;
 clrscr();
 printf("Ingrese una opci¢n del 1 al 5: ");
 scanf("%d",&opc);
 switch(opc)
 {
  case 1:
	 printf("Esta es la primer opci¢n.");
	 break;
  case 2:
	 printf("Esta es la segunda opci¢n.");
	 break;
  case 3:
	 printf("Esta es la tercer opci¢n");
	 break;
  case 4:
	 printf("Esta es la cuarta opci¢n");
	 break;
  case 5:
	 printf("Esta es la quinta y £ltima opci¢n");
	 break;
 /* default:
	  printf("Las opciones son del 1 al 5 forro ¨Qu‚ apretas gil?");
	  break;*/
 }
 getch();
}