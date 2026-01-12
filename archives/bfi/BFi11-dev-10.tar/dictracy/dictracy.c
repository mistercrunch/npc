/*
 * Dictracy Loadable Kernel Module
 *                          by -  twiz   - twiz@email.it
 *                                thefly - thefly@acaro.org
 *
 * That module let you investigate, read, search, dump and write the virtual
 * address space of a process running.
 *
 * From the idea exposed by vecna in rmfbd :
 *    http://www.s0ftpj.org/bfi/dev/BFi11-dev-06
 *
 * Thanks : silvio, Liv Tyler (by thefly)
 */

#define __KERNEL__
#define MODULE
#define __NR_getvirtaddr  224
#define __NR_rwfromvirt 223

#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/sys.h>
#include <linux/smp_lock.h>
#include <linux/unistd.h>
#include <linux/highmem.h>
#include "dictracy.h"


/* You need to retrieve from System.map the correct address to use that two
 * functions, if they aren't exported - check from /proc/ksyms
 *
 * # nm /usr/src/linux/vmlinux | sort | grep handle_mm_fault
 * 00000000c011d65c T handle_mm_fault
 */
 
int (*handle_mm_fault_e) (struct mm_struct *,struct vm_area_struct *, unsigned long , int ) = (void *)0xc011fc7c;

/* Same there */

struct vm_area_struct *(*find_extend_vma_e) (struct mm_struct *, unsigned long) = (void *)0xc01208a4;

extern void* sys_call_table[];
struct page * getpagefromaddr(unsigned long, struct task_struct *);


struct task_struct *get_task(pid_t pid)
{
	struct task_struct *p, **htable = &pidhash[pid_hashfn(pid)];
	read_lock(&tasklist_lock);
        for(p = *htable; p && p->pid != pid; p = p->pidhash_next)
	                ;
	read_unlock(&tasklist_lock);
	return p;
}


int getvirtaddr(struct mem_addr *temmem, int pid)
{ 
 struct task_struct *temp;

 if ( (temp = get_task(pid)) == NULL )
   return 1;
  
 temmem->startcode = temp->mm->start_code;
 temmem->endcode = temp->mm->end_code;
 temmem->startdata = temp->mm->start_data;
 temmem->enddata = temp->mm->end_data;
 temmem->startbrk = temp->mm->start_brk;
 temmem->startstack = temp->mm->start_stack;

 return 0;
}



/*
 * If you're used to ptrace code (kernel/ptrace.c) you should recognize 
 * something in the following lines :) 
 */

struct page * getpagefromaddr(unsigned long addr, struct task_struct *task) 
{ 
 pgd_t *pgd;
 pmd_t *pmd;
 pte_t *pte;
 struct vm_area_struct *vma;
 struct mm_struct *mm;
 
startpoint:
 
 task_lock(task);
 mm = task->mm;
  if (mm)
     atomic_inc(&mm->mm_users);
 task_unlock(task);
  if (!mm)
     return NULL;
 
 spin_lock(&task->mm->page_table_lock);
  
 pgd = pgd_offset(mm, addr);
 if (pgd_none(*pgd))
   goto fault;

 if (pgd_bad(*pgd))
  { 
   pgd_ERROR(*pgd);
   goto error;
  }

 pmd = pmd_offset(pgd, addr);
 if (pmd_none(*pmd))
  goto fault;

 if (pmd_bad(*pmd))
  {
   pmd_ERROR(*pmd);
   goto error;
  } 
 
 pte = pte_offset(pmd, addr);
 
 spin_unlock(&task->mm->page_table_lock);
 
 if (!pte_present(*pte))
   goto fault;
 
 return pte_page(*pte);

fault:
 spin_unlock(&task->mm->page_table_lock);
 vma = find_extend_vma_e(mm, addr);
 atomic_dec(&mm->mm_users);
 if ( handle_mm_fault_e(mm, vma, addr, 0) > 0) 
  goto startpoint;
 else
  return NULL;
error:
 spin_unlock(&task->mm->page_table_lock);
 atomic_dec(&mm->mm_users);
 return NULL; 
} 

int rwfromvirt(int pid, char *buffer, int len, unsigned long addr, int w)
{ 
 struct task_struct *task;
 struct page *page;
 struct mm_struct *mm;
 char *maddr;

 if ( (task = get_task(pid)) == NULL )
  return 1;

 page = getpagefromaddr(addr, task);
  if ( page == NULL )
   return -1;
 
 if (w)
  {
   if ((!VALID_PAGE(page)) || PageReserved(page))
     return -2;

   maddr = kmap(page);
   memcpy(maddr + (addr & ~PAGE_MASK), buffer, len);
   flush_page_to_ram(page);
   flush_icache_page(find_extend_vma_e(task->mm, addr), page);
   kunmap(page);
  }
 else 
  {
   maddr = (char *)kmap(page);
   memcpy(buffer, maddr + (addr & ~PAGE_MASK), len);
   flush_page_to_ram(page);
   kunmap(page);
  }
 mm = task->mm;
 atomic_dec(&mm->mm_users);
 
 return 0;
}

int init_module() 
{ 
 sys_call_table[__NR_getvirtaddr] = getvirtaddr;
 sys_call_table[__NR_rwfromvirt] = rwfromvirt;
 return 0;
} 

void cleanup_module ()
{
 sys_call_table[__NR_getvirtaddr] = NULL;
 sys_call_table[__NR_rwfromvirt] = NULL;
 printk("<1>Module Unloadded\n");
} 
 
  
