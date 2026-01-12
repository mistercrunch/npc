/*******************************************************************************
*                                                                              *
*            __  __    _    ____                                               *
*           |  \/  |  / \  / ___|      __ _ _   ___   _____ _ __               *
*           | |\/| | / _ \| |   _____ / _` | | | \ \ / / _ \ '__|              *
*           | |  | |/ ___ \ |__|_____| (_| | |_| |\ V /  __/ |                 *
*           |_|  |_/_/   \_\____|     \__, |\__, | \_/ \___|_|                 *
*                                     |___/ |___/                              *
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
*   # cc -o MAC-gyver MAC-gyver.c `libnet-config --defines --libs` -lpcap      *
*                                                                              *
*                                                                              *
*******************************************************************************/

#define ETH_FILE "/etc/ethernet_hosts.conf"
#define ERRBUF_SIZE ((PCAP_ERRBUF_SIZE > LIBNET_ERRBUF_SIZE) ? PCAP_ERRBUF_SIZE : LIBNET_ERRBUF_SIZE)
#define READ_TIMEOUT 1000
#define SNAPLEN 1564
#define PROMISC 1
#define DEBUG 0
#define LOOP -1
#define debugf(x...); if(debug) { printf(x); fflush(stdout); }

#include <pcap.h>
#include <libnet.h>

struct interface {
        int id;
        char *name;
        pcap_t *pcap_desc;
        char *filterstr;
        struct libnet_link_int *net_link;
        unsigned char *out_packet;
};

struct ether_addr *ext_hw_addr = (struct ether_addr *)NULL;
struct interface internal, external;
char *eth_file = (char *)NULL;
char debug = DEBUG;

extern char pcap_version[];

int main(int argc, char *argv[]);
void getparms(int argc, char* argv[]);
void usage(char *progname);
void initialize(struct interface *iface);
int ether_readfile(void);
void heat(char *id, struct pcap_pkthdr *pcap_h, u_char *out_packet);

int main(int argc, char* argv[]) {
        /* Set id flags */
        internal.id = 0;
        external.id = 1;

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
        if(fork()) {
                /* Parent process - internal interface */
                pcap_loop(internal.pcap_desc, LOOP, (pcap_handler)heat, (u_char *)internal.id);
        } else {
                /* Child process - external interface */
                pcap_loop(external.pcap_desc, LOOP, (pcap_handler)heat, (u_char *)external.id);
        }

        /* Never here */
        exit(0);
}

