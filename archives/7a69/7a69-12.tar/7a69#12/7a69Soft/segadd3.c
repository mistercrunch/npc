/*
 * TILE: SegAdd3 v1.2 <19/2/2001>
 *
 * CODER: Ripe - <ripe@7a69ezine.org>
 *
 * DESCR: With this modules, only root cat view other EUID process. This
 *        LKM is very useful if you bestow shell acounts on your system
 *        and don't want that a user can view olther user process.
 *
 * USAGE: The usage is very simple. You solely must to load the segadd3
 *        LMK.
 *
 *         root@donasec.com ~# insmod segadd3.o [-f]
 *         root@donasec.com ~#
 *
 * TESTED: This LKM have been tested without problems on:
 *           - Linux kernel 2.2.18pre10 (Debian 2.2)
 *           - Linux kernel 2.2.18 (Debian 2.2)
 *
 *         This LKM have been tested whith a strange problem on:
 *           - Linux kernel 2.2.19 (Slackware 7.1)
 *
 * GRETZ: To YbY for the idea :)
 *
 */


#define __KERNEL__
#define MODULE

#include <linux/ctype.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>

#include <linux/fd.h>
#include <linux/unistd.h>
#include <linux/fs.h>
#include <asm/fcntl.h>
#include <linux/proc_fs.h>
#include <linux/dirent.h>
#include <linux/file.h>

#include <sys/syscall.h>

#define _7A69SOFT_
#define CODER       "Ripe"
extern void *sys_call_table[];

int (*o_getdents)(unsigned int fd, struct dirent *dirp, unsigned int count);
int (*o_geteuid)(void);

int my_atoi(char *str) {
  int res=0;
  int mul=1;
  char *ptr;
  for (ptr=str+strlen(str)-1 ; ptr>=str ; ptr--) {
    if ((*ptr<'0') || (*ptr>'9')) return(-1);
    res+=(*ptr-'0')*mul;
    mul*=10;
  }
  return(res);
}

struct task_struct *get_task_struct(pid_t pid) {
  struct task_struct *p=current;
  do {
    if (p->pid == pid) return p;
    p=p->next_task;
  } while (p!=current);
  return(NULL);
}

void my_bzero(char *buf, int size) {
  memset(buf, 0, size);
}

int my_getdents(unsigned int fd, struct dirent *dirp, unsigned int count) {
  int ret=0;
  int o_ret=(*o_getdents)(fd, dirp, count);
  int euid=(*o_geteuid)(); 
  int pid;
  int tmp=0;
  struct task_struct *task;
  struct dirent *dirp_cpy, *dirp_ret, *dirp_cpy2, *dirp_ret2;
  if (o_ret<=0 || !euid) return(o_ret);
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
    task=get_task_struct((pid_t)pid);
    if (((task!=NULL) && (task->euid==euid)) || (task==NULL)) {
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

int init_module(void) {
  o_getdents=sys_call_table[__NR_getdents];
  o_geteuid=sys_call_table[__NR_geteuid];
  sys_call_table[__NR_getdents]=my_getdents;
  printk("SegAdd3 v1.2 By Ripe - <ripe@7a69ezine.org>\n");
  printk("(SA) Loaded!\n");
  return(0);
}

void cleanup_module()
{
  sys_call_table[__NR_getdents]=o_getdents;
  printk("SegAdd v1.2 by Ripe - <ripe@7a69ezine.org>\n");
  printk("(SA) UnLoaded!\n");
}

MODULE_AUTHOR("Ripe - <ripe@7a69ezine.org>");
MODULE_DESCRIPTION("Security: Process protection");
