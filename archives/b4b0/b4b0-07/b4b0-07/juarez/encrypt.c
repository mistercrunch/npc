/* 
|-----------------------------------------------------------------------|
| Encrypt.c 
| By tragen, coolny@geocities.com
| Please mail me if you find any bugs, or problems
| To use my program you have to use this command line :
| ./encrypt <input_file> <output_file> <numral_passcode(ex 4523)> <e> or <d> depending if you want to <e>ncrypt or <d>ecrypt
|------------------------------------------------------------------------| 
*/
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

main (int argc, char *argv[])
{
long int e,a,a2,rannum;
int infile, outfile, count;
unsigned long int key, key1, key2, key3, key4, key5, key6, key7, key8, key9,key2i;
unsigned long int key10,key11,key12,ss,sz;
long unsigned int i = 0;
int ds;
char keystr;
char myin[1];
char myout[1];
char *ifile, *ofile;

if(argc<4) {
	printf("\n Coded By Tragen, bugs/comments goto coolny@geocities.com \n");
	printf("\n usage: ");
	printf(argv[0]);
	printf(" <infile> <outfile> <numral password> <e> for encrypt <d> for decrypt \n");
	exit(22);
	}

ifile=argv[1];
ofile=argv[2];
key = atoi(argv[3]);
if(key>100000000) {
	printf("Numral password too big, shorten the length \n");
	exit(23);
}
ds=*argv[4];
sz = key * 10;
ss = key * key;

infile = open(ifile, O_RDONLY);
outfile = open(ofile, O_RDWR | O_CREAT | O_TRUNC, 0777);
i = 1;
if (ds == 101)
{
	
	printf("Encrypting ...\n");
	key1=ss-sz+4;
	key2=ss-4*key;
	key3=-ss+2*key+5;
	key4=ss-6*key+6;
	key5=2 * ss - 4;
	
	
}
else {
	
	printf("Decrypting ...\n");
	key1=-ss+sz-4;
	key2=-ss-4*-key;
	key3=ss+2*-key-5;
	key4=-ss-6*-key-6;
	key5=-2 * ss + 4;

}
i = 0;	

count=1;

while (( read (infile, myin, sizeof(myin))) > 0) {

if (ds == 101)	{
	i++;
      i = i + count;
}
else {
	i--;
	i = i - count;
}


	
	switch (count) {
	case 1:
		key1 = key1 +i ; 
		myout[0] = myin[0]+key1;
		count++;
		break;
	case 2:
		key2 = key2 +i ;
		myout[0] = myin[0]-key2;
		count++;
		break;
	case 3:
		key3 = key3 +i ;
		myout[0] = myin[0]+key3;
		count++;
		break;
	case 4:
		key4 = key4 +i ;
		myout[0] = myin[0]-key4;
		count++;
		break;
	
	case 5:
		key5 = key5 + i ;
		myout[0] = myin[0]+key5;
		count=1;
		break;
	
} 
	write(outfile, myout, sizeof(myout));
	}
if (ds == 101)	{
	printf("\nDone Encrypting...");
	printf("\n Encrypted %s", ifile);
	printf(" to %s", ofile);
	printf("\n");
	
}
else {
printf("\nDone Decrypting...");
	
	printf("\n Decrypted %s", ifile);
	printf(" to %s", ofile);
	printf("\n");
}
}

