/* Libreria VGA320 v1.01b - Para WATCOM C/C++ 11.0
   Coded by toshi[pDX/IF] - Buenos Ayres - Argentina.
   (c) 1998 - 2002 (iNDUSTRIA flASHERA iNC)
   -----------------------------------------------------------

   Funciones soportadas en esta libreria:

   SetVGA();                                    Setea el modo 13h
   SetText();                                   Setea el modo 03h
   setrgb(col, r, g, b);                        Setea las variables col,
                                                r, g y b
   putpixel(x, y, col);                         PUTea un pixel en x, y, y
                                                su color :)
   waitretrace();                               */                                                                                                                         


#include <stdio.h>
#define PI 3.1415927

typedef unsigned char byte;



void waitretrace(void);
        #pragma aux waitretrace=\
        "mov dx,3dah"\
        "wvr1:"\
        "in al,dx"\
        "test al,8"\
        "jnz wvr1"\
        "wvr2:"\
        "in al,dx"\
        "test al,8"\
        "jz wvr2"\
        modify [dx al];

/* setear modo de video 13h y modo texto 03h */


void SetVGA(void);
   
        #pragma aux SetVGA=\
         "mov ax, 13h"\
         "int 10h"\
         modify[eax];

void SetText(void);

        #pragma aux SetText=\
         "mov ax, 03h"\
         "int 10h"\
         modify[eax];



/* Ahora hay que setear la tabla rgb + col
   Esta funcion funcina asi
   " setrgb (col, r, g, b); "
   Es recomendable setear una paleta para que funcionen bien los
   colores

   Ver funcion setpal(pal);
   */


void setrgb (byte col, byte r, byte g, byte b)
{
        outp(0x3C8,col);
        outp(0x3C9,r);
        outp(0x3C9,g);
        outp(0x3C9,b);
}




/* La memoria de video empieza en A000:0000 y termina en A000:F9FF.
   Esto es igual a 63999 en decimal */


/* Para calcular el offset correcto para poner un pixel en cualquier
   ubicacion del eje x , y (es 2d)

   - Multiplicar la coordenada y por 320 (numero de pixeles en una linea)
   - A este resultado, sumarle la coordenada x
   - Usar el resultado final como un pointer hacia la direccion de memoria
     correcta
   - Dropear el valor que va desde 0-255 hasta esta locacion.


   Esto resulta en este algoritmo: offset = y * 320 + x
   La version en ASM de este calculo es mejorcita que la hecha en c

   x = Valor de X en la pantalla
   y = valor de Y en la pantalla
   col = color del pixel que se imprime en la pantalla */

void putpixel(long x,long y,long col);
        #pragma aux putpixel=\
        "mov edi,0a0000h"\
        "mov ecx,eax"\
        "shl eax,8"\
        "shl ecx,6"\
        "add eax,ecx"\
        "add eax,ebx"\
        "mov byte ptr [edi+eax],dl"\
        modify [edi ecx]\
        parm [ebx] [eax] [edx];

/* Para modificar la paleta hay que modificar los puertos 3c8h y 3c9h
   donde en el puerto 3C8H se pone el color con el que se empieza y
   en el puerto 3C9H se manda la info de la paleta.
   La funcion aca abajo es una modificacion de setrgb(); para que
   se pueda obtener un valor -pal- */

void setpal(char*pal)
{
        int i;
        outp(0x3c8, 0); /* empieza con el color 0 */
        for(i = 0; i < 256 * 3; i++)
        outp(0x3c9, pal[i]);  /* manda la paleta al puerto 3c9h */
}




