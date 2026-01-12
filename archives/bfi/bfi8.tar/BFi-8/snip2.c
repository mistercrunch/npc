/*
 * Name: SNiP V.2 Beta
 * Date: Tue Feb 15 01:00:26 2000
 * Author: pIGpEN [pigpen@s0ftpj.org, deadhead@sikurezza.org]
 *
 * SoftProject Digital Security for Y2K (www.s0ftpj.org)
 * Sikurezza.org Italian Security MailingList (www.sikurezza.org)
 * 
 * COFFEE-WARE LICENSE - This source code is like "THE BEER-WARE LICENSE" by
 * Poul-Henning Kamp <phk@FreeBSD.ORG> but you can give me in return a coffee.
 * 
 * Tested on: FreeBSD 4.0-19990705-CURRENT FreeBSD 4.0-19990705-CURRENT #6 i386
 * 	      Linux 2.2.5-15 #1
 */


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <dirent.h>

#define BFIART	"BFi"

#define BFISEP "<-| "
#define BFIEND "<-X->"
#define	BFISEP_LEN	strlen(BFISEP)

#define BUF_SIZE	255
#define DIRNAME		"bfisrc"

#define FILE_MODE	0
#define DIR_MODE	1

void usage	(char *cmd);
void dir2file	(char *dirname);
void snip	(char *filename, int mode);
void checktype	(char *name);
void makedir	(char *dirname);

extern char *optarg;

int main(int narg, char **arg)
{
	(narg!=2)	?	usage(arg[0])	:	checktype(arg[1]);

	return 0;
	
}	

void usage(char *cmd)
{
	printf("\nSNiPv2 - pIGpEN / softpj\n"
	       "Under COFFEE-WARE License\n"
	       "Usage: %s dir-or-file\n\n", cmd);
}	

void checktype(char *name)
{
	struct stat chk;

	if(stat(name,&chk)) {
		printf("Hmm %s doesn't exist\n", name);
		exit(0);
	}

	if(chk.st_mode & S_IFREG) snip(name, FILE_MODE);
	if(chk.st_mode & S_IFDIR) dir2file(name);

}
			

void dir2file(char *dirname)
{
	DIR *dp;
	struct dirent *de;
	char currentdir[BUF_SIZE];


	if(!getcwd(currentdir, sizeof(currentdir))) { 
		perror("[dir2file] getcwd()");
		exit(0);
	}

	if(!(dp=opendir(dirname))) {
		perror("[dir2file] opendir");
		exit(0);
	}

	if(chdir(dirname) == -1) {
		perror("[dir2file] chdir1");
		exit(0);
	}

	while((de=readdir(dp))) {

		printf("(%s) ...", de->d_name);
		
		switch(de->d_type) {
			case DT_FIFO:
			case DT_BLK:
			case DT_LNK:
			case DT_DIR:
			case DT_SOCK:	printf("skipping\n");
					continue;
		}
		
		if(strstr(de->d_name, BFIART)) {
			printf("bfi file!\n");
			chdir(dirname);
			snip(de->d_name,DIR_MODE);
		}else printf("not found\n");
	}

 	if( chdir(currentdir) == -1) {
		perror("[dir2file] chdir2");
		exit(0);
	}
	
	closedir(dp);
}	

void snip(char *filename, int mode)
{
	FILE *in, *out;
	char buf[BUF_SIZE];
	int i;
	static int passed = 0;
	char ofile[BUF_SIZE];

	bzero(ofile, sizeof(ofile));

	if(!(in=fopen(filename,"r"))) {
		perror("[snip] fopen()");
		exit(0);
	}


	while(fgets(buf, BUF_SIZE, in)) {
		if(!strncmp(buf, BFISEP, BFISEP_LEN)) {
			
			for(i=0; !isspace((int) buf[BFISEP_LEN+i]); i++) 
				ofile[i] = buf[BFISEP_LEN+i];
			
			ofile[i] = '\0';
			
			printf("\t\tDetected file -> %s\n", ofile);

			switch(mode) {
				case DIR_MODE:
						if(!passed) {
							makedir(DIRNAME);
							passed = 1;
						}
						chdir("bfisrc");		
						break;
			}


			if(!(out = fopen(ofile, "w"))) {
				perror("[snip] fopen() #2");
				exit(0);
			}

			while(fgets(buf, BUF_SIZE, in) && 
			      !strstr(buf, BFIEND)) fprintf(out,"%s",buf);

			fclose(out);

			if(passed && DIR_MODE) chdir("..");
		}	
	}
				
	fclose(in);	
}

void makedir(char *dirname) 
{
	mkdir(dirname, S_IRWXU | S_IRWXG | S_IRWXO);
}
