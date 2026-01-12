/*
 *  struct.c <test struct hook routine...>
 *
 *  010302  ulix <ulix@gmx.it>
 */

#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

#ifdef DEBUG
#  define dbg(fmt, args...) \
        printk(KERN_DEBUG "struct: %s: " fmt, __FUNCTION__ , ##args)
#else
#  define dbg(fmt, args...)
#endif

#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/types.h>
#include <linux/errno.h>

#include <linux/socket.h>
#include <linux/net.h>

/**
 cast net_proto_family, *pf now has the original struct
 OPS is the address of struct net_proto_family defined
 during compilation automatically...
**/
struct net_proto_family *pf = (struct net_proto_family *)0xc02f3f24;

int init_module (void)
{
	dbg("pf->family....... %d\n", pf->family);
	dbg("pf->create....... %p\n", pf->create);	

	return (0);
}

void cleanup_module (void)
{
	dbg("module unloaded...\n");
}
