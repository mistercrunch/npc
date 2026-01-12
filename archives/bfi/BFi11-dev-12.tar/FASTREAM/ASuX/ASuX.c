
 /*** FASTREAM suite ********************************************
 *                        _    ____       __  __                *
 *                       / \  / ___| _   _\ \/ /                *
 *                      / _ \ \___ \| | | |\  /                 *
 *                     / ___ \ ___) | |_| |/  \                 *
 *                    /_/   \_\____/ \__,_/_/\_\                *
 *                                                              *
 *             tHE rECIdjVO - <recidjvo@s0ftpj.org>             *
 *                                                              *
 *               *** DEVEL/UNTESTED/UNSTABLE ***                *
 *                                                              *
 ******************************************** fastboys, 2002 ***/

#define EXEC_UNI 
#define EXEC_MULTI 
//#define EXEC_UNI execl(..)
//#define EXEC_MULTI execl(..)

#ifdef FASTREAM_DEBUG
        #define debug(x...) fprintf(stderr, x)
#else
        #define debug(x...)
#endif

#define ASX_UNI "<ASX "
#define ASX_MULTI "<Mult"
#define TOK_UNI "<ref HREF="
#define TOK_MULTI1 "<D_Address>"
#define TOK_MULTI2 "<UDP_Port>"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void);
void unicast_asx(void);
void multicast_asx(void);

int main()
{
        char buff[24];

        debug("ASuX devel-version, fastboys 2002.\n");

        fgets(buff, sizeof(buff), stdin);
        if(!strncasecmp(ASX_UNI, buff, strlen(ASX_UNI))) {
                debug("Detected ASX Unicast format");
                unicast_asx();
        } else if(!strncasecmp(ASX_MULTI, buff, strlen(ASX_MULTI))) {
                debug("Detected pseudo-ASX Multicast format");
                multicast_asx();
        } else {
                debug("No format detected.\n");
        }

        exit(0);
}

void unicast_asx()
{
        char buff[1024], *b;
        while(fgets(buff, sizeof(buff), stdin) != (char *)NULL) {
                if((b = strstr(buff, TOK_UNI)) != (char *)NULL) {
                        b = strtok(b + strlen(TOK_UNI), "\"");
                        debug(" (%s)\n", b);
                        break;
                }
                bzero(buff, sizeof(buff));
        }

        EXEC_UNI;

        return;
}

void multicast_asx()
{
        char buff[1024], *b;
        char multi_ip[16];
        char multi_port[6];

        while(fgets(buff, sizeof(buff), stdin) != (char *)NULL) {
                if((b = strstr(buff, TOK_MULTI1)) != (char *)NULL) {
                        b = strtok(b, "<");
                        snprintf(multi_ip, 16, "%s", b + strlen(TOK_MULTI1) - 1);
                        debug(" (%s", multi_ip);
                } else if((b = strstr(buff, TOK_MULTI2)) != (char *)NULL) {
                        b = strtok(b, "<");
                        snprintf(multi_port, 6, "%s", b + strlen(TOK_MULTI2) - 1);
                        debug(" %s)\n", multi_port);
                }
                bzero(buff, sizeof(buff));
        }

        EXEC_MULTI;

        return;
}

/* EOF */
