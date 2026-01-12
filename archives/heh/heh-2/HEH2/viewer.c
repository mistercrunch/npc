/****************************************************/
/* HeH! Viewer v1.2 por Zomba (zombaq@ciudad.com.ar)*/
/* Visor de Ezines.                                 */
/* Informacion sobre instalacion, utilizacion, etc  */
/* puede ser encontrada en el archivo leeme.HEH     */
/* Zomba (zombaq@ciudad.com.ar)                     */
/* S.I.D.E. Team                                    */
/* HeH! Ezine: http://www.digitalrebel.net/heh      */
/****************************************************/
#include<stdio.h>
#include<strings.h>
#include<ncurses.h>
#include<termios.h>
WINDOW *mos,*bot;

main()
{

initscr();
noecho();
curs_set(0);
scrollok(mos, TRUE);
clearok(mos, TRUE);
idlok(mos, TRUE);
start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLUE);
    init_pair(2, COLOR_RED, COLOR_BLUE);
    init_pair(3, COLOR_CYAN, COLOR_BLACK);
    init_pair(4, COLOR_WHITE, COLOR_BLACK);
    init_pair(5, COLOR_YELLOW, COLOR_BLACK);
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK);

mos=newwin(21, 80, 0, 0);
bot=newwin(4, 80, 21, 0); 
napms(500);
wrefresh(bot);
wrefresh(mos);
while(1) 
{
wclear(bot);
wclear(mos);
heh();
menu();
wrefresh(bot);
wrefresh(mos);
}

}

/*********************************/
/*         CHAU() Salida         */
/*********************************/

chau()
{
wclear(bot);
wclear(mos);
wrefresh(bot);
wrefresh(mos);

delwin(bot);
delwin(mos);
endwin();
exit(0);
}

/*********************************/
/*           MENU                */
/*********************************/
menu()
{
FILE *fa;
int g=0,a=0, e=0,h=1, vamos, j;
char listado[100][200];
char pasaje[200];
unsigned char tecla;
char min[4];

if((fa=fopen("HEH.art", "r"))==NULL)
    {
    mvwprintw(bot, 1, 1, "Error al abrir el file\n");
    chau();
    }
memset(listado, '\0', sizeof(listado));

while(!feof(fa))
    {
    fgets(listado[g], sizeof(listado), fa);    
    g++;
    }
close(fa);
wclear(bot);
wattrset(bot, COLOR_PAIR(1));
mvwprintw(bot, 0, 0, "                                  -=HEH! Menu=-                                ");
wattrset(bot, COLOR_PAIR(4));
wrefresh(bot);

g--;
mvwprintw(mos, 14,1, "HEH! %d Articulos encontrados.", g);
wrefresh(mos);

g--;
wattrset(bot, COLOR_PAIR(5));
for(a=0;a<=g;a++)
{
mvwprintw(bot, h, e, "[%d] %s", a, listado[a]);
wrefresh(bot);
e=e+16;
if(e>70) 
    { 
     e=0;
     h++;
    }

}

mvwprintw(bot, h, e, "[q] QUIT");
wattrset(bot, COLOR_PAIR(4));

wrefresh(bot);
tecla='\0';
tecla=mvwgetch(bot, 1, 1);
if(tecla=='q') chau();

sprintf(min, "0%c\0", tolower(tecla));

vamos=atoi(min);
strncpy(pasaje, listado[vamos], strlen(listado[vamos])-1);

mostrar(pasaje);
wrefresh(bot);

}

/*********************************/
/*         MENU de archivo       */
/*********************************/

menu_file()
{

wclear(bot);
wattrset(bot, COLOR_PAIR(1));
mvwprintw(bot, 0, 0, "                                  -=HEH! Menu=-                                 ");
wattrset(bot, COLOR_PAIR(5));
mvwprintw(bot, 1,10, " [B]ajar");
mvwprintw(bot, 2,10, " [S]ubir");
mvwprintw(bot, 3,10, " [E]xtraer codigo fuente");
mvwprintw(bot, 1,50, "[V]olver");
mvwprintw(bot, 2,50, "[Q]uitar");

wattrset(bot, COLOR_PAIR(4));

wrefresh(bot);
}
/*********************************/
/*            HEH logo           */
/*********************************/

