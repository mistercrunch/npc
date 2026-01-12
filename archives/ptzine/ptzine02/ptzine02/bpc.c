#include <stdio.h>
#include <string.h>
#include <shadow.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <time.h>

#define  SLEEP_PID(x) sleep(x*3600) /* macro que lida com o "adormecimento" */
                                    /* do programa                          */
#define NAO 0
#define SIM 1
#define MSG_NEGACAO "Voce nao e o root!!:-("

char *out_fich="bpc.log",
     *notificacao="aviso.txt",  
     matriz[15][20];  /* necessaria para a divisao da entrada GECOS */

int tempo=24,  /* tempo default de "adormecimento" do programa  */
    flag=-2,
    num_pal;   /* numero de palavras da entrada GECOS */

struct passwd *p;
struct spwd *s;

int strdiv(const char *vector);
void log_to_root(const char *utilizador, int modo, time_t *time);
int notifica(const char *utilizador);
int crypt_e_cmp(void);
void help(const char *exe);
char *inverte(const char *vector);
char *concatena(const char *vector);
int testa_o_0(const char *vector);
char *o_0(const char *vector);


                       /*  COMECO DA FUNCAO MAIN  */

void main( int argc, char *argv[]){

int i,
    mail=SIM,
    dorme=1;
time_t hora;

  if(getuid()!=0){
                  puts(MSG_NEGACAO);
                  exit(0);
                  }

    for(i=1; i<argc; i++){
       if(argv[i][0]=='-'){
         switch(argv[i][1]){
     
               case 'h':
                       {
                        help(argv[0]);
                        exit(0);
                        }
               
               case 'o':
                       {
                        out_fich=&argv[i][2];  
                        break;
                       }
   
               case 'n':
                       {
                        notificacao=&argv[i][2];
                        break;
                       }

               case 'm':
                       {
                        mail=NAO;
                        break;
                       }

               case 't':
                       {
                        tempo=atoi(&argv[i][2]);
                        break;
                       }

	      case 's':
		      {
                       dorme=0;
                       break;
                      }

                    }
                 } 
 

              }  
  
  while(1){
  
    while( (p=getpwent())!=NULL){             

     if( (s=(struct spwd*) getspnam( p->pw_name))==NULL)
                                                      continue;
     if( strlen(s->sp_pwdp)!=13)
                             continue;
     num_pal=strdiv( p->pw_gecos );

     if( crypt_e_cmp()==SIM){
       if(mail!=NAO)
           flag=notifica(p->pw_name);
           hora=time(NULL);
           log_to_root(p->pw_name, flag, &hora); 
         }
     }
     if(dorme){
                SLEEP_PID(tempo);
                setspent();
                setpwent();
                }
                else
                   break;  
     }
}


                    /*   FIM DA FUNCAO MAIN   */


int strdiv(const char *vector){
int LINHAS,COLUNAS,COLUNAS2=0;

    for(LINHAS=0;;LINHAS++){
       for(COLUNAS=0;;COLUNAS++){
		   if(vector[COLUNAS2]=='\0'){
		       matriz[LINHAS][COLUNAS]='\0';
		       return(LINHAS+1);
		       }
	           if(vector[COLUNAS2]==32 && vector[COLUNAS2+1]!=32)
				{
				COLUNAS2++;
				break;
				}

				matriz[LINHAS][COLUNAS]=vector[COLUNAS2];
				COLUNAS2++;
			  }
			   matriz[LINHAS][COLUNAS]='\0';
			}
	             }


