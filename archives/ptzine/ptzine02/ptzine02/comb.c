/*  Duvidas para: elektro@null.net
     
    Programa criado para o auxilio no Calculo Combinatorio, em que sao 
    geradas todas as combinacoes de m, n a n.
    Por default estao atribuidos a m e a n os valores 49 e 6, respectivamente.
    Obvio como alterar estes valores, certo?

*/

#include <stdio.h>
#include <time.h>
#include <signal.h>
#define MAX 7

int vc[MAX];
int m=49, n=6;  /* Combinacoes de 49, 6 a 6: (49 C 6) */
unsigned long int combinacoes=0;

void gera(int, int);
void print_combinacoes(void);
void tempo_demorado(time_t *);
void sair();

time_t t1, t2;

void main(){

   signal(2, sair);
   time(&t1);
   gera(1,0);
   printf("Terminado!!!!!!!!!!!!!\n");
   tempo_demorado(&t1);

}

void gera(int p, int x){
int i;

    if(x==n){
          combinacoes++;
          print_combinacoes();
          return;
          }

    for(i=p; i<=m-n+x+1; i++){
       vc[x+1]=i;
       gera(i+1, x+1);
       }
}

void print_combinacoes(void){
int i;

    for(i=1; i<=n; i++)
       printf("%3d", vc[i]);
     
    printf("\n");
}

void tempo_demorado(time_t *tempo1){
time_t tempo2;

time(&tempo2);
printf("Gerador iniciado em: %s", ctime(tempo1));
printf("Terminado em       : %s", ctime(&tempo2));
printf("Combinacoes geradas: %lu\n", combinacoes);
exit(0);

} 

void sair(){
tempo_demorado(&t1);
exit(1);
}















