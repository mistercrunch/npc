#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

#include <mcrypt.h>

#define MAX_KEY_SIZE	32

static MCRYPT chandler;		/* crypto handler */

void SABBIA_key_init(char *fname)
{
	FILE *keyf =fopen(fname, "r");
	unsigned char keyblock[MAX_KEY_SIZE];
	struct stat st;
	int i, x =0;

	if(keyf ==NULL)
	{
		printf("unable to open key file %s on key directory!:%s\n",
			fname, strerror(errno)
		);
		exit(errno);
	}

	if(fstat(keyf->_fileno, &st) ==-1)
	{
		printf("unabale to stat open file (%s) ??? :%s\n",
			fname, strerror(errno)
		);
		exit(errno);
	}

	for(i =0; i < MAX_KEY_SIZE; i++)
	{
#define TBLK	8
		unsigned char tinyblock[TBLK];

		fseek(keyf, SEEK_SET, x);

		fread((void *)&tinyblock, 1, TBLK, keyf);

		keyblock[i] =tinyblock[TBLK -1];

		x =((x * (tinyblock[(i +1) % TBLK] + tinyblock[(x +1) % TBLK])) + (x +i));
		x = (x % st.st_size) -8;

		/* 4 is 4 */
		if(x < 0)
			x =tinyblock[4];

	}
	fclose(keyf);

	/* lib mcrypt serpent on few safe cipher mode! :) */
	chandler =mcrypt_module_open("serpent", NULL, "ecb", NULL);
	mcrypt_generic_init(chandler, keyblock, MAX_KEY_SIZE, NULL);


}

#define PAGEBLOCK	4096
unsigned char *encrypt_block(unsigned char *pt, int len)
{
	mcrypt_generic(chandler, pt, len);

	return pt;
}

unsigned char *decrypt_block(unsigned char *ct, int len)
{
	mdecrypt_generic(chandler, ct, len);

	return ct;
}
