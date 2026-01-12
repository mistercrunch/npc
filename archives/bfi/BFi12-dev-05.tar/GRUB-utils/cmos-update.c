/*    __   ___   _
     /  \ |   \ | | /\-----------< cmos-update.c >-------------------------->
    / /\ \| |\ \| |/ /  < Allegato a GRUB FallBack Patch >
   / /__\ \    /|   /   
   \______/ |\ \| | \   Azzera il Flag del kernel eseguito  
<---------|_| \_\_|\_\  

                        $ gcc cmos-update.c -o cmos-update
*/

#include <stdio.h>

#define DEVILCE "/dev/nvram"
#define CMOS_ADDR 0x6a

main()
  {
     FILE *x;
     char B;
     int  FB_default, FB_fallback, FB_ndefault, FB_nfallback, FB_nlast;
     x = fopen (DEVILCE, "r+");
     rewind(x);
     fseek(x, CMOS_ADDR,SEEK_CUR);
     fread (&B, 1, 1, x);

     FB_default= B & 0x1;
     FB_fallback= (int) (B & 0x2)>>1;
     FB_ndefault= (int) (B & 0xc)>>2;
     FB_nfallback= (int) (B & 0x30)>>4;
     FB_nlast= (int) (B & 0xc0)>>6;

     if (FB_ndefault == FB_nlast)
       {
          FB_default = 0;
       }

     if (FB_nfallback == FB_nlast)
       {
          FB_fallback = 0;
       }


     B = (char) FB_default | (char) FB_fallback <<1 | (char) FB_ndefault <<2 | (char) FB_nfallback <<4 | (char) FB_nlast <<6;

     printf("CMOS Byte Updated: %x\n", B);

     rewind(x);
     fseek(x, CMOS_ADDR,SEEK_CUR);
     fwrite (&B, 1, 1, x);

     fclose (x);
}

















