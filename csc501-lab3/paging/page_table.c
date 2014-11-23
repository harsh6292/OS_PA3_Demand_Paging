#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

pt_t* global_page_table[4];



/*-------------------------------------------------------------------------
 * initialize global page tables which will map 16MB physical memory 
 *-------------------------------------------------------------------------
 */
SYSCALL init_global_pt()
{
	STATWORD ps;
	int num_global_pt, num_entry_pt;
	pt_t* page_table;
	
	disable(ps);
	
	for( num_global_pt=0; num_global_pt<4; num_global_pt++)
	{
		page_table = create_a_page_table(0, FR_TBL); //null process initializes everything
	//	kprintf("\n Gloabl PT %d and address is %d and page no is %d \n", num_global_pt, (unsigned int)page_table, ((unsigned int)page_table/NBPG));
		if( page_table == NULL )
		{
			restore(ps);
			return(SYSERR);
		}
		
		
		for( num_entry_pt=0; num_entry_pt<(NBPG/4); num_entry_pt++)
		{
			page_table[num_entry_pt].pt_pres = 1;		/* page is present?		*/
			page_table[num_entry_pt].pt_write = 1;		/* page is writable?		*/
		    page_table[num_entry_pt].pt_user = 0;		/* is use level protection?	*/
		    page_table[num_entry_pt].pt_pwt	= 0;		/* write through for this page? */
		    page_table[num_entry_pt].pt_pcd	= 0;		/* cache disable for this page? */
		    page_table[num_entry_pt].pt_acc	= 0;		/* page was accessed?		*/
		    page_table[num_entry_pt].pt_dirty = 0;		/* page was written?		*/
		    page_table[num_entry_pt].pt_mbz	= 0;		/* must be zero			*/
		    page_table[num_entry_pt].pt_global =0;		/* should be zero in 586	*/
		    page_table[num_entry_pt].pt_avail = 0;		/* for programmer's use		*/
		    
			//gives base address of each page for each global page table
			page_table[num_entry_pt].pt_base = (num_global_pt*(NBPG/4) + num_entry_pt);		/* location of page?		*/
			
			//if((num_entry_pt%128 == 0) || num_entry_pt==1023)
			//kprintf("\nGPT has entries %d and its base address %d", num_entry_pt, page_table[num_entry_pt].pt_base);

		}
		
		global_page_table[num_global_pt] = page_table;
	}
	
	restore(ps);
	return OK;
}





/*-------------------------------------------------------------------------
 * create a page table on the fly/request 
 *-------------------------------------------------------------------------
 */
pt_t* create_a_page_table(int pid, int fr_type)
{
	int get_frame_num;
	pt_t* page_table;
	
	int frame_no, num_entry_pt;
	
	//kprintf("\n inside create page table");

	get_frame_num = fr_type;
	get_frm(&get_frame_num);		//get free frame and get frame no in pointer get_frame_num
	
	frame_no =get_frame_num;	//extract frame number from address of get_frame_num
	
	
	if( frame_no == SYSERR )
	{
		return(NULL);
	}
	
	set_frm_tab_upon_frame_allocation(pid, frame_no, FR_TBL);	//set frame table values
	
	//gives base address of page table
	page_table = (pt_t*)((FRAME0+frame_no)*NBPG);
	
	//kprintf("\n Frame no %d allocated to page table address %d or page no %d ", (frame_no+FRAME0), (unsigned int)page_table, ((unsigned int)page_table/NBPG));
	
	for( num_entry_pt=0; num_entry_pt<(NBPG/4); num_entry_pt++)
	{
		page_table[num_entry_pt].pt_pres = 0;		/* page is present?		*/
		page_table[num_entry_pt].pt_write = 0;		/* page is writable?		*/
	    page_table[num_entry_pt].pt_user = 0;		/* is use level protection?	*/
	    page_table[num_entry_pt].pt_pwt	= 0;		/* write through for this page? */
	    page_table[num_entry_pt].pt_pcd	= 0;		/* cache disable for this page? */
	    page_table[num_entry_pt].pt_acc	= 0;		/* page was accessed?		*/
	    page_table[num_entry_pt].pt_dirty = 0;		/* page was written?		*/
	    page_table[num_entry_pt].pt_mbz	= 0;		/* must be zero			*/
	    page_table[num_entry_pt].pt_global =0;		/* should be zero in 586	*/
	    page_table[num_entry_pt].pt_avail = 0;		/* for programmer's use		*/
	    page_table[num_entry_pt].pt_base = 0;		/* location of page?		*/

	}
	
	return(page_table);
}





/*-------------------------------------------------------------------------
 * create and initialize page directory at system start up
 *-------------------------------------------------------------------------
 */
