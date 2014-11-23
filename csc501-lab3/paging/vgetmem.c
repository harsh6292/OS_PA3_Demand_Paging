/* vgetmem.c - vgetmem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <proc.h>
#include <paging.h>

extern struct pentry proctab[];
/*------------------------------------------------------------------------
 * vgetmem  --  allocate virtual heap storage, returning lowest WORD address
 *------------------------------------------------------------------------
 */
WORD	*vgetmem(nbytes)
	unsigned int nbytes;
{
	STATWORD ps;    
	struct	virtualblock	*p, *q, *leftover;
	struct pentry *pptr;
	
	
	disable(ps);
	
	
	
	pptr= &proctab[currpid];
	//kprintf("\nInside VGETMEM(): for process %d", currpid);
	//kprintf("\nInside VGETMEM(): initial bytes aksed %d and pptr->vmemlist is %d and (pptr->vmemlist)->mnext is %d for process %d, addr %d", nbytes, pptr->vmemlist, (pptr->vmemlist)->mnext, currpid, (((pptr->vmemlist)->mnext))->mnext->vaddr);
	
	//kprintf("\n Inside VGETMEM(): pptr->vmemlist is %d and vmemlist->mnext is %d, addr %d, len %d", pptr->vmemlist, (pptr->vmemlist)->mnext,(struct virtualblock *)((pptr->vmemlist)->mnext)->vaddr, (pptr->vmemlist)->mnext->vlen);
	
	//kprintf("\n Inside VGETMEM(): ADDRESS %d, len %d",     ((pptr->vmemlist)->mnext)->vaddr     , (pptr->vmemlist)->mnext->vlen);
	//kprintf("\n Inside VGETMEM(): vmemlist %d, vmemlist->mnext %d, store %d", pptr->vmemlist, pptr->vmemlist->mnext, pptr->store);
	
	p = pptr->vmemlist->mnext;
	
	//kprintf("\nInside VGETMEM(): p->vaddr %d, p->vlen %d, *p->vaddr %d", p->vaddr, p->vlen, (&(unsigned int)(p->vaddr)));
	if (nbytes==0 )//|| (pptr->vmemlist)->mnext == (struct mblock *) NULL) 
	{
		//kprintf("\nInside VGETMEM(): Error!");
		restore(ps);
		return( (WORD *)SYSERR);
	}
	
	nbytes = (unsigned int) roundmb(nbytes);
	//kprintf("\nInside VGETMEM(): bytes asked : %d", nbytes);
	
	
	//q = pptr->vmemlist;
	//p = q->mnext;
	
	for ( (q = (pptr->vmemlist), p = q->mnext); (p != (struct virtualblock *) NULL); (q=p,p=p->mnext) )
	{
		//kprintf("\n inside VGETMEM() : inside for %d", p->vlen);
		if ( p->vlen == nbytes) 
		{
		//	kprintf("\nInside VGETMEM(): got the block when requested is equal to block %d", p->vlen);
			q->mnext = p->mnext;
			restore(ps);
			return( (WORD *)(p->vaddr) );
		} 
		
		else if ( p->vlen > nbytes ) 
		{
			leftover = (struct virtualblock*)getmem(sizeof(struct virtualblock*));//(struct mblock *)( (unsigned)p + nbytes );
			leftover->vaddr = (p->vaddr + nbytes);
			leftover->vlen = (p->vlen - nbytes);
			leftover->mnext = p->mnext;
			q->mnext = leftover;
			
			//leftover->mlen = p->mlen - nbytes;
		//	kprintf("\nInside VGETMEM(): got the block greater %d, leftover len %d, vmemlist %d, vmemlist->mnext %d, leftover %d, leftover->vaddr %d", p->vlen, leftover->vlen, pptr->vmemlist, pptr->vmemlist->mnext, leftover, leftover->vaddr);
			restore(ps);
			return( (WORD *)(p->vaddr) );
		}
		
		
	}
	
		
	restore(ps);
	return( (WORD *)SYSERR );
}


