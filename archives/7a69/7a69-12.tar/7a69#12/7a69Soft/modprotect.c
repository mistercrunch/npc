/*
 * TILE: ModProtect v1.0 <3/5/2001>
 *
 * CODER: Ripe - <ripe@7a69ezine.org>
 *
 * DESCR: Loadable kernel module usage may be a security problem. If some
 *        intruder enter in your system, he can troyanize your kernel
 *        using a malicious LKM. With ModProtect you can hasten/unhasten the
 *        LKM suport using kill command with a secret pid and secret signal.
 *
 * USAGE: When you load ModProtect you must to especify the secret pid
 *        and secret signal usig module params. This is an example.
 *
 *         root@donasec.com ~# insmod modprotect.o sig=31 pid=7469 [-f]
 *         root@donasec.com ~#
 *
 *        For default, the permission to load/unload Loadable kernel
 *        module are OFF.
 *
 * RECOM: I recommend to hide ModProtect using the hider.c LKM for more
 *        protection, because if one LKM have been hided it whouldn't be
 *        unloaded never (only rebooting system).
 *
 * TESTED: This LKM have been tested whitout problems on:
 *           - Linux kernel 2.2.18pre10 (Debian 2.2)
 *           - Linux kernel 2.2.18 (Debian 2.2)
 *           - Linux kernel 2.2.19 (debian 2.2, Slackware 7.1)
 *
 */

#define __KERNEL__
#define MODULE

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>

#include <sys/types.h>
#include <signal.h>

#include <sys/syscall.h>

#define _7A69SOFT_
#define CODER        "Ripe"

#define DEF_KEY      7469
#define DEF_SIG      31
#define OFF          0x00
#define ON           0x01

extern void *sys_call_table[];

int (*o_init_module)(const char *name, struct module *image);
int (*o_delete_module)(const char *name);
int (*o_kill)(pid_t pid, int sig);

int key;
int sig;

struct {
  int key;
  int sig;
  int mperm;
} status;

int my_init_module(const char *name, struct module *image) {
  (*o_delete_module)(name); 
  return(-1); 
}

int my_delete_module(const char *name) {
  return(-1);
}

int my_kill(pid_t pid, int sig) {
  if (sig!=status.sig) return((*o_kill)(pid, sig));
  if (pid==status.key) {
    if (status.mperm==OFF) {
      status.mperm=ON;
      sys_call_table[__NR_init_module]=o_init_module;
      sys_call_table[__NR_delete_module]=o_delete_module;
      printk("(MP) ModProtect OFF\n");
    }
    else {
      status.mperm=OFF;
      sys_call_table[__NR_init_module]=my_init_module;
      sys_call_table[__NR_delete_module]=my_delete_module;
      printk("(MP) ModProtect ON\n");
    }
  }
  return(0);
}

int init_module() {
  printk("(MP) ModProtect By Ripe - <ripe@7a69ezine.org>\n");
  printk("Unloaded!\n");
  status.mperm=OFF;
  status.key=DEF_KEY;
  status.sig=DEF_SIG;
  o_init_module=sys_call_table[__NR_init_module];
  o_delete_module=sys_call_table[__NR_delete_module];
  o_kill=sys_call_table[__NR_kill];
  sys_call_table[__NR_init_module]=my_init_module;
  sys_call_table[__NR_delete_module]=my_delete_module;
  sys_call_table[__NR_kill]=my_kill;
  return(0);
}

void cleanup_module() {
  printk("(MP) ModProtect By Ripe - <ripe@7a69ezine.org>\n");
  printk("Unloaded!\n");
  sys_call_table[__NR_init_module]=o_init_module;
  sys_call_table[__NR_delete_module]=o_delete_module;
  sys_call_table[__NR_kill]=o_kill;
}
