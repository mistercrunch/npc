/*
 * WU-FTPD Scanner by ca0s / Ezkracho Team
 *
 *		http://www.ezkracho.com.ar
 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <string.h>
#include <malloc.h>
#include <sys/time.h>
#include <sys/types.h>

#define TIMEOUT         	10
#define CHILDS            	20

pid_t wait(int *status);

/* Nombre del archivo default del log del scan.   */
char logfile[128] = "wux-scan.log"; 
int verbose = 0;
int sockfd;

/* Si no logra conectarse en el tiempo marcado en */
/* TIMEOUT cierra el socket.                      */
void connect_timeout() { 
    close(sockfd);
    exit(-1);
}


void ScanTarget(struct in_addr ip) {

    FILE *unf;
    int sockfd, yeah = 0;
    char buffer[1024], ruffer[1024];
    struct sockaddr_in address;


    memset(&address, '\0', sizeof(address));
    memcpy (&address.sin_addr, &ip, 4);
    address.sin_port = htons(21);

    if ((sockfd=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
	printf("\n(!) Esteee eemmmm...  maldito socket #$^#!#@~!\n\n");
	exit(0);
    }

    signal(SIGALRM, connect_timeout);
    alarm(TIMEOUT);    

    if ( connect(sockfd, (struct sockaddr *)&address, sizeof(address)) == -1 ) {
	alarm(0);
	exit(0);
    }
    
    memset(buffer, '\0', strlen(buffer));
    recv(sockfd, buffer, 1024, 0);

/* Banners de los wu vulnerables por los que busca, en caso de querer */
/* agregar nuevas versiones este es el lugar.                         */     
    if (strstr(buffer, "wu-2.4.2")) {
	memset(ruffer, '\0', strlen(ruffer));
	strcpy(ruffer, "]--> wu-2.4.2\n");
	yeah++;
    }
    if (strstr(buffer, "Version wu-2.4.2-academ[BETA-1")) {
	memset(ruffer, '\0', strlen(ruffer));
	strcpy(ruffer, "]--> Posible version vulnerable del wu-2.4.2\n");
	yeah++;
    }
    if (strstr(buffer, "wu-2.6.0(1)")) {
	memset(ruffer, '\0', strlen(ruffer));
	strcpy(ruffer, "]--> wu-2.6.0(1)\n");
	yeah++;
    }
    if ((strstr(buffer, "Mon Feb 28")) || (strstr(buffer, "Wed Aug 9"))) {
	memset(ruffer, '\0', strlen(ruffer));
	strcpy(ruffer, "]--> Posible version vulnerable del wu-2.6.0(1)\n");
	yeah++;
    }
    if (strstr(buffer, "wu-2.6.1")) {
	memset(ruffer, '\0', strlen(ruffer));
	strcpy(ruffer, "]--> wu-2.6.1\n");
	yeah++;
    }

    if (yeah != 0) {
	if (verbose == 1) printf("\n[ %s %s\n", inet_ntoa(ip), ruffer);
    
	if ((unf = fopen(logfile, "aw+")) == NULL) {
    	    printf("\n(!) Fallo al abrir el archivo %s.\n\n", logfile);
	    exit(0);
	}
	fprintf(unf, "[ %s %s", inet_ntoa(ip), ruffer);
	fclose(unf);
    }
    alarm(0);
    exit(0);
}


void CheckTarget(char *targetfile) {

    FILE *foo;
    char buffer[100];
    struct in_addr ip;
    int hijus = 1;

    if ((foo = fopen(targetfile, "r")) == NULL) {
        printf("\n(!) Fallo al abrir el archivo %s.\n\n", targetfile);
	exit(0);
    }

    while (fgets(buffer, sizeof(buffer), foo) != NULL) {
	if (buffer[strlen(buffer)-1] == '\n')
	    buffer[strlen(buffer)-1] = '\0';

	if (inet_aton(buffer, &ip) == 0) {
	    printf("\n(!) %s: con que inventando direcciones IP?\n\n", buffer);
	    exit(0);
	}
	
	if (hijus < CHILDS) {
	    switch (fork()) {
		case 0:
		    ScanTarget(ip);
		    exit(0);
		case -1:
		    printf("\n(!) Esteee eemmmm...  maldito fork() #$^#!#@~!\n\n");
		    exit(-1);
		default:
		    hijus++;
		    break;
	    }  
	} 
	else {
	    wait(NULL);
	    hijus--;
	}
    } 
     
    while(hijus--) wait(NULL);
    fclose(foo);
}


void FukinHelp(char *argv) {
    printf("\nWU-FTPD Scanner by ca0s / Ezkracho Team\n\n");
    printf("Uso: %s <Lista de IPs> [Opcion] [Argumento]\n\n", argv);
    printf("Opciones:\n");
    printf("   -o <Archivo de Salida> Guarda el log del scan.\n");
    printf("   -v Imprime en pantalla el progreso del scan.\n\n");
    printf("Ejemplos:\n");
    printf("   [root@foo]# %s targets.txt -v -o scan-1.log\n", argv);
    printf("   [root@foo]# %s targets.txt &\n\n", argv);
}


int main(int argc, char *argv[]) {

    int opcion;
    char targetfile[128]; 

    if (argc < 2) {
        FukinHelp(argv[0]);
        exit(0);
    }

    if (strlen(argv[1]) > 128) {
        printf("\n(!) El nombre del archivo de targets es demasiado largo.\n\n");
        exit(0);
    }
    else { strcpy(targetfile, argv[1]); }

    while ((opcion = getopt(argc, argv, "vo:")) != EOF) {
        switch (opcion) {
            case 'v':
                verbose=1;
                break;
            case 'o':
                if (strlen(optarg) > 128) { 
                    printf("\n(!) El nombre del archivo de log es demasiado largo.\n\n");
                    exit(0);
                }
                memset(logfile, '\0', strlen(logfile));
                strcpy(logfile, optarg);
                break;
            default:
                FukinHelp(argv[0]);
                exit(0);
        }
    }    
    CheckTarget(targetfile);  
}
