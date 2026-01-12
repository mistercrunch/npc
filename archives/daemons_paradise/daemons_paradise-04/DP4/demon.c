/*
 * Demon v2.0 by MORTIIS <mortiislord@iname.com>
 * 
 * Daemon's Paradise e-zine <daemons_paradise@iname.com>
 *
 */

#include <stdio.h>
#include <curses.h>
#include <errno.h>
#include <math.h>

void presentacion();
void mostrar_archivo(char *, char *,char *);
void leer_archivo(void);
void mostrar_menu(void);
void test_terminal(void);
void salida_programa(void);
static void cursor_off() { printf("%c[?25l",27); }
static void cursor_on() { printf("%c[?25h",27); }


 int numero_ezine;
 int numero_articulos;
 struct articulo
 {
  char fichero[9];
  char titulo[32];
  char autor[32];
  char email[32];
  char dificultad[16];
  char tema[16];
  char comentario[48];
 };

struct articulo *articulos;
SCREEN *pantalla;



main(int numarg,char **argum)
{

 
 initscr();
 cursor_off();
 test_terminal();
 start_color();
 noecho();
 cbreak();
 nonl();
 

 init_pair(1,COLOR_WHITE,COLOR_BLACK);
 init_pair(2,COLOR_RED,COLOR_BLACK);
 init_pair(3,COLOR_BLACK,COLOR_RED);
 init_pair(7,COLOR_BLACK,COLOR_YELLOW);
 init_pair(4,COLOR_YELLOW,COLOR_BLACK);
 init_pair(5,COLOR_CYAN,COLOR_BLACK);
 init_pair(6,COLOR_GREEN,COLOR_BLACK);


 leer_archivo();
 presentacion();
 mostrar_menu();
 salida_programa();

}
void salida_programa(void)

{
free(articulos);
endwin();
cursor_on();
}

void test_terminal(void)

{
 char *terminal;

 printw("\nChequeando el tipo de terminal...\n\n");
 terminal=(char *)getenv("TERM");
 printw("Esta utilizando el terminal %s\n",terminal);
 printw("\n ·Número de columnas: %d\n",COLS);
 printw(" ·Número de líneas: %d\n",LINES);
 refresh();
 sleep(1);

 if ( COLS < 80 || LINES < 25 || has_colors()==0 )
	{
	printw("    La terminal debe ser 80x25 (como mínimo) y permitir colores.\n\t Pruebe con la terminal linux.\n");
	refresh();
	endwin();
	cursor_on();
	exit(0);
	}

}

void presentacion()

