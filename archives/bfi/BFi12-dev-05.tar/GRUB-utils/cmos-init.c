/*    __   ___   _
     /  \ |   \ | | /\-----------< cmos-init.c >----------------------------->
    / /\ \| |\ \| |/ /  < Allegato a GRUB FallBack Patch >
   / /__\ \    /|   /   
   \______/ |\ \| | \   Inizializza il Byte sulla CMOS in base alle 
<---------|_| \_\_|\_\  inpostazioni contenute in /boot/grub/menu.lst

                        $ gcc cmos-init.c -o cmos-init
*/


#include <stdio.h>

#define rcpath "/boot/grub/menu.lst"

#define DEVILCE "/dev/nvram"
#define CMOS_ADDR 0x6a

int  FB_default=0, FB_fallback=0, FB_ndefault=-1, FB_nfallback=-1, FB_nlast;

void findparm(char *riga)
{
  char *punt, *punt2, oldchar;

  punt=riga;
  while (*punt && *punt != ' ' && *punt != '\t' && *punt != '=') punt ++;

  punt2 = punt;
  while (*punt2 == ' ' || *punt2 == '\t' || *punt2 == '=') punt2 ++;

  oldchar = *punt;
  *punt = 0;

  if (strcmp(riga, "default") == 0) FB_ndefault = atoi(punt2);
  if (strcmp(riga, "DEFAULT") == 0) FB_ndefault = atoi(punt2);
  if (strcmp(riga, "fallback") == 0) FB_nfallback = atoi(punt2);
  if (strcmp(riga, "FALLBACK") == 0) FB_nfallback = atoi(punt2);

}


main()
{
  FILE *rcfile, *x;
  char tag[16], val[64], riga[256], B;


  rcfile = fopen(rcpath, "r");
  if (rcfile == NULL)
    { 
       printf("Can't open GRUB Configuration file (/boot/grub/menu.lst)\n");
       exit(1);
    }
  else
    {  
       while(!feof(rcfile)) 
         {
            fgets (riga, 256, rcfile);
            findparm(riga);
         }
    }

  FB_nlast = FB_ndefault;
  if(FB_nfallback==-1)FB_nfallback=3;
  B = (char) FB_default | (char) FB_fallback <<1 | (char) FB_ndefault <<2 | (char) FB_nfallback <<4 | (char) FB_nlast <<6;

  printf("CMOS Byte Updated: %x\n", B);
  fclose(rcfile);


  x = fopen (DEVILCE, "r+");
  rewind(x);
  fseek(x, CMOS_ADDR,SEEK_CUR);
  fwrite (&B, 1, 1, x);

  fclose(x);
}


