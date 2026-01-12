/*
 * wdmon.c - A monitor to watch the watchdog
 *
 * Ascensit Srl - mb@ascensit.com - $Id$
 *
#define __u32 unsigned int
#define __u8  unsigned char
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/watchdog.h>

int time;

#define WD_DEV_NAME "/dev/watchdog"

#define WATCHTIME       45
#define WD_RESTART      30

int main(int argc, char *argv[])
{
  char *prg_name;

  int wd_f;

  /* Get the program name */
  if ((prg_name = rindex(argv[0], '/')) != NULL)
    prg_name++;
  else
    prg_name = argv[0];

  if ((wd_f = open(WD_DEV_NAME, O_RDWR)) < 0) 
    {
      fprintf(stderr, "%s: unable to open `%s'\n", prg_name, WD_DEV_NAME);
      exit(EXIT_FAILURE);
    }
  
  time=WATCHTIME;
  
  if (ioctl(wd_f, WDIOC_SETTIMEOUT, &time) < 0) 
    {
      fprintf(stderr, "%s: cannot setup the timer\n", prg_name);
      exit(EXIT_FAILURE);
    }
  
  /* Loop forever */
  while (1) 
    {
      sleep (WD_RESTART);
      
      if (ioctl(wd_f, WDIOC_KEEPALIVE, &time) < 0) 
	{
	  fprintf(stderr, "%s: cannot reset the timer\n", prg_name);
	  exit(EXIT_FAILURE);
	}
    }
  
  return 0;
  
} /* main */
