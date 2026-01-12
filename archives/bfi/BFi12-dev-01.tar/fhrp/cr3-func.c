#define MODULE
#define __KERNEL__

#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/string.h>
#include "config.h"

extern int pid_bash_safe;

unsigned long int take_global_page_dir()
{
	__asm__ __volatile__ ("movl %cr3, %eax");
}

int number_of_proc(void)
{
 unsigned int counter = 0;
 struct task_struct *p;
 for_each_task(p)
  {
   counter++;
  }
 return counter;
}

int stop_all_process_safe()
{
 int counter=0;
 struct task_struct *p;
 int t;
 read_lock(&tasklist_lock);
 for_each_task(p)
 {
  if(p->mm) 
   { 
    /* don't stop kernel thread */
    t = p->pid;
    if((t != pid_bash_safe ) && (t != SAFE_P_KLOGD) && (t != SAFE_P_SYSLOGD) && (t != SAFE_P_INIT))
     {
      /* nor "safe processes" */
      if((p->state != TASK_UNINTERRUPTIBLE) && (p != current))
       {
        counter++;
        force_sig(SIGSTOP, p);
       }
      }
    }
 }
read_unlock(&tasklist_lock);
return counter;
}

int resume_all_process()
{
 int counter=0,t,i;
 struct task_struct *p;
 for_each_task(p)
  {
   if(p->mm)
    { 
     /* don't stop kernel thread */
     t = p->pid;
     if((t !=  pid_bash_safe) && (t != SAFE_P_KLOGD) && (t != SAFE_P_SYSLOGD) && (t != SAFE_P_INIT))
      { 
     /* nor "safe processes"*/
       if((p->state != TASK_UNINTERRUPTIBLE) && (p!=current))
        {	
         /* nor uninterruptible processes */
        for(i=0; i < nr_task; i++)
          {
            if(__pa(p->mm->pgd) == table[i].cr3_global_page)
            force_sig(SIGCONT, p);
          }
 
	counter++;
 	}
      }		
    }
  }

 return counter;

}
