/* vcreate.c - vcreate */
    
#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <paging.h>

/*
static unsigned long esp;
*/

LOCAL	newpid();




/*------------------------------------------------------------------------
 *  create  -  create a process to start running a procedure
 *------------------------------------------------------------------------
 */
SYSCALL vcreate(procaddr,ssize,hsize,priority,name,nargs,args)
	int	*procaddr;		/* procedure address		*/
	int	ssize;			/* stack size in words		*/
	int	hsize;			/* virtual heap size in pages	*/
	int	priority;		/* process priority > 0		*/
	char	*name;			/* name (for debugging)		*/
	int	nargs;			/* number of args that follow	*/
	long	args;			/* arguments (treated like an	*/
					/* array in the code)		*/
{
	//kprintf("To be implemented!\n");
	
	STATWORD ps;
	int pid, getReturnValue, bs_no;
	struct mblock *mptr;
	
	int get_bs_no;
	
	
	bs_map_t* main_bstore;
	struct bs_next_proc* proc_list_bs;
	
	
	struct pentry *pptr;
	disable(ps);
	//kprintf("\nInside VCREATE()");
	if( hsize > 128 || hsize <=0 )
	{
		restore(ps);
		return(SYSERR);
	}
	
	getReturnValue = get_bsm(&get_bs_no);
	if( getReturnValue == SYSERR )
	{
		restore(ps);
		return(SYSERR);
	}
	
	bs_no = get_bs_no;
	
	//kprintf("\nInside VCREATE(): Returned from get_bsm() with bs_no %d and now calling CREATE()", bs_no);
	pid = create(procaddr,ssize,priority,name,nargs,args);
	//kprintf("\nInside VCREATE(): After returning from CREATE() with pid %d", pid); 
	
	disable(ps);
	pptr = &proctab[pid];
	//kprintf("\n Inside VCREATE(): BS No %d allocated to process %d with heap size %d", bs_no, pid, hsize); 
	
	
	main_bstore = &bsm_tab[bs_no];
	main_bstore->bs_status = BSM_MAPPED;			/* MAPPED or UNMAPPED		*/
	main_bstore->bs_pid = pid;							/* process id using this slot   */
	main_bstore->bs_vpno = 4096;						/* starting virtual page number */
	main_bstore->bs_npages = hsize;						/* number of pages in the store */
	main_bstore->bs_sem = -1;
	main_bstore->vuse = 1;
	main_bstore->proc_next = NULL;
	
	
	getReturnValue = bsm_map(pid, 4096, bs_no, hsize);
	disable(ps);
	
	proc_list_bs = (struct bs_next_proc*)main_bstore->proc_next;
	
	//kprintf("\nInside VCREATE(): After calling bsm_map");
	//kprintf("\n Inside vcreate(): BS no:.. %d     bs_pid..:  %d     bs_status..:  %d    bs->proc_next..: %d    bs_vpno..:  %d ", bs_no, main_bstore->bs_pid,  main_bstore->bs_status,  main_bstore->proc_next, main_bstore->bs_vpno);
	//kprintf("\n Inside vcreate(): proc_list_bs->bs_no: %d ..pid: %d..  vpno: %d..	npages: %d ", proc_list_bs->bs_no, proc_list_bs->bs_pid, proc_list_bs->bs_vpno, proc_list_bs->bs_npages);
	//kprintf("\n Inside BSM_MAP(): From BS point: main_bstore->proc_next->bs_no ..:%d   main_bstore->proc_next->bs_pid....:%d     main_bstore->proc_next->bs_vpno..:%d,   main_bstore->proc_next->bs_npages  ..:%d",  ((struct bs_next_proc*)&(*(main_bstore->proc_next))->bs_no), main_bstore->proc_next->bs_pid, main_bstore->proc_next->bs_vpno, main_bstore->proc_next->bs_npages);
	
	
	if( getReturnValue == SYSERR )
	{
		restore(ps);
		return(SYSERR);
	}
	
	
	pptr->vhpnpages = hsize;
	
	pptr->vhpno = 4096;
	
	pptr->store = bs_no;
	
	
	
	//pptr->pdbr
	
		
	//start the heap memory of proc & give it address..
	//(pptr->vmemlist) = (struct mblock*) (4096*NBPG);
	
	
	
	/*if( pptr->vmemlist == (struct virtualblock*)NULL)
	{
		kprintf("\n vmem null");
	}*/
	
	/*pptr->vmemlist.vaddr = (4096*NBPG);
    (pptr->vmemlist).mnext = 0;
    kprintf("\n Inside VCREATE(): pptr->vmemlist is %d and vmemlist->mnext is %d, addr %d", pptr->vmemlist, (pptr->vmemlist).mnext, pptr->vmemlist.vaddr);
    
    */
    
    
	//create_vmemlist(pptr,hsize);
   
   
   
    //kprintf("\n process name %s", pptr->pname);
	
	pptr->vmemlist = (struct virtualblock*)getmem(sizeof(struct virtualblock));
	struct virtualblock* head = getmem(sizeof(struct virtualblock));
	
	if(head == (struct virtualblock*)NULL)
	{
		return(SYSERR);
	}
	if(pptr->vmemlist == (struct virtualblock*)NULL)
	{
		return(SYSERR);
	}
	
	//pptr->store = 4096*4096;
	head->vaddr = 4096*4096;//*NBPG);
	head->vlen = hsize*4096;	
	head->mnext = (struct virtualblock*)NULL;

	//pptr->vmemlist = (struct virtualblock*)getmem(sizeof(struct virtualblock*));
	
	
	pptr->vmemlist->mnext = (struct virtualblock*)head;
	//pptr->vmemlist = head;
	
	//kprintf("\n Inside Create VMEMLIST(): pptr->vmemlist is %d and vmemlist->mnext is %d, addr %d, len %d\n.. heap->vaddr %d", pptr->vmemlist, (pptr->vmemlist)->mnext, ((struct virtualblock *)(pptr->vmemlist)->mnext)->vaddr, (pptr->vmemlist)->mnext->vlen, head->vaddr);
    
   
   //kprintf("\n Inside Create VCREATE(): addr %d, len %d\n.. ", ((struct virtualblock*)(pptr->vmemlist)->mnext)->vaddr, (pptr->vmemlist)->mnext->vlen);
    
   
   
   
   
   
   
   
    //Backing store 0 is at 2048*4096
    //bs has size = 128*4096
    //hence starting address of particular bs is bs_no*(128*4096) + 2048*4096
    
    mptr = ((2048*4096) + ((128*4096)*bs_no));
   // kprintf("\n Inside VCREATE(): Starting address of BS %d is %d", bs_no, mptr);
    mptr->mlen = 4096*hsize;
   // kprintf("\n Inside VCREATE(): Heap size of BS %d is %d", bs_no, mptr->mlen);
    mptr->mnext = 0;
   // kprintf("\n Inside VCREATE(): mptr->mnext is %d and mptr is %d", mptr->mnext, mptr);
    
    restore(ps);
	
	return(pid);
}


