/*******************************************************************************
*                                                                              *
*                       _         _                                            *
*                      / \       | |_ ___  __ _ _ __ ___                       *
*                     / _ \ _____| __/ _ \/ _` | '_ ` _ \                      *
*                    / ___ \_____| ||  __/ (_| | | | | | |                     *
*                   /_/   \_\     \__\___|\__,_|_| |_| |_|                     *
*                                                                              *
*                                                                              *
*        based on HEAT - Half Ethernet Address Translation tecnology           *
*                   tHE rECIdjVO - recidjvo@pkcrew.org                         *
*                        http://www.pkcrew.org/                                *
*                                                                              *
*      Libraries needed:                                                       *
*      - libpcap - http://www.tcpdump.org/                                     *
*      - Libnet  - http://libnet.sourceforge.net/                              *
*                                                                              *
*   compile with:                                                              *
*   # cc -o A-team A-team.c `libnet-config --defines --libs` -lpcap -pthread   *
*                                                                              *
*                                                                              *
*******************************************************************************/

#define PROGNAME "A-team"
#define ERRBUF_SIZE ((PCAP_ERRBUF_SIZE > LIBNET_ERRBUF_SIZE) ? PCAP_ERRBUF_SIZE : LIBNET_ERRBUF_SIZE)
#define READ_TIMEOUT 1000
#define STACK_TIMEOUT 2
#define STACKLEN 50
#define SNAPLEN 1564
#define PROMISC 1
#define DEBUG 0
#define LOOP -1
#define debugf(x...); if(debug) { printf(x); fflush(stdout); }

#include <pcap.h>
#include <libnet.h>
#include <pthread.h>

struct interface {
        int id;
        char *name;
        pcap_t *pcap_desc;
        struct libnet_link_int *net_link;
        unsigned char *out_packet;
        pthread_t pthread;
};

struct stack_packet {
        char packet[SNAPLEN];
        unsigned int len;
        struct timeval timestamp;
};

struct stack_packet int_recs[STACKLEN];
struct stack_packet ext_recs[STACKLEN];
pthread_mutex_t int_packet_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ext_packet_mutex = PTHREAD_MUTEX_INITIALIZER;
struct ether_addr *ext_hw_addr = (struct ether_addr *)NULL;
struct interface internal, external;
char debug = DEBUG;

extern char pcap_version[];

int main(int argc, char *argv[]);
void getparms(int argc, char* argv[]);
void usage(char *progname);
void initialize(struct interface *iface);
void *threadloop(void *iface);
void heat(char *id, struct pcap_pkthdr *pcap_h, u_char *out_packet);
void push(char *packet, int len, int id, struct timeval *tv);
int pop(char *packet, int len, int id);

int main(int argc, char* argv[]) {
        /* Set id flags */
        internal.id = 0;
        external.id = 1;

        /* Blank record structures */
        bzero(int_recs, sizeof(struct stack_packet) * STACKLEN);
        bzero(int_recs, sizeof(struct stack_packet) * STACKLEN);

        /* Get command line params */
        getparms(argc, argv);

        /* Initialize interfaces */
        initialize(&internal);
        initialize(&external);

        /* Fork && go! */
        if(!debug) {
                /* Go in the daemon land */
                if(fork()) {
                        exit(0);
                }
                setsid();
        }

        debugf("Logging traffic ( . -> from external to internal | o -> from internal to external)\n");
        pthread_create(&external.pthread, NULL, threadloop, (void *)&external);
        threadloop((void *)&internal);

        /* Never here */
        exit(0);
}

void getparms(int argc, char* argv[]) {
        char optc;

        /* Parse command line params */
        while((optc = getopt(argc, argv, "i:e:dh")) != EOF) {
                switch(optc) {
                        case 'i':
                                internal.name = optarg;
                                break;
                        case 'e':
                                external.name = optarg;
                                break;
                        case 'd':
                                debug = 1;
                                 debugf("Set verbose mode ON\n");
                                break;
                        case 'h':
                                usage(argv[0]);
                                exit(0);
                        default:
                                break;
                }
        }
        if((internal.name == (char *)NULL) || (external.name == (char *)NULL)) {
                usage(argv[0]);
                exit(-1);
        }
        debugf("Set internal interface to: %s\n", internal.name);
        debugf("Set external interface to: %s\n", external.name);

        return;
}

void usage(char *progname)
{
        /* Program usage */
        fprintf(stderr, "usage:\t%s -i internal_interface -e external_interface [-d]\n\t%s -h\n\n", progname, progname);
        return;
}

