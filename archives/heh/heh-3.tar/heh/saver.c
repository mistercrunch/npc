/* 
Ahora, ya tenes con que divertirte el fin de semana...
	    HeH Ezine - Screen Saver 
  Codeado por Zomba (zombaq@ciudad.com.ar)
Para compilarlo= $ gcc saver.c -o saver -lncurses
Led Flashing effect taken from "Console Ioctls Under Linux" by Matt Conover
*/
#include<stdio.h>
#include<ncurses.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<signal.h>
#include<sys/stat.h>
#include<linux/kd.h>
#include<sys/types.h>
#include<sys/ioctl.h>
#define ERROR -1
int fd;
void sighandler(int signum);

main()
{
WINDOW *w;
int b,c,i,h,x,z;
initscr();
noecho();
nodelay(stdscr, TRUE);
curs_set(0);

if((fd= open("/dev/console", O_NOCTTY)) == ERROR) {
    printw("Error al abrir el /dev/console");
    exit(ERROR);
    }
signal(SIGINT, sighandler);
signal(SIGTERM, sighandler);
signal(SIGQUIT, sighandler);
signal(SIGTSTP, sighandler);
printw("	                 http://www.digitalrebel.net/heh");
while(1) {
  for(i=0x01; i <=0x04; i++) {
  if(i==0x03) continue;
  if((ioctl(fd, KDSETLED, i))==ERROR) {
  printw("error en ioctl");
  close(fd);
  exit(ERROR);
    }
   for(h=1;h<=6;h++) {
     b=1+(int) (24.0*rand()/(RAND_MAX+1.0));
     c=0+(int) (80.0*rand()/(RAND_MAX+1.0));
     x=10+(int) (24.0*rand()/(RAND_MAX+1.0));
     z=3+(int) (80.0*rand()/(RAND_MAX+1.0));
     w=newwin( z, x, b, c);
     mvwprintw(w, 1, 2, "HEH Ezine");
     box(w,ACS_VLINE, ACS_HLINE);
     usleep(500);
     wrefresh(w);
     delwin(w);
     if(getch() >0) {
       endwin();
       system("clear");
       ioctl(fd, KDSETLED, 0x00);
       close(fd);
       exit(0);
       }
       
    refresh();
     }
   }
}
close(fd);
endwin();
}     
void sighandler(int signum) {
    if((ioctl(fd, KDSETLED, 0x0))==ERROR) {
     printw("error en ioctl");
     close(fd);
     exit(ERROR);
     }
     close(fd);
     exit(0);
}
     