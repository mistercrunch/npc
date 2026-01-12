/* coded by polos */ 

#include <stdio.h>

FILE *iplist;
char buffer[200];


main(int argc,char *argv[]) {

    if(argc<2) {
	ayuda();
    }
   
    if(argc<3) {
	printf("Falta selecciona la clase: -a , -b o -c ");
	exit(1);
    }

    if(argc<4) {
	printf("Falta seleccionar un archivo de salida\n");
	exit(1);
    }

    if(strlen(argv[3])>20) {
	printf("Elija un nombre de archivo mas corto\n");
	exit(1);
    }

    if(strlen(argv[2])>4) {
	printf("No es una opcion disponible\n");
	exit(1);
    }

    if(strlen(argv[1])>16) {
	printf("Verifique la ip ingresada\n");
	exit(1);
    }

    iplist=fopen(argv[3],"w");

    if(strstr(argv[2],"-c")) {
	sprintf(buffer,"%s",argv[1]);
	checkea(buffer,2);      
        generador(argv[1],1);
    }
    else if (strstr(argv[2],"-b")) {	
	sprintf(buffer,"%s",argv[1]);
	checkea(buffer,1);      
	generador(argv[1],2);
    }
    else if (strstr(argv[2],"-a")) {	
	sprintf(buffer,"%s",argv[1]);
	checkea(buffer,0);      
        generador(argv[1],0);
    }
}

generador(char getip[100],int a) {
int i;   

    printf("Generando ip..... Espere\n");	

    if(a==1) { 
	for(i=0;i<255;i++) {
	    sprintf(buffer,"%s.%d",getip,i);
	    fprintf(iplist,"%s\n",buffer);
        }
    }

    if(a==2) {
	int e=0;
     
        while(e<255) {
	    for(i=0;i<255;i++) {
		sprintf(buffer,"%s.%d.%d",getip,e,i);
	        fprintf(iplist,"%s\n",buffer);
	    }
	    e++;
	}	
    }
    
    if(a==0) {
	int e=0;
	int s=0;
	
	while(s!=255) {
	    for(e=0;e<255;e++) {

		for(i=0;i<255;i++) {
		    sprintf(buffer,"%s.%d.%d.%d",getip,s,e,i);
		    fprintf(iplist,"%s\n",buffer);
	        }
	    }
	    s++;
	}
    }
    fclose(iplist);
}
	      
	
      
ayuda() {
    printf("IP Generator ........ C0ded by polos\n");
    printf("Clase A:  ./genera 200 -a output-file\n ");
    printf("Clase B:  ./genera 200.10 -b output-file\n");
    printf("Clase C:  ./genera 200.10.23 -c output-file\n");
    exit(1);
}

checkea(char *buffe,int test) {
int e=0;
int a=0;
   
    for(a=0;a<(strlen(buffer));a++) {
	if(buffer[a]=='.') {
	e++;
	}
    }
   
    if(test == e) {
	return 1;
    }
    else {
	printf("La ip dada no corresponde con la clase seleccionada\n");
        exit(1);
    }
}