void initialize(struct interface *iface){
        char errorstr[ERRBUF_SIZE];

        debugf("Using libpcap %s\n", pcap_version);
        debugf("Using libnet " LIBNET_VERSION "\n");
        
        debugf("Initializing %s\n", iface->name);

        /* libpcap */
        debugf("[%s] reading.. ", iface->name);
        if((iface->pcap_desc = pcap_open_live(iface->name, SNAPLEN, PROMISC, READ_TIMEOUT, errorstr)) == (pcap_t *)NULL) {
                fprintf(stderr, "\n[%s] %s\n", iface->name, errorstr);
                exit(EXIT_FAILURE);
        }

        if(pcap_datalink(iface->pcap_desc) != DLT_EN10MB) {
                fprintf(stderr, "\n[%s] %s seems not to be an ethernet device.\n", iface->name, iface->name);
                pcap_close(iface->pcap_desc);
                exit(EXIT_FAILURE);
        }
        debugf("OK\n");

        /* Get external interface harwaer address */
        if(iface->id) {
                if((ext_hw_addr = libnet_get_hwaddr((struct libnet_link_int *)NULL, iface->name, (u_char *)errorstr)) == (struct ether_addr *)NULL) {
                        libnet_error(0, "[%s] %s\n", iface->name, errorstr);
                        pcap_close(iface->pcap_desc);
                        exit(EXIT_FAILURE);
                }
                debugf("[%s] External hardware address set to [%02x:%02x:%02x:%02x:%02x:%02x]\n", iface->name,
                        ext_hw_addr->ether_addr_octet[0], ext_hw_addr->ether_addr_octet[1], ext_hw_addr->ether_addr_octet[2],
                        ext_hw_addr->ether_addr_octet[3], ext_hw_addr->ether_addr_octet[4], ext_hw_addr->ether_addr_octet[5]);
        }

        /* libnet */        
        debugf("[%s] writing.. ", iface->name);
        if((iface->net_link = libnet_open_link_interface(iface->name, errorstr)) == (struct libnet_link_int *)NULL) {
                libnet_error(0, "[%s] %s\n", iface->name, errorstr);
                pcap_close(iface->pcap_desc);
                exit(EXIT_FAILURE);
        }
        if(libnet_init_packet(SNAPLEN, &iface->out_packet) == -1) {
                libnet_error(0, "[%s] libnet_init_packet() error.\n", iface->name);
                pcap_close(iface->pcap_desc);
                libnet_close_link_interface(iface->net_link);
                exit(EXIT_FAILURE);
        }
        debugf("OK\n");

        return;
}

void *threadloop(void* iface){
        /* Get packets */
        pcap_loop(((struct interface *)iface)->pcap_desc, -1, (pcap_handler)heat, (u_char *)((struct interface *)iface)->id);
        return NULL;
}

void heat(char *id, struct pcap_pkthdr *pcap_h, u_char *out_packet) {
        struct interface *iface;
        
        /* Check where the packet is coming from */
        if((int)id) {
                /* From external interface to internal interface */
                iface = &internal;

                /* Check for duplicates */
                pthread_mutex_lock(&ext_packet_mutex);
                if (pop(out_packet, pcap_h->len, (int)id)){
                        /* packet is duplicate */
                        pthread_mutex_unlock(&ext_packet_mutex);
                        return;
                }
                push(out_packet, pcap_h->len, (int)id, (struct timeval *)&pcap_h->ts);
                pthread_mutex_unlock(&ext_packet_mutex);

        } else {
                /* From internal interface to external interface */
                iface = &external;

                /* Check for duplicates */
                pthread_mutex_lock(&int_packet_mutex);
                if (pop(out_packet, pcap_h->len, (int)id)){
                        /* packet is duplicate */
                        pthread_mutex_unlock(&int_packet_mutex);
                        return;
                }
                memcpy(((struct libnet_ethernet_hdr *)out_packet)->ether_shost, ext_hw_addr->ether_addr_octet, ETHER_ADDR_LEN);
                push(out_packet, pcap_h->len, (int)id, (struct timeval *)&pcap_h->ts);
                pthread_mutex_unlock(&int_packet_mutex);
        }        

        /* Write the packet */
        libnet_write_link_layer(iface->net_link, iface->name, out_packet, pcap_h->len); 
        debugf((int)id ? "." : "o");
}

/* Free rewrite from hogwash (http://hogwash.sourceforge.net/) */
void push(char *packet, int len, int id, struct timeval* tv) {
        struct stack_packet *seek;
        int i;

        if(id) {
                seek = &ext_recs[0];
        } else {
                seek = &int_recs[0];
        }

        /* Find a free slot */
        for(i = 0; i < STACKLEN; i++) {
                if(!seek[i].len) {
                        memcpy(seek[i].packet, packet, len);
                        seek[i].len = len;
                        memcpy(&seek[i].timestamp, tv, sizeof(struct timeval));
                        return;
                }

                /* Remove expired packets */
                if((tv->tv_sec - seek[i].timestamp.tv_sec) > STACK_TIMEOUT) {
                        seek[i].len = 0;
                        i--;
                }
        }
}

/* Free rewrite from hogwash (http://hogwash.sourceforge.net/) */
int pop(char *packet, int len, int id){
        struct stack_packet *seek;
        int i;

        if(id) {
                seek = int_recs;
        } else {
                seek = ext_recs;
        }

        for(i = 0; i < STACKLEN; i++) {
                if (seek[i].len == len) {
                        if(!memcmp(seek[i].packet, packet, len)) {
                                /* packet is in the stack */
                                seek[i].len = 0;
                                return 1;
                        }
                } 
        }

        return 0;
}

/******************************************************************************/
