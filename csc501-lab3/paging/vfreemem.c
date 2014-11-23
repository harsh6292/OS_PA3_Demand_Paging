/* vfreemem.c - vfreemem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <proc.h>
#include<paging.h>
extern struct pentry proctab[];
/*------------------------------------------------------------------------
 *  vfreemem  --  free a virtual memory block, returning it to vmemlist
 *------------------------------------------------------------------------
 */
SYSCALL	vfreemem(block, size)
	struct	virtualblock *block;
	unsigned int size;
{
	//kprintf("To be implemented!\n");
	STATWORD ps;  
	struct pentry * pptr;
	struct virtualblock *p, *q, *first_ptr;
	unsigned top;
	
	pptr= &proctab[currpid];
	
	if (size==0 || (unsigned)block>((unsigned)((pptr->vhpnpages)*4096)+(4096*4096)) || ((unsigned)block)<((unsigned) NBPG*4096) )
	{
		//kprintf("\nInside VFREEMEM(): Error! %d, %d, %d", (unsigned)block, (unsigned)((pptr->vhpnpages)*4096), (unsigned) NBPG*4096);
		restore(ps);
		return(SYSERR);
	}
	
	//kprintf("\nInside VFREEMEM():  %d, %d, %d", (unsigned)block,((unsigned)((pptr->vhpnpages)*4096)+(4096*4096)), (unsigned) NBPG*4096);
	size = (unsigned)roundmb(size);
	disable(ps);
	
	first_ptr = pptr->vmemlist;
	
	//q=curr//
	//p=next
	
	
	
	
	//kprintf("\n here here %d, %d, size %d", block, first_ptr->mnext->vaddr, size);
	block->vlen = size;
	block->mnext = p;
	block->vaddr = (unsigned)block;
	
	
		//kprintf("\n block + len %d and initia length %d",block->vaddr + block->vlen, (pptr->vmemlist)->mnext->vlen);
	
	if( (block->vaddr + block->vlen) < first_ptr->mnext->vaddr)
	{
		//kprintf("\n here %d", block);
		block->mnext = first_ptr->mnext;
		first_ptr->mnext = block;
	}
	
	else if (((block->vaddr + block->vlen) > first_ptr->mnext->vaddr) && (block->vaddr <=first_ptr->mnext->vaddr ))
	{
		//kprintf("\n else here ");
		first_ptr->mnext->vaddr = block->vaddr;
		first_ptr->mnext->vlen+= (first_ptr->mnext->vaddr  - block->vaddr)*4;
		
	}
	else if((block->vaddr + block->vlen) == first_ptr->mnext->vaddr)
	{
		first_ptr->mnext->vaddr = block->vaddr;
		first_ptr->mnext->vlen+= size;
	}
	else
	{
		restore(ps);
		return(SYSERR);
	}
	
	
	restore(ps);
	return(OK);
}

