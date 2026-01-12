/* tuxiscan 0.1 by tuxisuau.
 *
 * Utilidad:
 * 
 * Barre un rango de ips intentando la conexion a un puerto concreto, y muestra
 * las respuestas de los servers que contestan en formato ip:respuesta
 * 
 * Compilar:
 * gcc tuxiscan.c -lpthread -o tuxiscan
 *
 * <admin> Administro esta red de 1024 maquinas y se que almenos unas 10 tienen
 * el ftp vulnerable! Que maquinas eran?!? mierda! no me acuerdo!
 *
 * Este programa le sacaria la lista de las versiones en menos de 1 segundo,
 * excepto las maquinas con parkinson que tuvieran el servicio en el inetd o el
 * server en el swap...
 * 
 * Este programa NO es para hacer el kiddie. Os recuerdo: Compilar un xpl y
 * ejecutarlo no es ser h4x0r. Tambien hay que entender como funcionan, y
 * tratar de programar alguno. (Critica constructiva, eh? ;)
 *
 * Aviso: Compilar y usar este programa, al menos en España, en un principio no
 * es ilegal, pero su naturaleza puede hacer saltar las alarmas de los isp,
 * incluso de los isp de tu isp y de los isp de los ips de tu isp. No lo uses
 * con tu conexion de verdad a no ser que quieras que te la xapen, ni con
 * ninguna conexion tuya, porque te van a fichar. Escanear rangos grandes desde
 * tu casa seria de jilipollas.
 *
 * De todas formas, el programa tiene un par de fallos mas o menos graves para
 * evitar que cualquiera los pueda compilar... uno impedira la compilacion, el
 * otro escanear mas de X ips seguidas.
 * 
 * tuxisuau@7a69ezine.org
 * 
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <pthread.h>

/* Jugar con estos valores */
#define BUFFERSIZE 256
#define MAXTHREADS 128
#define TIMEOUTREAD 30000
#define TIMEOUTCONNECT 10000

typedef struct
{
	pthread_t tid;
	pthread_attr_t tattr;
	unsigned long ip;
	int port;
	int received;
	char buffer[BUFFERSIZE+1];
} hinfo;

pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond=PTHREAD_COND_INITIALIZER;
int ipleft,threadnum=0;

void show_help(void)
{
	printf("Tuxiscan by tuxisuau\n");
	printf("./tuxiscan <ipstart> <ipend> <port>\n");
	printf("tuxisuau@7a69ezine.org\n");
	exit(-1);
}

void checkserver(hinfo *h)
{
	int ifd,flags,pollret,result_connect,timeout_connect,timeout_read;
	char *reply;
	struct sockaddr_in servah;
	struct pollfd pollfd_connect,pollfd_read;	
	memset(h->buffer,0,BUFFERSIZE);
	
	pthread_mutex_lock(&lock);
	pthread_mutex_unlock(&lock);

	while ((ifd=socket(AF_INET, SOCK_STREAM, 0))==-1); 
	
	servah.sin_family = AF_INET;
	servah.sin_port = htons(h->port);
	servah.sin_addr.s_addr = htonl(h->ip);
	bzero(&(servah.sin_zero),8);

	timeout_read = TIMEOUTREAD;
	timeout_connect = TIMEOUTCONNECT;
	
	flags = fcntl(ifd, F_GETFL);
	fcntl(ifd , F_SETFL, flags | O_NONBLOCK);
	
	connect(ifd, (struct sockaddr *)&servah,sizeof(struct sockaddr));
	
	pollfd_connect.fd = ifd;
	pollfd_connect.events = POLLOUT;
	
	pollret = poll(&pollfd_connect, 1, timeout_connect);
	
	if (pollret) {
		getsockopt(ifd, SOL_SOCKET, SO_ERROR, &result_connect, sizeof(result_connect));
	}
	
	if (!pollret||(pollret && result_connect)) {
		close(ifd);
		goto exit_thread;
	
	pollfd_read.fd = ifd;
	pollfd_read.events = POLLIN;

	pollret = poll(&pollfd_read, 1, timeout_read);
	
	h->received = 0;
	if (pollret) h->received = read(ifd, h->buffer, BUFFERSIZE);
	
	close(ifd);

	if(h->received>0) {
		printf("%s:%s",inet_ntoa(htonl(h->ip)),(h->buffer));
		fflush(stdout);
	}
	
exit_thread:
	pthread_mutex_lock(&lock);
	threadnum--;
	ipleft--;
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&lock);
	pthread_exit(0);
}

int main(int argc,char *argv[])
{
	int port,received,pid,threadmax = MAXTHREADS;
	unsigned long ip,ipstart,ipend,ipnum,pthread_create_ret;
	hinfo *h;
	hinfo **htable;
	if (argc!=4) 
		show_help();
	port = atoi(argv[3]);
	ipstart = ntohl((unsigned long)inet_addr(argv[1]));
	ipend = ntohl((unsigned long)inet_addr(argv[2]));
	ipnum = ipend-ipstart+1;
	htable = (hinfo **)malloc(4*(ipnum));
	ipleft=ipnum;
	for(ip = ipstart;ip<ipend;ip++) {
		h = (hinfo *)malloc(sizeof(hinfo));
		if (h==NULL) {
			printf("Malloc() error\n");
			exit(-1);
		}
		*htable = h;
		h->port=port;
		h->ip=ip;

		while (threadnum>=threadmax) {
			pthread_cond_wait(&cond, &lock);
		}
		
		threadnum++;

		pthread_attr_init (&h->tattr);
		pthread_attr_setdetachstate(&h->tattr, PTHREAD_CREATE_DETACHED);
		while (pthread_create(&h->tid, NULL, (void *)checkserver, h));
		htable = htable++;
	}
	
	while (ipleft) {
		pthread_cond_wait(&cond, &lock);
	}
	pthread_mutex_unlock(&lock);
}
