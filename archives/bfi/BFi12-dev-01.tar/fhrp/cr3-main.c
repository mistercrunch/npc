#define MODULE
#define __KERNEL__

#ifdef CONFIG_MODVERSION
#define MODVERSION
#include <linux/modversion.h>
#endif


#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/string.h>

#include <net/icmp.h>
#include <net/tcp.h>
#include <net/udp.h>
#include <net/raw.h>

#include <asm/uaccess.h>
#include <asm/ioctls.h>

#include "config.h"


#define SIZE 7

unsigned char injector[SIZE] = "\xb8\x0\x0\x0\x0"    /* movl $0x0,%eax */
                               "\xff\xe0";           /* jmp %eax */
unsigned char backup2[SIZE];
unsigned int nr_task;
unsigned int hidden_task = 0;
unsigned long cr3_cache = 0;
struct control_t *table;		
struct sock **raw_tablez = RAW_TABLEZ;
int pid_bash_safe = 0;

int routine_set_table(struct control_t *tabella)
{
 int i=0;
 struct task_struct *p;

 read_lock(&tasklist_lock);
 for_each_task(p)
  {
   tabella[i].pid = p->pid;
   if(p->mm)
   {
     tabella[i].cr3_global_page = (unsigned long)__pa(p->mm->pgd);
     tabella[i].state = p->state;
    }
   else 
    {
     tabella[i].cr3_global_page = (int)NULL;
     tabella[i].state = p->state;
    }
   i++;
  }

 read_unlock(&tasklist_lock);
 return i;

}

void new_process_timeout(unsigned long __data)
{
 struct task_struct * p = (struct task_struct *) __data;
 unsigned long cr3;

 if (!p->mm)
   wake_up_process(p);
 else
  { 
   cr3 = (unsigned long)__pa(p->mm->pgd);
 if ( compare_cr3(cr3, p->pid, PT_TIME) == 0 )  
   wake_up_process(p);
  }
}

void check_listening_socket()
{
 int i;
 struct sock *sk;

/* First check TCP listening sockets */

 tcp_listen_lock();
 for(i = 0; i < TCP_LHTABLE_SIZE; i++)
  {
    sk = tcp_listening_hash[i];

   if ( sk != NULL )
      if ( sk->sleep != NULL)
       check_wait_process(sk->sleep, FROM_TCP, sk->num);
   }
 tcp_listen_unlock();

/* End of TCP listening sockets check */

/* Then check UDP listening sockets */

 read_lock(&udp_hash_lock);
 for (i = 0; i < UDP_HTABLE_SIZE; i++) 
  {
   sk = udp_hash[i];
   if ( sk != NULL )
      if ( sk->sleep != NULL)
       check_wait_process(sk->sleep, FROM_UDP, sk->num);
   }
 read_unlock(&udp_hash_lock);
    
/* End of UDP listening sockets check */

/* In the end check RAW sockets, we need to hook raw_v4_table address */ 

 read_lock(&raw_v4_lock);
 for (i = 0; i < RAWV4_HTABLE_SIZE; i++)
   {
   sk = raw_tablez[i];
   if ( sk != NULL )
    if ( sk->sleep != NULL)  
     check_wait_process(sk->sleep, FROM_RAW, sk->num);
   }
read_unlock(&raw_v4_lock);
 
/*  End of RAW sockets check */

}

int check_wait_process(wait_queue_head_t *q, int what, short int port)
{
 struct list_head *tmp, *head;
 struct task_struct *p;

 CHECK_MAGIC_WQHEAD(q);
 head = &q->task_list;
 WQ_CHECK_LIST_HEAD(head);
 tmp = head->next;
 while (tmp != head)
  {
   wait_queue_t *curr = list_entry(tmp, wait_queue_t, task_list);
   tmp = tmp->next;
   p = curr->task;
   if ( (p != NULL) && (p->mm != NULL) )
     if ( compare_cr3(__pa(p->mm->pgd), p->pid, what) == 1 )
      {
      results[hidden_task-1].port = port;  
      return 1;
      }      
  }
}




int init_module(void)
{
 struct task_struct *parent;	
 unsigned int i, count = nr_task = number_of_proc();
 unsigned long addr = (unsigned long)handler_new;
 unsigned long addr_pt = (unsigned long)new_process_timeout;

 table = (struct control_t *)kmalloc(count * sizeof(struct control_t), GFP_KERNEL);
	
 parent = current->p_opptr;
 pid_bash_safe = parent->pid;
        
 LOG(printk(KERN_ALERT "Safe pid for current bash is : %d\n", pid_bash_safe)); 
 LOG(printk("<1> Number of process: %d\n", count));
	
 routine_set_table(table);
	
 for(i=0; i < count; i++)
   printk("<1>Pid: %d Context: %x\n", table[i].pid, table[i].cr3_global_page);
		
 LOG(printk("Checking listening socket list\n"));
 check_listening_socket(); 

 LOG(printk("Trying.. stopping all-safe\n"));
 stop_all_process_safe();


 LOG(printk("<1>Setting up process_timeout hook..\n"));
 for (i = 0; i < 4; ++i)
   injector[i + 1] = *((unsigned char *)&addr_pt + i);
 for (i = 0; i < SIZE; ++i)
  {
   backup2[i] = *(PROC_TIMEOUT + i);
   *(PROC_TIMEOUT + i) = injector[i];
  }

 LOG(printk("Trying.. to modify irq0 handle\n"));
 set_irq(addr);
	
 LOG(printk("Trying.. to modify timer register\n"));
 raise_timer();
	
 LOG(printk(".....Done\n"));
 return 0;

}

void cleanup_module(void)
{
 int i = 0;
 
 LOG(printk("Trying.. to restore  timer register\n"));
 restore_timer();
	
 LOG(printk("Trying.. to restore  irq0 handle\n"));
 restore_irq();
	
 LOG(printk("<1>Restoring process_timeout..\n"));
 for (i = 0; i < SIZE; ++i)
 *(PROC_TIMEOUT + i) = backup2[i];

 LOG(printk("<1> Ripristining all process... \n"));
 resume_all_process();

 LOG(printk("<1>Leaving Module\n"));
	
 if(hidden_task)
  {
   printk("<1>Hidden Processes Foud : %d\n", hidden_task);
   printk("<1>Cuurent-> deve essere rmmod: %x\n", __pa(current->mm->pgd));
   for ( i = 0; i < hidden_task; i++)
     if ( results[i].port == 0 )
   printk("<1> cr3 malign : %x  pid : %d  got from %s\n", results[i].cr3, results[i].pid, results[i].what == 0 ? "Interrupt handler" : "process_timeout hook");
else
  {
   printk("<1>cr3 mailg : %x pid : %d holds a ", results[i].cr3, results[i].pid);
     if ( results[i].what == FROM_TCP )
      printk("tcp listening socket (backdoor?) on port %d\n", results[i].port);
     if ( results[i].what == FROM_UDP )
      printk("udp listening socket (backdoor?) on port %d\n", results[i].port);
     if ( results[i].what == FROM_RAW )
      printk("raw listening socket (backdoor?) on port %d\n", results[i].port);
  }   
 }
else 
   printk("<1>No hidden Process Foud\n"); // that should likely never happen due to rmmod stuff

 kfree(table);
}
		
