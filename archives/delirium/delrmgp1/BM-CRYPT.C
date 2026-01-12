/* Mega-XOR Scrambler by Bluesman@cyberspace.org 12/94

   This scramble function provides good security by doing exclusive ORs (XOR)
   on every data element with multiple sequences of random data generated
   by seeds that are extrapolated from a passphrase. XOR is used so that
   the same function can be used to descramble the data as well as scramble
   it.

   NOTE: This function is compiler specific. That is, the random() function
         is different from compiler to compiler. Data that is ciphered
         with a Turbo C compiled program may not decipher correctly with
         a MicroSoft C compiler. You could include your own random()
         function with this code.. I would supply one, but I'm too lazy
         to look for random functions.

   The file to be scrambled is broken into 4096 byte blocks. Each block
   has n^(8*n) possible combinations where n=number of characters 
   in the passphrase. That's a LOT of combinations.

   Each character in the passphrase increments the seed that is used for
   the random function. Each block is XORed as many times as there are
   characters in the passphrase with the incrementing seed.

   The seed is incremented rather than simply reloaded with the next
   character since the multiple random strings used to XOR the data
   need not be in any particular order. By simply reloading the seed
   with characters from the passphrase, the passphrase "security" will
   produce the same crypt as "ytiruces" (The reverse). Incrementing
   the seed eliminates that problem by generating seeds that are
   unique to the ordering of the passphrase as well as the individual
   characters. 

   A cryptanalysis method to defeat this security would involve selecting
   (n) sequences of random data out of 65536 and XORing the sequences with
   the crypt data. (n=number of characters in passphrase) This would 
   require 65536^n tests to try every combination. If someone were to
   try this method on data scrambled with only a 4 letter passphrase 
   and EACH test only took 1 NANOSECOND, it will still take around
   58,494,241 CENTURIES to try every combination. They would STILL
   have to figure out which combination produced the deciphered data. 
   Most of us don't have that much time to wait. There may be other
   methods to defeat this cipher.. But it's beyond my scope of
   mathematical understanding.

   This source was compiled with Borland C++ 3.1 and should compile with
   most ANSI C compilers.

   Be careful typing in your password! It only asks for it once.. Maybe
   I'll have it verify when I get to it.

*/                            

#include <stdlib.h>           
#include <stdio.h>             
#include <string.h>
#include <conio.h>

void scramble(char *infile, char *outfile, char *passphrase)
{
char block[4097];                   
int seed=0,x,y,size;                  
FILE *in,*out;

in=fopen(infile,"rb");              /* Open infile */
out=fopen(outfile,"wb");            /* Open outfile */

printf("\nCrypting ");


while(1)
    {
    
    size=fread(block,1,4096,in);   /* Read 4096 Bytes from infile  */

    printf("+");

    for(x=0;x!=strlen(passphrase);x++)
        {
        printf(".");
        
        seed=seed+(unsigned int)passphrase[x];  /* Increment the seed with the */
                                                /* numerical value of a        */
                                                /* passphrase character        */

        srand(seed);    /* Set a new random seed */

        for(y=0;y!=size;y++) block[y]=block[y]^random(seed); /* XOR the block */
        }

    fwrite(&block,1,size,out);  /* Write out the scrambled block */

    if(size!=4096) break;       /* Check for the end of the file */

    } 
}


void main(int argc, char *argv[])

{
char passphrase[500];

int x=0;
char pass;
char ch;

if(argc!=3) 
    {
    printf("\nUsage: %s infile outfile\n\n",argv[0]);
    exit(0);
    }

    printf("\nEnter PassPhrase: ");

    do
        {
        ch=getch();
        if(ch==3 || ch==27) { printf("\n\nAborting\n"); exit(1); }
        if(ch==8 && x>=1) { x--; printf("%c%c%c",8,32,8,8); }
        else if(ch!=8 && ch!=13) { printf("#"); passphrase[x++]=ch; }
        } while(ch!=13);
    

    passphrase[x]=0;

scramble(argv[1],argv[2],passphrase);


}

