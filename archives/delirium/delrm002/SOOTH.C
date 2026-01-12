// sooth.c
// Author:   	daemon9@netcom.com
// Purpose:	Prepares unsuitable passwd files for use with CrackerJack 1.4
//
// root: /d3L01rLFP7hs:0:1:root:/:/bin/csh  ->  root:/d3L0C1LFP7hs:0:1:root:/:/bin/csh
//
// Limitations:	Needs input filename to be in the form xxxxxxxx.xxx
// 	        (The '.' is expected in function _SpawnFilename())
//
// Responses: 	daemon9@netcom.com
// 	     	94fa137@viking.dvc.edu

#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
#include<string.h>

#define FNAMELENGTH	13
#define EXT1          	's'
#define EXT2          	't'
#define EXT3           	'h'
#define FALSE		0
#define TRUE		1

FILE *fpIN,*fpOUT;

void main(int argc,char *argv[]){

	void _OpenFile(int,char*);
	char * _SpawnFilename(char *);
	void _Sooth(void);

	if(argc!=2){	      			// Needs 1 arguement
		puts("Usage:	sooth <filename> \n");
		printf("Where");
		textcolor(RED);
		cprintf(" filename ");
		printf("is the name of the passwd file to be soothed for use with CrackerJack.");
		exit(1);
	}
	_OpenFile(0,argv[1]);
	_OpenFile(1,_SpawnFilename(argv[1]));
	_Sooth();
}						// End main

// Function:		OpenFile
// Purpose:		To open a file
// Parameters:		A mode indicator[0=reading;1=creation/overwriting] and a pointer to the filename to be opened/created
// Return value:	None.
// Implementation:      Standard file opening

void _OpenFile(int mode,char *filename){


	if(!mode){
		if(!(fpIN=fopen(filename,"r"))){
			textcolor(LIGHTRED+BLINK);
			cprintf("OpenFile reported an error trying to open %s. Cannot open file.",filename);
			exit(1);
		}
	return;
	}

	if(!(fpOUT=fopen(filename,"w"))){
		textcolor(LIGHTRED);
		cprintf("OpenFile cannot create new file %s.",filename);
		exit(1);
	}
}

// Function:		CreateFilename
// Purpose:		To create a filename based from the user inputed one
// Parameters:		A pointer to the input filename
// Return Value:    	Character Pointer; To the new string
// Implementation:	Takes the input filename and creates the new filename ?.sth

char *_SpawnFilename(char *filename){

	static char tempBuffer[FNAMELENGTH];
	int i=0;

	memcpy(tempBuffer,filename,sizeof(filename));
	while(tempBuffer[i]!=0x2e)i++;

	tempBuffer[i+1]=EXT1;
	tempBuffer[i+2]=EXT2;
	tempBuffer[i+3]=EXT3;
	return tempBuffer;
}

// Function:		Sooth
// Purpose:		To sooth out the unsavory spaces in some passwd files
// Parameters:		None.
// Return Value:	None.
// Implementation:	Grabs a char, while it's not an EOF, write it to the output file, and if it's the first colon,
//			ignore the following char (a space)

void _Sooth(void){

	int chr;
	int ColonFlag=FALSE;
	do{
		chr=fgetc(fpIN);
		if(chr==0x3A&&!ColonFlag){
			fputc(chr,fpOUT);
			fgetc(fpIN);
			ColonFlag=TRUE;
			continue;
		}
		if(chr==0x0A)ColonFlag=FALSE;
		fputc(chr,fpOUT);
	}while(chr!=EOF);
}