pd_t* create_page_dir(int pid)
{
	pd_t* page_dir;
	int get_frame_num;
	STATWORD ps;
	int frame_no, num_entry_pd;
	//kprintf("\n Inside CREATE_PAGE_DIR()");
	get_frame_num = FR_DIR;
	get_frm(&get_frame_num);		//get free frame and get frame no in pointer get_frame_num
	
	disable(ps);
	frame_no =get_frame_num;	//extract frame number from address of get_frame_num
	
	if( frame_no == SYSERR )
	{
		return(NULL);
	}
	
	
	set_frm_tab_upon_frame_allocation(pid, frame_no, FR_DIR);	//set frame table values
	
	//gives base address of page table
	page_dir = (pd_t*)((FRAME0+frame_no)*NBPG);
	
	//kprintf("\n Frame no %d allocated to page DIRectory address %d or page no %d \n", (frame_no+FRAME0), (unsigned int)page_dir, ((unsigned int)page_dir/NBPG));
	
	
	//make entry into page directory for global page tables
	for( num_entry_pd=0; num_entry_pd<4; num_entry_pd++)
	{
		page_dir[num_entry_pd].pd_pres	= 1;		/* page table present?		*/
		page_dir[num_entry_pd].pd_write = 1;		/* page is writable?		*/
		page_dir[num_entry_pd].pd_user	= 0;		/* is use level protection?	*/
		page_dir[num_entry_pd].pd_pwt	= 0;		/* write through cachine for pt?*/
		page_dir[num_entry_pd].pd_pcd	= 0;		/* cache disable for this pt?	*/
		page_dir[num_entry_pd].pd_acc	= 0;		/* page table was accessed?	*/
		page_dir[num_entry_pd].pd_mbz	= 0;		/* must be zero			*/
		page_dir[num_entry_pd].pd_fmb	= 0;		/* four MB pages?		*/
		page_dir[num_entry_pd].pd_global= 0;		/* global (ignored)		*/
		page_dir[num_entry_pd].pd_avail = 1;		/* for programmer's use		*/
		page_dir[num_entry_pd].pd_base	= ((unsigned int)(global_page_table[num_entry_pd])/NBPG);		/* location of page table?	*/
		
		//kprintf("\nGlobal pt %d has page DIRectory address %d", num_entry_pd, page_dir[num_entry_pd].pd_base);
	}
	
	
	
	//for page tables that are not global page table and hence initialized
	for( num_entry_pd=4; num_entry_pd<(NBPG/4); num_entry_pd++)
	{
		page_dir[num_entry_pd].pd_pres	= 0;		/* page table present?		*/
		page_dir[num_entry_pd].pd_write = 0;		/* page is writable?		*/
		page_dir[num_entry_pd].pd_user	= 0;		/* is use level protection?	*/
		page_dir[num_entry_pd].pd_pwt	= 0;		/* write through cachine for pt?*/
		page_dir[num_entry_pd].pd_pcd	= 0;		/* cache disable for this pt?	*/
		page_dir[num_entry_pd].pd_acc	= 0;		/* page table was accessed?	*/
		page_dir[num_entry_pd].pd_mbz	= 0;		/* must be zero			*/
		page_dir[num_entry_pd].pd_fmb	= 0;		/* four MB pages?		*/
		page_dir[num_entry_pd].pd_global= 0;		/* global (ignored)		*/
		page_dir[num_entry_pd].pd_avail = 0;		/* for programmer's use		*/
		page_dir[num_entry_pd].pd_base	= 0;		/* location of page table?	*/
		
		//if((num_entry_pd%128 == 0) || num_entry_pd==1023)
		//	kprintf("\nNormal pt %d has page DIRectory address %d", num_entry_pd, page_dir[num_entry_pd].pd_base);
	}
	
	restore(ps);
	return(page_dir);
}





/*-------------------------------------------------------------------------
 * free a page table when it is not being used
 *-------------------------------------------------------------------------
 */
int free_a_page_table(pt_t* page_table)
{
	int returnValue = OK;
	//page_table = (pt_t*)((FRAME0+frame_no)*NBPG);
	returnValue = free_frm((((unsigned int)page_table/NBPG)-FRAME0));
	
	return(returnValue);
	
}


int  check_page_table_to_free(int frame_no, int vpno)
{
	int proc_count, pd_count, pt_count, page_table_present;
	pt_t* page_table;
	
	page_table_present=0;
	
	for( proc_count=0; proc_count<NPROC; proc_count++)
	{
		if( (page_table =(pt_t*)((return_ptr_to_page_table(proc_count)*NBPG)))!=SYSERR )
					
		for( pt_count=0; pt_count<(NBPG/4); pt_count++)
		{
			if( page_table[pt_count].pt_pres)
			{
				if( pt_count == frame_no )
				{
					page_table[pt_count].pt_pres=0;
				
				}
							
			}
		}
					
		for( pt_count=0; pt_count<(NBPG/4); pt_count++)
		{
			if(page_table[pt_count].pt_pres)
			{
				page_table_present = 1;
			}
		}



		if(page_table_present == 1)
		{
			//kprintf("\nFreeing page table %d",((unsigned int)page_table/4096));
			free_a_page_table(page_table);
		}


	}
}




int return_ptr_to_page_table(int pid)
{

	int proc_count, pd_count;
	pd_t* page_dir;


	for(proc_count=0; proc_count<NPROC; proc_count++)
	{

		if(proctab[proc_count].pstate!=PRFREE)
		{

			page_dir =(( proctab[proc_count].pdbr)*4096);
			for( pd_count=4; pd_count<(NBPG/4); pd_count++)
			{

				if( page_dir[pd_count].pd_pres)
				{
					return(page_dir[pd_count].pd_base);

				}

			}
		}
	}

	return(SYSERR);
}











/*-------------------------------------------------------------------------
 * free page directory along with any page tables associated with it
 *-------------------------------------------------------------------------
 */
int free_page_directory(pd_t* page_dir)
{
	int returnValue_page = OK;
	int returnValue_dir = OK;
	int num_entry;
	
	for( num_entry = 0; num_entry<(NBPG/4); num_entry++)
	{
	
		//page directory contains a page table entry, so free each page table too
		returnValue_page = free_a_page_table((pt_t*)&(page_dir[num_entry]));
	}
	
	returnValue_dir = free_frm((((unsigned int)page_dir/NBPG)-FRAME0));
	
	if( (returnValue_page == SYSERR) || (returnValue_dir == SYSERR) )
	{
		returnValue_page = SYSERR;
	}
	
	
	/*-------------------------------------------------------------------------------------------------
		check for return value here, multiple return value for page, check for multiple return values
	--------------------------------------------------------------------------------------------------*/
	
	return(returnValue_page);
}
