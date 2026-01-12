#include <stdio.h>
#include <string.h>
#include <errno.h>
#define SIM 1
#define NAO 0
#define LINHA 100
#define STR_LEN 4   /* tamanho minimo das strings aceites */

typedef struct no{
         char *string;
	 struct no *esq;
	 struct no *dir;
         } NO, *ARVORE;

typedef int boolean; 

ARVORE insert(const char *, ARVORE);
void print_em_ordem(ARVORE);
void porque(void);
char *divide(const char *);

char vector[LINHA];

void main(int argc, char *argv[]){
ARVORE a=0;
FILE *fd;
char *p;

   if(argc<2){
     printf("elektro@null.net\n");
     printf("Uso: %s <ficheiro de texto>\n", argv[0]);
     exit(1);
     }

   if((fd=fopen(argv[1], "r"))==NULL){
     perror(argv[1]);
     exit(1);
     }

   while(fgets(vector, LINHA, fd)!=NULL)
     while((p=divide(vector))!=NULL)
            if(strlen(p)>=STR_LEN)
              a=insert(p, a);

   print_em_ordem(a);
}

ARVORE insert(const char *s, ARVORE a){

  if(a==0){
    ARVORE nova;
    if((nova=(ARVORE) malloc(sizeof(NO)))==NULL){
      porque();
      exit(1);
      }
    if((nova->string=(char *) malloc(sizeof(strlen(s)+1)))==NULL){
      porque();
      exit(1);
      }
    strcpy(nova->string, s);
    nova->esq=0;
    nova->dir=0;
    return(nova);
    }

  if(!strcmp(s, a->string))
    return(a);
 
  if(strcmp(s, a->string)<0){
    a->esq=insert(s, a->esq);
    return(a);
    }

  a->dir=insert(s, a->dir);
  return(a);

}

void print_em_ordem(ARVORE a){

  if(a==0)
    return;

  print_em_ordem(a->esq);
  printf("%s\n", a->string);
  print_em_ordem(a->dir);
}

char *divide(const char *s){
static char pal[30];
static int index=0;
int index2=0,
    flag=0;

   bzero(pal, 25);

   while(1){
 
          if(s[index]=='\0'){
            index=0;
            return NULL;
	    }      

          if(s[index]>=-32 && s[index]<=-29 ||
	     s[index]>=-64 && s[index]<=-61){
             pal[index2++]='a';
	     index++;
             flag=1;
	     continue;
	     }

	  if(s[index]>=-24 && s[index]<=-22 ||
             s[index]>=-56 && s[index]<=-54){
             pal[index2++]='e';
	     index++;
             flag=1;
	     continue;
	     }

	  if(s[index]>=-20 && s[index]<=-18 ||
             s[index]>=-52 && s[index]<=-50){
             pal[index2++]='i';
	     index++;
             flag=1;
	     continue;
	     }

	  if(s[index]>=-14 && s[index]<=-11 ||
             s[index]>=-46 && s[index]<=-43){
             pal[index2++]='o';
	     index++;
             flag=1;
	     continue;
	     }

	  if(s[index]>=-7 && s[index]<=-5 ||
             s[index]>=-39 && s[index]<=-37){
             pal[index2++]='u';
	     index++;
             flag=1;
	     continue;
	     }

	  if(s[index]==-25 || s[index]==-57){
	     pal[index2++]='c';
	     index++;
	     flag=1;
	     continue;
	     }

          if((s[index]>='a' && s[index]<='z') ||
             (s[index]>='A' && s[index]<='Z'))
	    {            

	    if(s[index]>='A' && s[index]<='Z'){
               pal[index2++]=(s[index++]+32);
               flag=1;
               continue;
	       }
             
             pal[index2++]=s[index++];
	     flag=1;
             }
	  else{
               index++;
               if(flag){
		 pal[index2]='\0';
		 return pal;
	         }
	       }
            }
}

void porque(){
printf("  Memoria insuficiente!!
  Este erro ocorreu provavelmente devido ao excessivo tamanho do ficheiro
de texto! Ou talvez devido a fragmentacao da memoria!
  Faca um reboot ao computador e volte a experimentar.
  Se ocorrer de novo o mesmo erro mande um e-mail para elektro@null.net, a
expor a situacao.\n");
}