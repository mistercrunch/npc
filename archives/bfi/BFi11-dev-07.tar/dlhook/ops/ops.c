/*
 *  ops.c <test struct hook routine...>
 *
 *  070302  ulix <ulix@gmx.it>
 */

#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

#ifdef DEBUG
#  define dbg(fmt, args...) \
        printk(KERN_DEBUG "ops: %s: " fmt, __FUNCTION__ , ##args)
#else
#  define dbg(fmt, args...)
#endif

#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/types.h>
#include <linux/errno.h>

#include <linux/socket.h>
#include <linux/skbuff.h>
#include <linux/net.h>
#include <net/sock.h>

#include "ops.h"


static int n_recvmsg
(struct socket *sock, struct msghdr *msg, int l, int f, struct scm_cookie *scm)
{
	dbg("recieved packet -- go on...\n");

	return (o_recvmsg(sock, msg, l, f, scm));
}	

static int ops_init (void)
{
	dbg("ops->family....... %d\n", ops->family);
	dbg("ops->release...... %p\n", ops->release);
	dbg("ops->bind......... %p\n", ops->bind);
	dbg("ops->connect...... %p\n", ops->connect);
	dbg("ops->socketpair... %p\n", ops->socketpair);
	dbg("ops->accept....... %p\n", ops->accept);
	dbg("ops->getname...... %p\n", ops->getname);
	dbg("ops->poll......... %p\n", ops->poll);
	dbg("ops->ioctl........ %p\n", ops->ioctl);
	dbg("ops->listen....... %p\n", ops->listen);
	dbg("ops->shutdown..... %p\n", ops->shutdown);
	dbg("ops->setsockopt... %p\n", ops->setsockopt);
	dbg("ops->getsockopt... %p\n", ops->getsockopt);
	dbg("ops->sendmsg...... %p\n", ops->sendmsg);
	dbg("ops->recvmsg...... %p\n", ops->recvmsg);
	dbg("ops->mmap......... %p\n", ops->mmap);
	dbg("ops->sendpage..... %p\n", ops->sendpage);

	/** saving old func **/
	o_recvmsg = ops->recvmsg;
	dbg("o_recvmsg......... %p\n", o_recvmsg);
	/** setting new func **/
	ops->recvmsg = n_recvmsg;
	dbg("ops->recvmsg...... %p\n", ops->recvmsg);

	return (0);
}

static void ops_exit (void)
{
	/** resetting old func **/
	dbg("ops->recvmsg...... %p\n", ops->recvmsg);
	ops->recvmsg = o_recvmsg;
	dbg("ops->recvmsg______ %p\n", ops->recvmsg);

	return;
}

module_init(ops_init);
module_exit(ops_exit);
