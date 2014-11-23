#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

int get_bs(bsd_t bs_id, unsigned int npages) 
{
	STATWORD ps;
	
	bs_map_t* main_bs;
	int getReturnValue;
  	if ( (bs_id < 0 ) || ( bs_id > MAX_ID) ||(npages < 1) || ( npages >128))
  	{
		//kprintf("\nget bs() call error: parameter error! \n");
		return SYSERR;
  	}
	disable(ps);
	//kprintf("\n INside get bs() ..");
	getReturnValue = check_bs_used_getbs(bs_id);
	
	if(getReturnValue== SYSERR)
	{
		//kprintf("\n BS being used for vheap.. inside get_bs()");
		return(SYSERR);
	}
	
	
	main_bs = &bsm_tab[bs_id];
	if (main_bs->bs_status == BSM_MAPPED)
	{
		//kprintf("\n Inside get_bs()..UM..%d", main_bs->bs_npages);
		restore(ps);
        return(main_bs->bs_npages);
    }

    else if (main_bs->bs_status == BSM_UNMAPPED) 
    {
    	
    	//main_bstore = &bsm_tab[bs_no];
		main_bs->bs_status = BSM_MAPPED;			/* MAPPED or UNMAPPED		*/
		main_bs->bs_pid = currpid;							/* process id using this slot   */
		main_bs->bs_vpno = 0;						/* starting virtual page number */
		main_bs->bs_npages = npages;						/* number of pages in the store */
		main_bs->bs_sem = -1;
		main_bs->vuse = 0;
		main_bs->proc_next = NULL;
	
		//kprintf("\nget_bs()..BS MPD..%d", main_bs->bs_npages);
		//bsm_map(int pid, int vpno, int source, int npages)
		//getReturnValue = bsm_map(currpid, 4096, bs_id, npages);
        
        restore(ps);
        return(npages);
    }

    restore(ps);
    return SYSERR;
    
    
}


