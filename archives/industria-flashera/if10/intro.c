/* iNDUSTRIA fLASHERA iNC
   :: www.iflashera.org ::
   intro coded by toshi
   Needed djgpp + allegro to compile
   enjoy this shit */

#include "allegro.h"
#include <stdio.h>
int main()
{
 BITMAP *buffer;
 int c;
 int ejex[100]; /* 320 */
 int ejey[199]; /* 240 */
 int iax=0,jax=0;
 int i;
 int textx[111];
 float textosinusoidal=0;
 int scroller=400;
 
/* for(i=0; i<200; i++) {
 ejex[i]=0;
 ejey[i]=0;
 textx[i]=0;
 } */



 allegro_init();
 install_timer();
 if (set_gfx_mode(GFX_SAFE, 320, 240, 0, 0) != 0) {
    set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
    allegro_message("No es posible el modo grapixcoz\n\Error de Allegro : %s\n", allegro_error);
    return 1;
    }
  set_palette(default_palette);
 text_mode(-1);
 buffer = create_bitmap(SCREEN_W, SCREEN_H);

 c=retrace_count+32;
 do {
 int counter, bla,ble;
 clear_to_color(buffer, makecol(0, 0, 0));
 textprintf(buffer, font, 170+(textx[iax]-300)/3, 210, makecol(scroller, scroller, scroller), "iNDUSTRIA fLASHERA iNC");
 textprintf(buffer, font, 166+(textx[iax]-300)/3, 220, makecol(scroller, scroller,scroller), ":: www.iflashera.org ::");
 textprintf(buffer, font, 150+(textx[iax]-300)/3, 230, makecol(scroller,scroller,scroller), "que linda la trigonometria!");
 textprintf(buffer, font, scroller, 0, makecol(bla/5, bla/4, ble/5), "industria flashera proudly presents... the first intro for the mag, very shitty, indeed..... and very huge too..... al parecer, no soy tan bueno optimizando codigo........... 300k+ !!!!!!..... que mierda...... hablando en serio, esta intro es una porqueria... apreta cualquier tecla para sacar esta basura.................................. en serio, apreta alguna tecla, asi termina...................... ey! vamos! no me digas que te gusto!...................................... de verdad, ya fue... el seno ");
 textprintf(buffer, font, scroller+4080, 0, makecol(bla/5, bla/4, ble/5), "y el coseno pueden ser muy divertidos, pero no tanto con rutinas putpixel tan chotas como las que uso........................ basta che, me esta empezando a dar verguenza......................... de que te reis???? tan malo es ?????? ..................... seguro que no podrias hacer ni la mitad de lo que hace este programa a mano ............................ con una calculadora y poniendo todos los valores en una tablita, asi no es en tiempo real................... que podes hacerlo? mandame un mail con la ");
 textprintf(buffer, font, scroller+8180, 0, makecol(ejex[iax], 0, 0/* bla/5, bla/4, ble/5*/ ),"tabla completa.................... ah, si, ya se que esta esa tabla en cualquier libro de analisis, pero dejame de joder, que esperas de alguien que todavia no rindio bien analisis ................................... bue, me canse de esta mierda, no hablo mas....... ;)");
 scroller--;


 for (counter=0; counter <1024; counter++){
 iax++;
 jax++;
 bla+=.002; /* .002 kewwwwwl! */
 ble+=212; /* 212 kewl! */
 textosinusoidal+=.00005;


  if (jax = 2444000){
    jax=0;
    iax=0;
   }
   ejex[iax] = 150+ cos(ble)*100; // posisiona el efecto en x
   textx[iax] = sin(textosinusoidal)*150; // el baiven ;)
   ejey[jax] = 109+ cos(bla)*100; // posisiona el efecto en y
   putpixel(buffer, ejex[iax], ejey[jax], ejex[iax*24]); // pone los puntitos 333
                                                 // en la pantalla

  }


 blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

 vsync();
 } while (!kbhit());
 destroy_bitmap(buffer);
 clrscr();
    printf("industria flashera huge intro\n coded in djgpp + allegro lib by toshi[if]\n\n Yep, this file is very huge");
 return 0;
}

/* END_OF_MAIN(); */