/*



int create_vmemlist(struct pentry* pptr, int hsize)
{
	//struct pentry* pptr;
	//pptr= &proctab[pid];
	
	//pptr->vmemlist = (struct virtualblock*)getmem(sizeof(struct virtualblock*));
	kprintf("\n process name %s", pptr->pname);
	
	pptr->vmemlist = (struct virtualblock*)getmem(sizeof(struct virtualblock));
	struct virtualblock* head = getmem(sizeof(struct virtualblock));
	
	if(head == (struct virtualblock*)NULL)
	{
		return(SYSERR);
	}
	if(pptr->vmemlist == (struct virtualblock*)NULL)
	{
		return(SYSERR);
	}
	
	pptr->store = 4096*4096;
	head->vaddr = 4096*4096;//*NBPG);
	head->vlen = hsize*4096;	
	head->mnext = (struct virtualblock*)NULL;

	//pptr->vmemlist = (struct virtualblock*)getmem(sizeof(struct virtualblock*));
	
	
	pptr->vmemlist->mnext = (struct virtualblock*)head;
	//pptr->vmemlist = head;
	
	kprintf("\n Inside Create VMEMLIST(): pptr->vmemlist is %d and vmemlist->mnext is %d, addr %d, len %d\n.. heap->vaddr %d", pptr->vmemlist, (pptr->vmemlist)->mnext, ((struct virtualblock *)(pptr->vmemlist)->mnext)->vaddr, (pptr->vmemlist)->mnext->vlen, head->vaddr);
    
	return(OK);
	
}

*/



/*------------------------------------------------------------------------
 * newpid  --  obtain a new (free) process id
 *------------------------------------------------------------------------
 */
LOCAL	newpid()
{
	int	pid;			/* process id to return		*/
	int	i;

	for (i=0 ; i<NPROC ; i++) {	/* check all NPROC slots	*/
		if ( (pid=nextproc--) <= 0)
			nextproc = NPROC-1;
		if (proctab[pid].pstate == PRFREE)
			return(pid);
	}
	return(SYSERR);
}
