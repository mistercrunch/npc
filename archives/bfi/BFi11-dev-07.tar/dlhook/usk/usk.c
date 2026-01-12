/*
 *  usk.c <the simplest way to remove dl layer...>
 *
 *
 *  280202 ulix <ulix@gmx.it>
 */

#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

#ifdef DEBUG
#  define dbg(fmt, args...) \
	printk(KERN_DEBUG "uks: %s: " fmt, __FUNCTION__ , ##args)
#else
#  define PKTD(fmt, args...)
#endif

#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/types.h>
#include <linux/sched.h>
#include <linux/errno.h>

#include <net/sock.h>

void cleanup_module (void)
{
        dbg("module unloaded...\n");
}

int init_module (void)
{
	sock_unregister(PF_PACKET);
        dbg("socket (PF_PACKET) unregistered...\n");
        dbg("module loaded...\n");
        return (0);
}
