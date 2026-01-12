#define MODULE
#define __KERNEL__

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/string.h>
#include <linux/timex.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/param.h>
#include <linux/irq.h>
#include "config.h"

struct irqaction temp;
unsigned long address;
static int t = 0;
void (*old_thandle)(int, void *, struct pt_regs *) = TIME_ADDR;

void set_irq(unsigned long addr) 
{
 unsigned long flags;

 save_flags(flags);
 cli();
 memcpy(&temp, IRQ0, sizeof(struct irqaction));  
 address = (unsigned long)temp.handler;
 temp.handler = (void *)addr;
 memcpy(IRQ0, &temp, sizeof(struct irqaction));
 restore_flags(flags); 
}

void raise_timer()
{
 unsigned long flags;

 save_flags(flags);
 cli();
 outb_p(0x34,0x43);
 outb_p(MY_LATCH & 0xff , 0x40);
 outb(MY_LATCH >> 8 , 0x40);
 restore_flags(flags);

}

void restore_timer()
{
 unsigned long flags;

 save_flags(flags);
 cli();
 outb_p(0x34,0x43);
 outb_p(LATCH & 0xff , 0x40);
 outb(LATCH >> 8 , 0x40);
 restore_flags(flags);

}

void restore_irq()
{
 unsigned long flags;

 save_flags(flags);
 cli();
 temp.handler = (void *)address;
 memcpy(IRQ0, &temp, sizeof(struct irqaction));
 restore_flags(flags); 
}

void handler_new(int irq, void *dev_id, struct pt_regs *regs)
{
 compare_cr3(take_global_page_dir(), current->pid, INT_TIME);	    
 if ( t == MY_HZ / HZ )  // that would make the scheduler behave almost the same 
 {
   old_thandle(irq, dev_id, regs);
   t = 0; 
 }
 t++;	  
 
}

