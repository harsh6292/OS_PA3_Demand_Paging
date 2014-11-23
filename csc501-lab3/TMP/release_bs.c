#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

SYSCALL release_bs(bsd_t bs_id) {

  /* release the backing store with ID bs_id */
	
	bs_map_t* main_bs;
	int getReturnValue, canReleaseBS;
	int flag;
	STATWORD ps;
	flag = 0;
	main_bs = &bsm_tab[bs_id];
	disable(ps);
	
	//kprintf("\n Inside release bs()..");
	
	
	getReturnValue = check_for_mapping_before_releasing_bs(bs_id, &flag);
	//kprintf("\n Inside release bs()..checked mapping : %d", getReturnValue);
	canReleaseBS = flag;
	//canReleseBS is true(1) then we can release bs else not
	
	if(canReleaseBS == 0)
	{
		//kprintf("\n Inside release bs()..bs can't be free as it has maps");
		restore(ps);
		return(OK);
	}
	
	
	
	
	/*----------------------------------------------------------------------------------
			what if some process doesn't do xm-unmap and directly calls release_bs() ?
			because this process still has its own entry
	-----------------------------------------------------------------------------------*/
	
	
	
	
	
	
	if(canReleaseBS == 1)	//means bs can be released
	{
		main_bs->bs_status = BSM_UNMAPPED;			/* MAPPED or UNMAPPED		*/
		main_bs->bs_pid = -1;							/* process id using this slot   */
		main_bs->bs_vpno = 0;						/* starting virtual page number */
		main_bs->bs_npages = 128;						/* number of pages in the store */
		main_bs->bs_sem = 0;
		main_bs->vuse = 0;
		main_bs->proc_next = NULL;
		//kprintf("\n release_bs(): bs releasd");
		restore(ps);
		return(OK);
	}
	
	
	//kprintf("\n Inside release_bs(): None case");
	restore(ps);
    return OK;

}
