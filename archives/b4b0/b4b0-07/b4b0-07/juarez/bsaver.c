/* b4b0-saver.c
*  cp4kt (see-packt) & Qytpo (pee-nis)
*
*  special thanks to w00w00 (Shok mainly) for their/his article on console
*  ioctls. Without that, this would have taken a *lot* longer.
*  This is 93.42 % Qytpos code. i love u guy. p.s. thnx vol
*
*  greets to #!animalcrackers
*
*/

/* over view -- cp4kt
*  
*  basically, its kinda sloppy, but it works. another problem remains however 
*  and that problem is the passwd[] buffer. to compile, do th1z: 
* 
*  # gcc sscr.c -o sscr -lncurses
*
*
*  if that doesn't work, u have a problem. get ncurses fool.
* 
*  this is meant for entertainment purposes only. no real dillmonkies were harmed
*  in the creation of this. thank u. 
*  
* ** SIDE NOTE ** to break, hit enter once. do not hold down the enter key. 
*/

#include <stdio.h>
#include <curses.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <linux/kd.h>
#include <linux/vt.h>
#include <sys/ioctl.h>
#include <signal.h>


#define NUMWORDS 20 	/* number of words in *words[] (not including NULL) */

#define  LOCK(x) 	if (ioctl(fd, VT_LOCKSWITCH, 1) < 0) { 		\
				perror("LOCK malfunction");    		\
					exit(-1); 			\
				}

#define  UNLOCK(x) 	if (ioctl(fd, VT_UNLOCKSWITCH, 1) < 0) {	\
				perror("UNLOCK malfunction");		\
					exit(-1);			\
				}

static char 	passwd[] = "dillmonkey" ;
int 		fd; 

int init_all( void ) 
{ 
  	initscr();
 	 noecho();
 	 nodelay(stdscr, TRUE);
 	 curs_set(0);
 	 start_color();

	signal(SIGINT, SIG_IGN);
  	signal(SIGTERM, SIG_IGN);
  	signal(SIGQUIT, SIG_IGN);
  	signal(SIGTSTP, SIG_IGN);

}

int main(int argc, char *argv[]) 
{
  WINDOW *w;
  int randx, randy;
  int i;
  char *wordz, pbuf[20];
  char *words[] = {
	"DRUGZ!",
	"B4B0 ROCKING THE HOUSE!",
	"NIGGER!",
	"CRACKHEAD!",
	"CUNTLIPS!",
	"FUCKASS!",
	"DILLMONKEY"
	"CRACK!", 
	"TWEAK!", 
	"CRANK!",
	"VODKA!",
        "PENIS!",
	"BOOBS!",
        "SPERM!",
	"PUSSY!",
        "BITCH!",
	"CHINK!",
	"SKANK!",
        "WHORE!",
        "QUEER!",
	"LAMER!",
	"LOSER!",
	"DRUNK!",
  	NULL
  };

  init_all();

  if((fd = open("/dev/tty", O_RDWR)) < 0) {
    perror("cannot open tty");
    exit(-1);
  }
                                                   
  LOCK(fd);

  while(1) {

    srand(time(0));

    wordz = words[rand() % NUMWORDS];

    if(strlen(wordz) > 6) {

      	i = (79 - (strlen(wordz) + 2)); 
	randx = rand() % i;
	randy = rand() % 21; 

     }

     else {

	randx = rand() % 71;
	randy = rand() % 21;	

    }

     w = newwin(3, strlen(wordz) + 2, randy, randx);
       mvwprintw(w, 1, 1, "%s", wordz);
       box(w, ACS_VLINE, ACS_HLINE);
       wrefresh(w);
       sleep(1);
       wclear(w);
       wrefresh(w);
       delwin(w);
	     
       if(getch() > 0) {
         w = newwin(3, 25, 10, 35);
         mvwprintw(w, 1, 1, "password: ");
         box(w, ACS_VLINE, ACS_HLINE);
         wrefresh(w);
	 wgetstr(w, pbuf);
	 wrefresh(w);
	 if((strcmp(passwd, pbuf)) == 0) 
	   break; 
				
	  else 	{	
 	    wclear(w);
	    wrefresh(w);
	    delwin(w);	
	    continue; 
	  }	
      }	
  }	    

  endwin();
  UNLOCK(fd);
  return 0;

}


