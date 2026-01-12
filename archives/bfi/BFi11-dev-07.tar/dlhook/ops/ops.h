/*
 *  ops.h <header for packet_recvmsg hook funcs...>
 *
 *  070302  ulix <ulix@gmx.it>
 */

#ifdef __KERNEL__

#include <linux/config.h>
#ifndef UTS_RELEASE
  #include <linux/version.h>
#endif

#ifndef KERNEL_VERSION
  #define KERNEL_VERSION(vers,rel,seq) (((vers)<<16) | ((rel)<<8) | (seq))
#endif

/** only with 2.2, 2.4 **/
#if (LINUX_VERSION_CODE & 0xff00) == 0x100
  #error "Please don't use linux-2.1.x, use 2.2.x instead"
#endif
#if (LINUX_VERSION_CODE & 0xff00) == 0x300
  #error "Please don't use linux-2.3.x, use 2.4.x instead"
#endif
/*
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)
  #error "kernel too recent, this module won't run"
#endif
*/

#undef LINUX_24
#undef LINUX_22

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
  #define LINUX_24
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
  #define LINUX_22
#endif


#ifdef LINUX_24
  #ifndef CONFIG_NETFILTER
    #error "Netfilter support is not enabled in kernel configuration"
  #endif
  #include <linux/netfilter.h>
  #include <linux/netfilter_ipv6.h>
#else
  #define NF_STOLEN	0
  #define NF_DROP	0
  #define NF_ACCEPT	0
#endif

#ifndef LINUX_24
  #define net_device device   /** before 2.4 'net_device' struct was 'device'**/
#endif

#ifndef module_init
  #define module_init(x)	int init_module(void) { return x(); }
  #define module_exit(x)	void cleanup_module(void) { x(); }
#endif


/** dump packets utils: .../net/ipv6* rulez **/
#define dip(addr) ntohs((addr).s6_addr16[0]), ntohs((addr).s6_addr16[1]), \
                  ntohs((addr).s6_addr16[2]), ntohs((addr).s6_addr16[3]), \
                  ntohs((addr).s6_addr16[4]), ntohs((addr).s6_addr16[5]), \
                  ntohs((addr).s6_addr16[6]), ntohs((addr).s6_addr16[7])

#define HDR_LEN   sizeof(struct ipv6hdr)

/**
 our pointer to the original packet_recvmsg func
**/
int (*o_recvmsg)
(struct socket *sock, struct msghdr *msg, int l, int f, struct scm_cookie *scm);

/**
 cast proto_ops, *ops now has the original struct:
 OPS is the address of struct proto_ops automatically
 defined during compilation...
**/
struct proto_ops *ops = (struct proto_ops *)(OPS);

#endif /* __KERNEL__ */
