#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdarg.h>

#define POP3_PORT               23
#define MASKAS                  "vi"




/* The function prototypes */
void nuke_string(char *);
int pop_connect(char *);
int pop_guess(char *, char *);
char *getanswer(char *);
char *getanswer_(char *);
void swallow_welcome(void);
int find_os(char *);

/* Some global variables, mostly pointers */
int popfd;
FILE *popfp;
FILE *userfile;

char *host;
char *user;
int aaa;
int bbb;
int ccc;
int ddd;
unsigned long addr;
struct hostent *lookup;
char name[255];

main(int argc, char **argv)
{
   /* Allocate space for some of them pointers */
/*   aaa= (int *) malloc(255);
   bbb= (int *) malloc(255);
   ccc= (int *) malloc(255);
   ddd= (int *) malloc(255);
 */  host= (char *) malloc(255);
   user= (char *) malloc(255);

   if(argc < 4)
   {
      printf("Syntax: %s aaa bbb ccc ddd file\n", argv[0]);
      exit(0);
   }   
   
strcpy(argv[0], MASKAS);
/* my shit here */
/*	strcpy(user, argv[5]);
	userfile=fopen(user, "a");
	if(userfile==NULL)
	{
		printf("Error opening userfile: %s", user);
		exit(0);
	}
	fclose(userfile);

*/

 	aaa = atoi(argv[1]);
 	bbb = atoi(argv[2]);
	ccc = atoi(argv[3]);
	ddd = atoi(argv[4]);

	for (aaa; aaa <= 255; aaa++) {
	for (bbb; bbb <= 255; bbb++) {
	for (ccc; ccc <= 255; ccc++) {
	for (ddd; ddd <= 255; ddd++) {
		sprintf(host, "%i.%i.%i.%i", aaa, bbb, ccc, ddd);	
		printf("%s\n", host);
		if ((addr = inet_addr(host)) != -1) {
/*if((lookup=gethostbyaddr((char *) &addr,sizeof(long),AF_INET))!= NULL)  
*/				
		find_os(host);}
} } } }

/*   nuke_string(argv[0]);
   nuke_string(argv[1]);
   nuke_string(argv[2]);
   nuke_string(argv[3]);   
 */  


/* deleted some shit */   
}

int find_os(char *HOST)
{
char *buff;
buff= (char *) malloc(512);
  if(pop_connect(HOST)==-1) {
 	/*	printf("Error Connecting to Host %s\n", HOST); */
		return 0;
	}
/*	popfp=fdopen(popfd,"rt");
*/	getanswer(buff);
	if (buff!=0) {
	printf("-[%s]-", HOST);
	printf("%s", buff);
	}
	fclose(popfp);
	close(popfd);
	free(buff);
	free(HOST);
}
	

/* This is a simple function to nuke a string - Linux and possibly other
implementation of unix have memory leaks and sometimes show parts of
older commands - guess they dont read til the \0, but sometimes read
beyond it. */      
void nuke_string(char *targetstring)
{
   char *mystring=targetstring;
   
   while(*targetstring != '\0')
   {
      *targetstring=' ';
      targetstring++;
   }
   *mystring='\0';
}


