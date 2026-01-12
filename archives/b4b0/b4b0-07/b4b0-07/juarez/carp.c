/* comp4ct sayz : veni vidi vici 
*
*  shoutoutz to #!animalcrackers and b4b0.
*  thnx to qytpo for making me feel like doing something fun.
*
*  $ ./carp > /dev/tty0   	ITZ FUN FOR THE WHOLE FAMILY!
* 
*/

#include <stdio.h>
#include <curses.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	WINDOW *w; 
	int i;
	char *str = "hi";
	char *b = "comp4ct s4yz hi!";

	initscr();
	curs_set(0);
		for(i = 0 ; i < 5000 ; i++) {
			w = newwin(3, strlen(str)+2, (rand() % 23),(rand() % 76));
		    	box(w, ACS_VLINE, ACS_HLINE);
		    	mvwprintw(w, 1, 1, "%s", str);
		    	wrefresh(w);
		}
	memset(w, 0, sizeof(w));
	w = newwin(3, strlen(b)+2, 10 , 30 );
	box (w, ACS_VLINE, ACS_HLINE);
	mvwprintw(w, 1, 1, "%s", b);
	wrefresh(w);
	sleep(4);
	delwin(w);
endwin();
return;
}