int crypt_e_cmp(void){
int i,
    i2;
static char temp[20];

if( 
  strcmp( s->sp_pwdp, 
          crypt(p->pw_name, s->sp_pwdp))==0)
                                           return(SIM);
if(
  strcmp( s->sp_pwdp,
          crypt( inverte(p->pw_name), s->sp_pwdp))==0)
                                           return(SIM);
if(
  strcmp( s->sp_pwdp,
          crypt( concatena(p->pw_name), s->sp_pwdp))==0)
                                           return(SIM);
if( testa_o_0(p->pw_name)==SIM)
  if(
    strcmp( s->sp_pwdp,
            crypt( o_0(p->pw_name), s->sp_pwdp))==0)
                                           return(SIM);
   for(i=1; i<=99; i++){
   sprintf(temp, "%s%d", p->pw_name, i);
if(
  strcmp( s->sp_pwdp,
             crypt(temp, s->sp_pwdp))==0)
                                       return(SIM);
         }

   for(i=0; i<=num_pal; i++){

if( 
  strcmp( s->sp_pwdp, 
          crypt(matriz[i], s->sp_pwdp))==0)
                                           return(SIM);
if(
  strcmp( s->sp_pwdp,
          crypt( inverte(matriz[i]), s->sp_pwdp))==0)
                                           return(SIM);
if(
  strcmp( s->sp_pwdp,
          crypt( concatena(matriz[i]), s->sp_pwdp))==0)
                                           return(SIM);
if( testa_o_0(matriz[i])==SIM)
  if(
    strcmp( s->sp_pwdp,
            crypt( o_0(matriz[i]), s->sp_pwdp))==0)
                                           return(SIM);
   for(i2=1; i2<=99; i++){
   sprintf(temp, "%s%d", matriz[i], i);
if(
   strcmp( s->sp_pwdp,
           crypt(temp, s->sp_pwdp))==0)
                                   return(SIM);
         }
   }


return(NAO);
}


int notifica(const char *utilizador){
static char aviso[40];

  if( fopen(notificacao, "r")==NULL)
                               return(-1);

  sprintf(aviso, "cat %s | mail -s IMPORTANTE!!! %s", notificacao, utilizador);
  system(aviso);
  return(0);
}

void log_to_root(const char *utilizador, int modo, time_t *time){
FILE *p;

p=fopen( out_fich, "a");

if( modo==-1)
 fprintf(p, 
"%s - %s(nao notificado...nenhuma mensagem de aviso atribuida!!)\n\n" ,
utilizador, ctime(time));
else

if( modo==-2)
 fprintf(p,
"%s - %s(nao notificado...opcao -m!!)\n\n",
utilizador, ctime(time));
else 

 fprintf(p, 
"%s - %s(notificado)\n\n", utilizador, ctime(time));
 fclose(p); 
}  


void help(const char *exe){

printf("Uso: %s -[opcoes]\n", exe);
printf("Contacto: elektro@null.net\n");
printf("     -h  ajuda\n");
printf("     -o  altera o ficheiro de destino do log\n");
printf("     -n  altera o ficheiro de entrada de notificacao\n");
printf("     -m  liga/desliga a opcao de notificar o utilizador\n");
printf("     -t  altera o intervalo entre verificacoes (default 24h)\n");
printf("     -s  corre o programa apenas uma vez e sai. No Sleep\n");
}



char *inverte(const char *passwd){
static char temp5[20];
int i,tamanho;
char temp;

strcpy( temp5, passwd);
tamanho=strlen(temp5);

  for(i=0; i<(tamanho/2); i++){
	                 temp=temp5[i];
			 temp5[i]=temp5[(tamanho-1)-i];
			 temp5[(tamanho-1)-i]=temp;
			}
	  return temp5;
    }



char *concatena(const char *passwd){
static char temp6[20];
int tamanho;

strcpy( temp6, passwd);
strcat( temp6, passwd);
return temp6;
}

int testa_o_0(const char *passwd){
int i;

  for(i=0;;i++){
		if(passwd[i]=='\0')
				break;
		if(passwd[i]=='o' || passwd[i]=='O')
				return(SIM);
			      }
				return(NAO);
		      }


char *o_0(const char *passwd){
static char temp3[20];
int i;

strcpy( temp3, passwd);

	for(i=0;;i++){
		if( temp3[i]=='\0' )
				   return temp3;
		if(temp3[i]=='o' || temp3[i]=='O')
				   temp3[i]='0';
		     }
	      }





















