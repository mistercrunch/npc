/* related sistem define */

#define SAFE_P_KLOGD 66
#define SAFE_P_SYSLOGD 63 
#define SAFE_P_INIT 1

#define MY_HZ 1000
#define MY_LATCH  ((CLOCK_TICK_RATE + MY_HZ/2) / MY_HZ) 

#define IRQ0 (void *)0xc0249fe8     
/* root@twiz:~# nm /usr/src/linux/vmlinux | grep irq0
   00000000c0249fe8 d irq0 */

#define TIME_ADDR (void(*) (int, void *, struct pt_regs *))0xc010a8ac   
/* root@twiz:~# nm /usr/src/linux/vmlinux | grep timer_interrupt
   00000000c010a8ac t timer_interrupt */

#define PROC_TIMEOUT (unsigned char *)0xc010eca0
/* root@twiz:~# nm /usr/src/linux/vmlinux | grep process_timeout
   00000000c010eca0 t process_timeout */

#define RAW_TABLEZ (struct sock **)0xc02cb920
/* root@twiz:~# nm /usr/src/linux/vmlinux | grep raw_v4_htable
   00000000c02cb920 B raw_v4_htable */


#define MAX_RESULTS 10
#define INT_TIME 0
#define PT_TIME 1

#define FROM_TCP 2
#define FROM_UDP 3
#define FROM_RAW 4

struct control_t {
        int state;
        int pid;
        unsigned long cr3_global_page;

};

struct report_data {
	unsigned long cr3;
	int pid;
	int what;
        short int port;        /* Used just in listening sockets checking */
} results[MAX_RESULTS];         

extern unsigned int nr_task;
extern unsigned int hidden_task;
extern struct control_t *table;
extern unsigned long cr3_cache;    

struct task_struct *stop_a_process(int, int);
int stop_all_process_safe(void);
int resume_all_process(void);
int number_of_proc(void);
void set_irq(unsigned long);
void restore_irq(void);
void raise_timer(void);
void restore_timer(void);
void handler_new(int, void *, struct pt_regs *);
int check_wait_process(wait_queue_head_t *q, int what, short int port);

static inline int compare_cr3(unsigned long cr3, int pid, int what)
{
 int i;

 for(i=0; i< nr_task; i++)      /* using global nr_task, table, ... defined in cr3-main.c */
   if(cr3 == table[i].cr3_global_page)
     return 0;

 if ( hidden_task == 0)
  {
   results[hidden_task].cr3 = cr3;
   results[hidden_task].pid = pid;
   results[hidden_task].what = what;
   hidden_task++;
   cr3_cache = cr3;
   return 1;
  }

 if ( cr3_cache == cr3 || hidden_task == MAX_RESULTS) 
  return 1; 

 for( i = 0; i < hidden_task; i++ )
  { 
   if ( cr3 == results[i].cr3 )
    {
     cr3_cache = cr3;  
     return 1;
    }
  } 

 results[hidden_task].cr3 = cr3;
 results[hidden_task].pid = pid;
 results[hidden_task].what = what;

 hidden_task++;
 cr3_cache = cr3;

 return 1;
}

/* others */
#define DEBUG__CR3  	/* enable debugging */

#ifdef DEBUG__CR3
#define LOG(x) x
#else
#define LOG(x)
#endif
