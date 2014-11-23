/* xm.c = xmmap xmunmap */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>


/*-------------------------------------------------------------------------
 * xmmap - xmmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmmap(int virtpage, bsd_t source, int npages)
{
	STATWORD ps;
	bs_map_t* main_bs;
	int   getReturnValue;

  /* sanity check ! */


  if ( (virtpage < 4096) || ( source < 0 ) || ( source > MAX_ID) ||(npages < 1) || ( npages >128)){
	//kprintf("xmmap call error: parameter error! \n");
	return SYSERR;
  }


	disable(ps);
	//kprintf("\nInside xmap()..bsd req :%d and npages %d", source, npages);
  //kprintf("xmmap - to be implemented!\n");
  
  main_bs = &bsm_tab[source];
  
    
  getReturnValue = check_bs_used_xmmap(source);
  if( getReturnValue == SYSERR )
  {
  		//kprintf("\nInside xmap()..error in bs");	
  		restore(ps);
  		return(SYSERR);
  }
  
  
  getReturnValue = bsm_map(currpid, virtpage, source, npages);
  
  if( getReturnValue == SYSERR )
  {	
  		//kprintf("\nInside xmap()..error in mapping");	
  		restore(ps);
  		return(SYSERR);
  }
  
  restore(ps);
  return(OK);
  
}



/*-------------------------------------------------------------------------
 * xmunmap - xmunmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmunmap(int virtpage )
{
  	
  	STATWORD ps;
  	
  	int getStoreValue, getPageNo;
  	bs_map_t* main_bs;
  	int getReturnValue, bs_no, page_no;
  	unsigned int temp;
  	
  	/* sanity check ! */
  	if ( (virtpage < 4096) )
  	{ 
	//	kprintf("xmummap call error: virtpage (%d) invalid! \n", virtpage);
		return SYSERR;
  	}

	disable(ps);
	//kprintf("\nInside xmunmap().. %d ", virtpage);

  
  	//getStoreValue = 20;
  
	getReturnValue = bsm_lookup(currpid, (virtpage*4096), &getStoreValue, &getPageNo);
    if (getReturnValue == SYSERR) {
       // kprintf("xmunmap(): could not find mapping!\n");
        restore(ps);
        return SYSERR;
    }
	
	// kprintf("\nxmunmap(): bs returned %d!\n", getStoreValue);

	bs_no = getStoreValue;
	page_no = getPageNo;
    // For all frames that are mapped decrease their refcnts
    bstore_dec_fr_refcnt(bs_no, virtpage);
    	
  	
  	getReturnValue = bsm_unmap(currpid, virtpage, bs_no);
  	if( getReturnValue == SYSERR )
  	{	
  		//kprintf("xmummap: bsm_unmap error");
  		restore(ps);
  		return(SYSERR);
  	}  	
  	
  	
  	//kprintf("\nxmummap: all OK");
  	
  	temp = ((proctab[currpid].pdbr)<<12);
	write_cr3(temp);
  	
  	restore(ps);
  	return(OK);
}

