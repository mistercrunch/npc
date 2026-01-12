#include<stdio.h>
#include<conio.h>
main()
{
 int var;
 do
 {
  printf("Ingrese un n£mero: ");
  scanf("%d",&var);
  printf("El n£mero ingresado es %d\n",var);
 }while(var!=0);
 getch();
}