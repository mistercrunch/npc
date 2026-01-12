/*
 *  smap.c <find address in System.map file...>
 *
 *  I found different layout of System.map file, so if its offsets
 *  start whit a number of _zero_, I delete them...
 *  (RedHat 7.x distributions have this special layout...)
 *
 *  110202  ulix <ulix@gmx.it>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>


int main (int argc, char *argv[])
{	
	char *name;
	char b[8192];
	char a[11];
	char f[50];
	FILE *fd;
	if (argc < 2)
                exit (-1);	
	
	name = argv[1];
	name[strlen(name)+1] = '\0';

	if ((fd = fopen (MAP, "r")) < 0) {
		perror ("fopen");
		exit (-1);
	}

	memset(&b, '\0', sizeof(b));	
	memset(&a, '\0', sizeof(a));	
	memset(&f, '\0', sizeof(f));

	while ((fgets(b, sizeof(b), fd)) != NULL) {
		if (strstr(b, name)) {
			if (b[0] != '0')
				snprintf(f, strlen(name) + 5, "%s", b + 11);
			else
				snprintf(f, strlen(name) + 5, "%s", b + 19);

			f[strlen(f)-1] = '\0';

			if ((strlen(f)) == (strlen(name))) {
				if (!strcmp(f, name)) {
					if (b[0] != '0')
						snprintf(a, 11, "0x%s", b);
					else
						snprintf(a, 11, "0x%s", b + 8);
				}
				break;
			}
		}
	}
	printf("%s", a);

	fclose (fd);

	return (0);
}