heh()
{
wattrset(mos, COLOR_PAIR(3));
mvwprintw(mos, 0, 4, ".a$$$$b.     .a$$$$b. .#$$$$$$$$$SSa. .aS$$Sb.     .aS$$Sb.  .a$$b.");
mvwprintw(mos, 1, 4, "$$$$$$$$     $$$$$$$$ $$$$$$$$$$$$$$' $$$$$$$$     $$$$$$$$ $$$$$$");
mvwprintw(mos, 2, 4, "$$$$$$$$     $$$$$$$' $$$$$$$.        $$$$$$$$     $$$$$$$' $$$$$$");
mvwprintw(mos, 3, 4, "$$$$$$$$\\,.,/$$$$$$$  $$$$$$$$        $$$$$$$$     $$$$$$$ $$$$$$'");
mvwprintw(mos, 4, 4, "$$$$$$$$$$$$$$$$$$$$  $$$$$$$$$$$$$$. $$$$$$$$$$$$$$$$$$$$ $$$$$$.");
mvwprintw(mos, 5, 4, "$$$$$$$$$$$$$$$$$$$$. $$$$$$$$$$$$$$' l$$$$$$$$$$$$$$$$$$$. $$$$$$");
mvwprintw(mos, 6, 4, "l$$$$$$$/' `\\$$$$$$$$ l$$$$$$'        l$$$$$$$     $$$$$$$$ $$$$$$");
mvwprintw(mos, 7, 4, "8$$$$$$$     $$$$$$$$ 8$$$$$$$$$$$$$. 8$$$$$$$     $$$$$$$$");
mvwprintw(mos, 8, 4, "`*S$$$S'     `S$$$$S' `*S$$$$$$$$$S*' `S$$$S'      `S$$$$S'  .a$$b.");
mvwprintw(mos, 9, 4, " .------------------------------------------------------.   $$$$$$$");
mvwprintw(mos, 10, 4, "@        HEH! Magazine, weird ideas by weird ppl!        \"$  *S$$$'");
mvwprintw(mos, 11, 4, "\".________________________________________________________$$=.____*'");
mvwprintw(mos, 12, 4, "           `.____________________________.'");
mvwprintw(mos, 13, 10,"      http://www.digitalrebel.net/heh");

wattrset(mos, COLOR_PAIR(6));
mvwprintw(mos, 18, 35, "HEHviewer 1.2v(for Slack) - Hecho por Zomba");
mvwprintw(mos, 19, 62, "(zomb4@yahoo.com)");



wattrset(mos, COLOR_PAIR(4));

wrefresh(mos);
}
/************************/
/*  Mostrar Archivo     */
/************************/

mostrar(char *arch)
{
int a,b=0,c,d,resta;
int g=0;
char hola;
FILE *fd;
char buffer[600][1000];
unsigned char tecla;
char min;
char codigo[100];


tecla='b';
mvwprintw(mos, 16, 1, "Abriendo el articulo [%s] HEH! HEH!", arch);
wrefresh(bot);

if((fd=fopen(arch, "r"))==NULL)
    {

    mvwprintw(bot, 1, 1, "Error al abrir el file %s\n", arch);
    wrefresh(bot);
    chau();
    }

memset(buffer, '\0', sizeof(buffer));
memset(codigo, '\0', sizeof(codigo));

do
    {
    fgets(buffer[g], sizeof(buffer), fd);
    g++;
    mvwprintw(mos, 14,1, "HEH! Procesando %d filas.     ", g);
    wrefresh(mos);
    if(g==600) break;     	
    } while(!feof(fd));

close(fd);
napms(500);
mvwprintw(mos, 0,0, "HEH!HEH!HEH!HEH!");
wrefresh(mos);

while(1)
{
wclear(mos);
min=tolower(tecla);
if(b>g && min!='p') break;

switch(min)
{
case 'q':chau();

case 'e': sprintf(codigo, "./extract %s\n", arch);
	  system(codigo);
	  break;

case 'v': return;
	  break;
case ' ':
case 's' :
        if(b!=21)
	 {
	a=0;
	resta=b-42;
	mvwprintw(bot, 11, 1, "r=%d", resta);
	for(d=resta; d < b; d++)
	    {
	    mvwprintw(mos, a, 1, "%s", buffer[d]);
	    a++;
	    }
	b=d-21;
        //mvwprintw(bot, 13, 1, "a=%d", a);
	//mvwprintw(bot, 12, 1, "d=%d", d);
	//mvwprintw(bot, 10, 1, "b=%d", b);
	  }
	wrefresh(mos);
	break;
case 10: 
case 'b':
for(a=0;a<21; a++)
    {
    
    mvwprintw(mos, a, 1, "%s", buffer[b]);
    wrefresh(mos);
    b++;
    }
    

//mvwprintw(bot, 10, 1, "b=%d", b);
wrefresh(bot);
break;
	    
}


menu_file();
noecho();
curs_set(0);
keypad(bot, TRUE);
raw();
tecla=mvwgetch(bot, 1, 1);

}
}