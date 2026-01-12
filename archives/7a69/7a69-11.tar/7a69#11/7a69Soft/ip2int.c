/* ip2int.c */
#include <stdio.h>
#define VERSION "ip2int 0.0.1 by tuxisuau"
void ajuda(void){
	printf("%s\n",VERSION);
	printf("us: ip2int a b c d\n");}
int ip2int(int a,int b,int c,int d){
	return ((a*256*256*256)+(b*256*256)+(c*256)+d);}
int main(int argc,char *argv[]){
	int a=0,b=0,c=0,d=0,i=0;
	if(argc!=5)
		ajuda();
	else{
		printf("Ip: %d.%d.%d.%d\n",a=atoi(argv[1]),b=atoi(argv[2]),c=atoi(argv[3]),d=atoi(argv[4]));
		printf("Integer: %d\n",i=ip2int(a,b,c,d));}
	}
/* EOF */
