#include<stdio.h>
#include<conio.h>
main()
{
 int i;
 clrscr();
 for(i=0;i<=10;i++) /*Dice esto que se empieza de 0 y se termina en 10*/
 {
  printf("El nuevo n£mero es %d\n",i);
  printf("Faltan %d vueltas\n",10-i);
 }
 getch();
}