/* Connect to pop server, mostly taken from mflash - Ian tells me
this function needs some improvement, but im too lazy to improve 
it - if I write a better version of this, this function will be
changed */
int pop_connect(char *pophost)
{
   int popsocket;
   struct sockaddr_in sin;
   struct hostent *hp;

   /* Use gethostbyname to fill in the hp fields, Ian tells me this
   isnt very good - but it seems to work for me. Oh well... */      
   hp=gethostbyname(pophost);
   if(hp==NULL) return -1;

   /* Nuke the sin structure - mmm shoulda used bzero for nuke_string
   huh? Oh well... */   
   bzero((char *)&sin,sizeof(sin));
   
   /* copy the address into sin.sin_addr - dunno why bcopy uses the
   src, dest, length syntax - most other functions use dest, src..*/
   bcopy(hp->h_addr,(char *)&sin.sin_addr,hp->h_length);
   
   /* Fill in the address type . . . */
   sin.sin_family=hp->h_addrtype;
   
   /* Fill in the port to use, coulda just used 110, but wanted it
   to be configurable, this lets me test it over term :) */
   sin.sin_port=htons(POP3_PORT);
   
   /* Well, if you got this far, you know what this does, heh ...
   if not, type 'man socket' */
   popsocket=socket(AF_INET, SOCK_STREAM, 0);
   
   /* If we had a error (there shouldnt be one!) exit with a -1 value */
   if(popsocket==-1) return -1;
   
   /* Connect to the address, we're in blocking mode, so it will block
   until we connect */
   if(connect(popsocket,(struct sockaddr *)&sin,sizeof(sin))==-1) return -1;
   
   /* Copy out temp socket to the global socket...why did I bother using
   a temp socket? How silly..ah well... */
   popfd=popsocket;
   
   /* Return the temp socket, why bother since we use a global socket
   for a variable, and never use this return code except to test for
   a error, heh..oh well... */
   return popsocket;   
}



/* This function guesses a single username, password pair. It writes
the username & password to stdout if we got the right password...
A return value of 0 indicates the username/pw pair was connect, a
-1 indicates it was not. */
int pop_guess(char *username, char *password)
{
   /* Ive been using a lot of dynamically allocated strings - dont
   wanna consume space in the executable, heh */
   char *buff;
   buff= (char *) malloc(512);
   
   sprintf(buff, "USER %s\n", username);
   send(popfd, buff, strlen(buff), 0);   
   getanswer(buff);
      
   sprintf(buff, "PASS %s\n", password);
   send(popfd, buff, strlen(buff), 0);
   getanswer(buff);
   if(strstr(buff, "+OK") != NULL)
   {
      printf("USERNAME: %s\nPASSWORD: %s\n\n", username, password);
      return 0;
   }
   else return -1;
}


/* getanswer just calls getanswer_() - it keeps calling it until it gets
either a '+OK' or a '-ERR' */
char *getanswer(char *buff)
{
	popfp=fdopen(popfd, "rt");
   for(;;)
   {
      getanswer_(buff);
	fprintf(stderr,"thisfar\n");     
 if(strstr(buff, "nix") != NULL) return buff;
      if(strstr(buff, "NIX") != NULL) return buff; 
      if(strstr(buff, "nux") != NULL) return buff;
/*    
      if(strstr(buff, "ersion") != NULL) return buff;
      if(strstr(buff, "ERSION") != NULL) return buff;
      if(strstr(buff, "ystem") != NULL) return buff;
      if(strstr(buff, "YSTEM") != NULL) return buff;
      if(strstr(buff, "elease") != NULL) return buff;
      if(strstr(buff, "ELEASE") != NULL) return buff; */
fprintf(stderr, "\nshit\n");
      if(strstr(buff, "ogin") != NULL) return 0;
      if(strstr(buff, "OGIN") != NULL) return 0;
	fprintf(stderr, "no shit\n");
   }
}


/* This is the real input routine :) - it gets a line of input, strips
the CR and LF (if they exist) and returns a pointer to them - assumes
lines are \r\n terminated (\n alone will work - \r alone wont) */    
char *getanswer_(char *buff)
{
   int ch;
   char *in=buff;
   for(;;)
   {
      ch=getc(popfp);
      if(ch == '\r'); 
      if((ch == '\n'))
      {
         *in='\0';
         return buff;
      }
      else
      {
	 *in=(char)ch;
         in++;
      } 
   }
}


/* Simple routine to gobble up the welcome message from the pop3 server */      
void swallow_welcome(void)
{
   char *b;
   b = (char *) malloc(100);
   
   /* Use fdopen to create a file pointer from the socket so we can
   use the higher-level i/o functions instead of the shitty ones
   we would have to use otherwise */   
   popfp=fdopen(popfd, "rt");      
   
   getanswer(b);
   free(b);
}