void getparms(int argc, char* argv[]) {
        char optc;

        /* Parse command line params */
        while((optc = getopt(argc, argv, "i:e:f:dh")) != EOF) {
                switch(optc) {
                        case 'i':
                                internal.name = optarg;
                                break;
                        case 'e':
                                external.name = optarg;
                                break;
                        case 'f':
                                eth_file = optarg;
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

        if(eth_file == (char *)NULL) {
                eth_file = (char *)malloc(strlen(ETH_FILE) + 1);
                memcpy(eth_file, ETH_FILE, strlen(ETH_FILE));
                eth_file[strlen(ETH_FILE)] = '\0';
        }
        debugf("Set ethernet address file to: %s\n", eth_file);

        return;
}

void usage(char *progname)
{
        /* Program usage */
        fprintf(stderr, "usage:\t%s -i internal_interface -e external_interface [-d] [-f addresses_file]\n\t%s -h\n\n", progname, progname);
        return;
}

void initialize(struct interface *iface){
        char errorstr[ERRBUF_SIZE];
        struct bpf_program bpf_p;

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

        /* Create filter ruleset */
        debugf("[%s] Setting interface data..\n", iface->name);
        if(iface->id) {
                /* External */
                if((ext_hw_addr = libnet_get_hwaddr((struct libnet_link_int *)NULL, iface->name, (u_char *)errorstr)) == (struct ether_addr *)NULL) {
                        libnet_error(0, "[%s] %s\n", iface->name, errorstr);
                        pcap_close(iface->pcap_desc);
                        exit(EXIT_FAILURE);
                }
                debugf("[%s] External hardware address set to [%02x:%02x:%02x:%02x:%02x:%02x]\n", iface->name,
                        ext_hw_addr->ether_addr_octet[0], ext_hw_addr->ether_addr_octet[1], ext_hw_addr->ether_addr_octet[2],
                        ext_hw_addr->ether_addr_octet[3], ext_hw_addr->ether_addr_octet[4], ext_hw_addr->ether_addr_octet[5]);

                iface->filterstr = (char *)malloc((size_t)37);

                sprintf(iface->filterstr, "not ether src host %02x:%02x:%02x:%02x:%02x:%02x",
                        ext_hw_addr->ether_addr_octet[0], ext_hw_addr->ether_addr_octet[1], ext_hw_addr->ether_addr_octet[2],
                        ext_hw_addr->ether_addr_octet[3], ext_hw_addr->ether_addr_octet[4], ext_hw_addr->ether_addr_octet[5]);
            } else {
                /* Internal */
                if(ether_readfile()) {
                        pcap_close(iface->pcap_desc);
                        exit(EXIT_FAILURE);
                }
        }

        if(pcap_compile(iface->pcap_desc, &bpf_p, iface->filterstr, 1, 0) < 0) {
                pcap_perror(iface->pcap_desc, (char *)NULL);
                pcap_close(iface->pcap_desc);
                exit(EXIT_FAILURE);
        }
        free(iface->filterstr);
        
        /* Set the filter */
        if(pcap_setfilter(iface->pcap_desc, &bpf_p) < 0)
        {
                pcap_perror(iface->pcap_desc, (char *)NULL);
                pcap_close(iface->pcap_desc);
                exit(EXIT_FAILURE);
        }
#if PCAP_VERSION_MINOR >= 6
        pcap_freecode(&bpf_p);
#endif
        debugf("[%s] OK\n", iface->name);

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

int ether_readfile()
{
        FILE *fp;
        unsigned int tmp_addr[ETHER_ADDR_LEN];
        int line = 0, entries = 0;
        char tmp_line[1024];

        /* Open ethernet address file */
        if((fp = fopen(eth_file, "r")) == (FILE *)NULL) {
                fprintf(stderr, "Error opening %s\n", eth_file);
                return(1);
        }

        /* Count valid entries */
        while(1) {
                if((fgets(tmp_line, 1024, fp)) == (char *)NULL) {
                        break;
                }
                line++;
                if((tmp_line[0] == '#') || (strlen(tmp_line) <= 1) || (tmp_line[0] == ' ')) {
                        continue;
                }
                if(sscanf(tmp_line, "%x:%x:%x:%x:%x:%x", &tmp_addr[0], &tmp_addr[1], &tmp_addr[2],
                                                         &tmp_addr[3], &tmp_addr[4], &tmp_addr[5]) != 6) {
                        fprintf(stderr, "Error on %s (line:%d)\n", eth_file, line);
                        fclose(fp);
                        return(1);
                }
                entries++;
        }

        /* Create filter rules */
        internal.filterstr = (unsigned char *)malloc((size_t)(36 * entries));
        rewind(fp);
        while(1) {
                if((fgets(tmp_line, 1024, fp)) == (char *)NULL) {
                        break;
                }

                if((tmp_line[0] == '#') || (strlen(tmp_line) <= 1) || (tmp_line[0] == ' ')) {
                        continue;
                }

                sscanf(tmp_line, "%x:%x:%x:%x:%x:%x", &tmp_addr[0], &tmp_addr[1], &tmp_addr[2],
                                                      &tmp_addr[3], &tmp_addr[4], &tmp_addr[5]);

                if(!strlen(internal.filterstr)) {
                        sprintf(internal.filterstr, "ether src host %02x:%02x:%02x:%02x:%02x:%02x", tmp_addr[0], tmp_addr[1],
                                                                           tmp_addr[2], tmp_addr[3], tmp_addr[4], tmp_addr[5]);
                } else {
                        sprintf(internal.filterstr, "%s or ether src host %02x:%02x:%02x:%02x:%02x:%02x", internal.filterstr,
                                                tmp_addr[0], tmp_addr[1], tmp_addr[2], tmp_addr[3], tmp_addr[4], tmp_addr[5]);
                }
                debugf("[%s] Added hardware address [%02x:%02x:%02x:%02x:%02x:%02x]\n", internal.name,
                        tmp_addr[0], tmp_addr[1], tmp_addr[2], tmp_addr[3], tmp_addr[4], tmp_addr[5]);
        }

        /* Close file and return */
        fclose(fp);
        return(0);
}

void heat(char *id, struct pcap_pkthdr *pcap_h, u_char *out_packet) {
        struct interface *iface;
        
        /* Check where the packet is coming from */
        if((int)id) {
                /* From external interface to internal interface */
                iface = &internal;
        } else {
                /* From internal interface to external interface */
                memcpy(((struct libnet_ethernet_hdr *)out_packet)->ether_shost, ext_hw_addr->ether_addr_octet, ETHER_ADDR_LEN);
                iface = &external;
        }        

        /* Write the packet */
        libnet_write_link_layer(iface->net_link, iface->name, out_packet, pcap_h->len); 
        debugf((int)id ? "." : "o");
}

/******************************************************************************/