{
WINDOW *pres;

pres=newwin(LINES,COLS,0,0);
wclear(pres);
wattrset(pres,COLOR_PAIR(6));
mvwprintw(pres,0,0,"
       _/_/_/
      _/    _/                                                   _/
     _/      _/                                                 _/
    _/       _/ _/_/_/_/ _/_/_/ _/_/  _/_/ _/_/_/_/ _/_/    _/   _/_/_/_/
   _/       _/ _/    _/ _/     _/ _/_/ _/ _/    _/ _/ _/   _/   _/
  _/      _/  _/___ _/ _/_/_/ _/  _/  _/ _/    _/ _/  _/  _/   _/_/_/_/
 _/     _/   _/    _/ _/     _/      _/ _/    _/ _/   _/ _/         _/
_/_/_/_/    _/    _/ _/_/_/ _/      _/ _/_/_/_/ _/    _/_/   _/_/_/_/

       _/_/_/_/_/
      _/      _/
     _/      _/
    _/      _/  _/_/_/_/ _/_/_/_/ _/_/_/_/ _/_/_/   _/ _/_/_/_/ _/_/_/_/
   _/_/_/_/_/  _/    _/ _/    _/ _/    _/ _/   _/  _/ _/       _/
  _/          _/___ _/ _/_/_/_/ _/___ _/ _/    _/ _/ _/_/_/_/ _/_/_/_/
 _/          _/    _/ _/_/     _/    _/ _/   _/  _/       _/ _/
_/          _/    _/ _/  _/_/ _/    _/ _/_/_/   _/ _/_/_/_/ _/_/_/_/
");

wrefresh(pres);

 sleep(1);
 wattrset(pres,COLOR_PAIR(4));
 wclear(pres);
 mvwprintw(pres,LINES/5,COLS/16,"
	`Si el cerebro humano fuese tan simple que pudiesemos entenderlo,
	 entonces seríamos tan simples que no podríamos entenderlo.'

	   					         ANONIMO

");
 wrefresh(pres);
 wgetch(pres);
 delwin(pres);


}


void leer_archivo(void)

{
 int i,k;
 FILE *datos;
 int j; 
 datos = fopen("demon.dat","r");
 fscanf(datos,"%d",&numero_ezine);
 fscanf(datos,"%d",&numero_articulos);

 articulos=(struct articulo*)malloc(numero_articulos*sizeof(struct articulo)); 
 i=0;
 while (i < numero_articulos) 
 {

  fscanf(datos,"%s",articulos[i].fichero);
  articulos[i].fichero[8]=0;
  getc(datos);

  k=0;
  j=getc(datos);
  while (j != 10)
   {
  articulos[i].titulo[k]=j;
    j=getc(datos);
    k++;
   }

  k=0;
  j=fgetc(datos);
  while (j != 10)
  {
  articulos[i].autor[k]=j;
    j=fgetc(datos);
    k++;
  }

   k=0;
  j=fgetc(datos);
  while (j != 10)
  {
  articulos[i].comentario[k]=j;
    j=fgetc(datos);
    k++;
  }

  fscanf(datos,"%s",articulos[i].email);
  fscanf(datos,"%s",articulos[i].dificultad);
  fscanf(datos,"%s",articulos[i].tema);
  i++;
 }
 }


void mostrar_menu()

{


int tecla;
int i;
int donde=0;
WINDOW *ul;
WINDOW *ur;
WINDOW *l;
WINDOW *dr;

while (1)
 { 
 i=0;
 l = newwin(LINES,COLS*3/8,0,0);
 ur = newwin(LINES/5,COLS*5/8,0,COLS*3/8);
 dr = newwin(LINES*4/5,COLS*5/8,LINES/5,COLS*3/8);
 keypad(l,TRUE);
 wattrset(ur,COLOR_PAIR(4));

 mvwprintw(ur,0,5,"    _/_/_/      _/_/_/_/               ");
 mvwprintw(ur,1,5,"   _/   _/     _/    _/     Daemon's   ");
 mvwprintw(ur,2,5,"  _/    _/    _/_/_/_/      Paradise   ");
 mvwprintw(ur,3,5," _/   _/     _/                        ");
 mvwprintw(ur,4,5,"_/_/_/  _/  _/   _/     e-zine Numero %d",numero_ezine);


 wrefresh(ur);
 wrefresh(dr);
 wattrset(l,COLOR_PAIR(1));

   while (i < numero_articulos)
     {
      if (i == donde)
	{
	 wattrset(l,COLOR_PAIR(2));
	 mvwprintw(l,i+2,0,"%s",articulos[i].titulo);
         i++;
	 wattrset(l,COLOR_PAIR(1));
	}
	else
        {  
         mvwprintw(l,i+2,0,"%s",articulos[i].titulo);
         i++;
	}
     
     }
 
      if (donde == numero_articulos) 
        {
         wattrset(l,COLOR_PAIR(2));
         mvwprintw(l,23,0,"SALIR");
         wattrset(l,COLOR_PAIR(1));
        }
 	else mvwprintw(l,23,0,"SALIR");

      wrefresh(l);

 if (donde != numero_articulos)
     {

 wattrset(dr,COLOR_PAIR(6));

      mvwprintw(dr,3,5,"TITULO: ");
      mvwprintw(dr,5,5,"TEMA: ");
      mvwprintw(dr,7,5,"AUTOR: ");
      mvwprintw(dr,9,5,"E-MAIL: ");
      mvwprintw(dr,11,5,"DIFICULTAD: ");
      mvwprintw(dr,13,5,"COMENTARIO ");

 wattrset(dr,COLOR_PAIR(5));

      mvwprintw(dr,3,13,"%s",articulos[donde].titulo);
      wclrtoeol(dr);
      mvwprintw(dr,5,11,"%s",articulos[donde].tema);
      wclrtoeol(dr);
      mvwprintw(dr,7,12,"%s",articulos[donde].autor);
      wclrtoeol(dr);
      mvwprintw(dr,9,13,"%s",articulos[donde].email);
      wclrtoeol(dr);
      mvwprintw(dr,11,17,"%s",articulos[donde].dificultad);
      wclrtoeol(dr);
      mvwprintw(dr,15,5,"%s",articulos[donde].comentario);
      wclrtoeol(dr);

      wrefresh(dr);   
    }  
 
 tecla = wgetch(l);
 if ( tecla == KEY_DOWN && donde < numero_articulos) donde++;
 else if ( tecla == KEY_UP && donde > 0) donde--;

 else if (tecla == 13 && donde != numero_articulos)
 {
 delwin(l);
 delwin(dr);
 delwin(ur);
 mostrar_archivo(articulos[donde].titulo,articulos[donde].fichero,articulos[donde].autor);
 }
 else if (donde == numero_articulos && tecla == 13)
  {
   wattrset(l,COLOR_PAIR(6));
   scrollok(l,1);scrollok(ur,1);
   for (i=1;i<13;i++)
   {
    wscrl(l,i);wrefresh(l);sleep(1.6);
   }
   mvwprintw(l,19,0,"        Realizado por:");
   mvwprintw(l,21,0,"MORTIIS mortiislord@iname.com");
   mvwprintw(l,23,0,"  Daemon's Paradise e-zine");
   mvwprintw(l,24,0," daemons_paradise@iname.com"); 
   for (i=1;i<5;i++)
   {
    wscrl(l,i);wrefresh(l);sleep(1.6);
   }
   sleep(4);
   for (i=1;i<7;i++)
   {
    wscrl(l,i);wrefresh(l);sleep(1.6);
   }
  
   wgetch(ur);
   flushinp();
   for (i=1;i<5;i++)
   {
    wscrl(ur,i);wrefresh(ur);sleep(1.6);
   }
   delwin(l);
   delwin(dr);
   delwin(ur);
   return;
  }  
}




 
}


/************************************************************************/
/*	ESTA FUNCION MUESTRA EL ARCHIVO					*/
/************************************************************************/


void mostrar_archivo(char *titulo, char *archiv,char *who)
{


 WINDOW *salida;
 WINDOW *info;
 FILE *archivnum;
 
 char *fich;
 char *init;

 int d,cont,numlin,numtot,itera,pul;
 int *lineas;
 float porc;


 salida =newwin(LINES*22/25,COLS,0,0);
 info = newwin(LINES*3/25,COLS,LINES*22/25,0);

 idlok(info, TRUE);
 scrollok(info, FALSE);
 keypad(info, TRUE);
 idlok(salida, TRUE);
 scrollok(salida, FALSE);
 keypad(salida, TRUE);


wattrset(salida,COLOR_PAIR(1));

if (( archivnum = fopen(archiv,"r")) == NULL)
        {
	  wclear(info);
          wclear(salida);
          mvwprintw(salida,5,5,"Asegurate de que el archivo %s esta en este directorio\n",archiv);
          wrefresh(info);
	  wrefresh(salida);
	  wgetch(salida);
	  return; 
        }

  cont=0;
  numlin=1;
 


while (d != EOF)
{
  d = fgetc(archivnum);
  if (d==10 ) numlin++;
  cont++;
} 

numtot=numlin;
lineas=(int *)malloc((numlin+1)*sizeof(int)); 

rewind(archivnum);

d=1;
lineas[0]=0;
lineas[1]=0;
numlin=2;
cont=0;
while (d != EOF)
 {
  d = fgetc(archivnum);
  if ( d==10 )
		 {
		  lineas[numlin]=cont+1;
		  numlin++;	  
		 } 
  cont++; 
}
 
if ( cont == 1) {

          wclear(info);
          wclear(salida);
          mvwprintw(salida,5,5,"El fichero %s existe pero esta vacio\n",archiv);
          wrefresh(info);
          wrefresh(salida);
          wgetch(salida);
          free(lineas);
          return;

	}
 
 rewind(archivnum);
 fich = (char *)malloc((cont+1)*sizeof(char));
 if (fich == NULL)    {

          wclear(info);
          wclear(salida);
          mvwprintw(salida,5,5,"Error al asignar memoria",archiv);
          wrefresh(info);
          wrefresh(salida);
          wgetch(salida);
	  free(lineas);
          return;
        }


init=fich;
 
*fich=0;
 *fich = fgetc(archivnum);
  do
  {
   fich++;  
   *fich = fgetc(archivnum);
   } while (*fich != EOF);
 *fich=0;
 fclose(archivnum);
 wclear(info);
 wattrset(info,COLOR_PAIR(2));

 mvwprintw(info,1,0,"TITULO: %s",titulo);
 mvwprintw(info,1,34,"AUTOR: %s",who);
 mvwprintw(info,1,60,"TAMAÑO: %d bytes",cont-1);

 wrefresh(info);
  
wclear(salida);
itera=lineas[1];
wattrset(info,COLOR_PAIR(6));

if (numtot < LINES-1)
{

mvwprintw(info,2,37,"-- 100%% --");
while (itera < lineas[numtot])
        {
        wprintw(salida,"%c",init[itera]);
        itera++;
        }
wrefresh(salida);
wrefresh(info);
do pul=wgetch(salida);
while ( pul != 'q' );
} else

{
while (itera < lineas[LINES-5])
	{
	wprintw(salida,"%c",init[itera]);
	itera++;
	}
wrefresh(salida);
numlin=1;


do {

  pul=wgetch(salida);
  if (pul == KEY_UP && numlin != 1) numlin--;
	else if (pul == KEY_DOWN && numlin+LINES-6 < numtot) numlin++;
	else if (pul == KEY_NPAGE && numlin+(2*LINES-8) < numtot) numlin=numlin+LINES-6;
	else if (pul == KEY_PPAGE && numlin >= LINES-4 ) numlin=numlin-LINES+6;
	else if (pul == KEY_PPAGE && numlin < LINES-4 ) numlin=1;
	else if (pul == KEY_NPAGE && numlin+(2*LINES-8) >= numtot) numlin=numtot-LINES+6;


	
 itera=lineas[numlin];
 wmove(salida,0,0);
 while (itera < lineas[numlin + LINES - 6])
        {
        wprintw(salida,"%c",init[itera]);
        itera++;
        }

 mvwprintw(info,2,37,"-- %d%% --",100*(numlin+LINES-6)/numtot);
 wclrtoeol(info);
 wrefresh(info);
 wrefresh(salida);

 } while (pul != 'q');
}

 free(init);
 free(lineas);
 wclear(info);
 wrefresh(info);
 wclear(salida);
 wrefresh(salida);
 delwin(info);
 delwin(salida);
}
