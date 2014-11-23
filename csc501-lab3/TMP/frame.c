/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>





struct fifo_list
{
	int frm_no;
	struct fifo_list* frm_next;
	struct fifo_list* frm_prev;
};

struct class_queue
{
	int frm_no;
	struct class_queue* frm_next;
	struct class_queue* frm_prev;
};

struct class_queue* nru_head;
struct fifo_list* fr_list_ptr;


//fr_map_t frm_tab[NFRAMES];

/*-------------------------------------------------------------------------
 * init_frm - initialize frm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_frm()
{
	STATWORD ps; 
	int i;
	
	disable(ps);
	fr_list_ptr = NULL;
	nru_head = NULL;
	for( i=0; i<NFRAMES; i++)
	{
		frm_tab[i].fr_status = FRM_UNMAPPED;			/* MAPPED or UNMAPPED		*/
		frm_tab[i].fr_pid = -1;							/* process id using this frame  */
		frm_tab[i].fr_vpno = -1;						/* corresponding virtual page no*/
		frm_tab[i].fr_refcnt = 0;						/* reference count		*/
		frm_tab[i].fr_type = -1;//FR_TBL;//FRM_FREE			/* FR_DIR, FR_TBL, FR_PAGE	*/
		frm_tab[i].fr_dirty = -1;
		//cookie;										/* private data structure	*/
		frm_tab[i].fr_loadtime = 0;
	}
	
	restore(ps);
	return OK;
}




/*-------------------------------------------------------------------------
 * get_frm - get a free frame according to page replacement policy
 *-------------------------------------------------------------------------
 */
SYSCALL get_frm(int* avail)
{
	//kprintf("To be implemented!\n");
	STATWORD ps;
	int get_frame_num, replace;
	//fr_map_t* free_frame;
	struct fifo_list* new_frm;
	int type, count;
	int fr_type = *avail;
	disable(ps);
  	//kprintf("\n Inside GET_FRAME(int* avail)");
	
	type = 0;
	get_frame_num = get_free_frame(fr_type, &type);
	
	if(get_frame_num == SYSERR)
	{
		//kprintf("\nERROR	ERROR!!!!!");
		restore(ps);
		return(SYSERR);
	}
	
	*avail = get_frame_num; 
	
	replace = type;
	
	if( (fr_type == FR_PAGE) && (replace == 0))
	{
		new_frm = (struct fifo_list*)getmem(sizeof(struct fifo_list));
		if(new_frm == NULL)
		{
			//kprintf("\nError in allocating memory: get_frm()");
			restore(ps);
			return(SYSERR);
		}
		//kprintf("\nin allocating memory: get_frm()");
		new_frm->frm_no = get_frame_num;
		new_frm->frm_next = NULL;
		new_frm->frm_prev = NULL;
	
	
		if(fr_list_ptr == NULL)
		{
			fr_list_ptr = new_frm;
			fr_list_ptr->frm_next = fr_list_ptr;
			fr_list_ptr->frm_prev = fr_list_ptr;
			//kprintf("\n NULL wala Frame no given from get_frm %d and fr_list_ptr %d and new_frm %d, frm no : %d", (get_frame_num+FRAME0), fr_list_ptr, new_frm, new_frm->frm_no);
	
			//kprintf("\n NULL wala  fr_list_ptr->frame_no %d ", fr_list_ptr->frm_no);
			//sleep(10);
		}
	
		else
		{
			new_frm->frm_next = fr_list_ptr;
			new_frm->frm_prev = fr_list_ptr->frm_prev;
		
			fr_list_ptr->frm_prev->frm_next = new_frm;
			fr_list_ptr->frm_prev = new_frm;
	
			//kprintf("\n NOT NULL wala Frame no %d and fr_list_ptr %d and new_frm %d, frm no : %d", fr_list_ptr->frm_next->frm_no, new_frm->frm_next->frm_no, fr_list_ptr->frm_no, new_frm->frm_no);
			//kprintf("\n NOT NULL wala  fr_list_ptr->frame_no %d ", fr_list_ptr->frm_no);
	
		}
	
	}
	
	
	if( (fr_type == FR_PAGE) )
	{
		count = 0;
		while(count<10)
		{
			count++;
		}
	}
		/*new_frm = (struct class_queue*)getmem(sizeof(struct class_queue));
		if(new_frm == NULL)
		{
			//kprintf("\nError in allocating memory: get_frm()");
			restore(ps);
			return(SYSERR);
		}
		//kprintf("\nin allocating memory: get_frm()");
		new_frm->frm_no = get_frame_num;
		new_frm->frm_next = NULL;
		new_frm->frm_prev = NULL;
	
	
		if(nru_head == NULL)
		{
			nru_head = new_frm;
			nru_head->frm_next = nru_head;
			nru_head->frm_prev = nru_head;
			//kprintf("\n NULL wala Frame no given from get_frm %d and fr_list_ptr %d and new_frm %d, frm no : %d", (get_frame_num+FRAME0), fr_list_ptr, new_frm, new_frm->frm_no);
	
			//kprintf("\n NULL wala  fr_list_ptr->frame_no %d ", fr_list_ptr->frm_no);
			//sleep(10);
		}
	
		else
		{
			
			
			
			new_frm->frm_next = nru_head;
			new_frm->frm_prev = nru_head->frm_prev;
		
			nru_head->frm_prev->frm_next = new_frm;
			nru_head->frm_prev = new_frm;
	
			//kprintf("\n NOT NULL wala Frame no %d and fr_list_ptr %d and new_frm %d, frm no : %d", fr_list_ptr->frm_next->frm_no, new_frm->frm_next->frm_no, fr_list_ptr->frm_no, new_frm->frm_no);
			//kprintf("\n NOT NULL wala  fr_list_ptr->frame_no %d ", fr_list_ptr->frm_no);
	
		}
	



	}*/
	
		//kprintf("\n Frame no given from get_frm %d and fr_list_ptr %d ", (get_frame_num+FRAME0), fr_list_ptr);
	
	
	
		
	//what if we don't get free frame ? implement here
  
	//free_frame = &frm_tab[get_frame_num];
  
	
  	restore(ps);
	return OK;
}




/*-------------------------------------------------------------------------
 * allocate a free frame to get_frm() function
 *-------------------------------------------------------------------------
 */
int get_free_frame(int fr_type, int* replace)
{
	int count_frames, getReturnValue;
	fr_map_t *free_frame;
	
	//kprintf("\n Inside GET_FREE_FRAME(), pid %d, pdbr %d", currpid, proctab[currpid].pdbr);
	*replace = 0;
	if( (fr_type == FR_TBL) || (fr_type == FR_DIR) )
	{
		for( count_frames=0; count_frames<(1535-NFRAMES+1); count_frames++)
		{
			if( frm_tab[count_frames].fr_status == FRM_UNMAPPED )
			{
				/*if(fr_type == FR_TBL)
					kprintf("\nAllocating frame no %d to table ", (count_frames+FRAME0));
				else
					kprintf("\nAllocating frame no %d to DIR ", (count_frames+FRAME0));*/
				return(count_frames);
			}
			
		}
	}
	
	else if( fr_type == FR_PAGE )
	{
		for( count_frames=0; count_frames<NFRAMES; count_frames++)		//(1535-NFRAMES+1)
		{
			if( frm_tab[count_frames].fr_status == FRM_UNMAPPED )
			{
				//kprintf("\nAllocating frame no %d to page ", (count_frames+FRAME0));
				return(count_frames);
			}
		}
	}
	
	getReturnValue = replace_the_page(fr_type);
	
	if(getReturnValue == SYSERR)
	{
		return(SYSERR);
	}
	
	else
	{
		*replace = 1;
		//kprintf("\nRETURNING frame %d", getReturnValue);
		return(getReturnValue);
	}
	
	/*if(fr_type == FR_TBL)
		kprintf("\nPage Table Not Allocated. Out of Memory!");
	if(fr_type == FR_DIR)
		kprintf("\nPage Directory Not Allocated. Out of Memory!");*/

	return(SYSERR);
	
	//what if we don't get free frame ? implement here
}







int replace_the_page(int fr_type)
{
	STATWORD ps;
	int curr_frm_vpno, curr_frm_pid, curr_frame_no, bs_id, page_no, getReturnValue, finalFrame, k;
	int store, pg, final_bs, final_page_no;
	long vaddr;
	pd_t* page_dir;
	struct fifo_list* pointer, *head;
	pt_t* page_table;
	struct class_queue* head_nru_q;
	
	
	disable(ps);
	if( grpolicy() == SC )
	{
		
		page_table = (pt_t*)(1031*4096);
		
		
		//check and reset acc bit and return frame no
		pointer = fr_list_ptr;
		head = fr_list_ptr;
		do
		{
			
			curr_frame_no = pointer->frm_no;
		
			curr_frm_pid = frm_tab[curr_frame_no].fr_pid;
			curr_frm_vpno = frm_tab[curr_frame_no].fr_vpno;
			vaddr = (curr_frm_vpno*4096);
			
			
			//kprintf("\nInside do while with frame no %d    ===================here=============================== %d", curr_frame_no, curr_frame_no);
			
			//kprintf("\nbefore bsm_lookup: pid %d, vpno %d , vaddr %d: ",curr_frm_pid, curr_frm_vpno, (vaddr/4096));
		
			getReturnValue = bsm_lookup( curr_frm_pid, vaddr, &store, &pg);
			//kprintf("\nreturn from bsm_lookup: store");// %d, page_no %d: ",(*store), (*pageth));
			if(getReturnValue == SYSERR)
			{
				//kprintf("\nError in page replace: bsm lookup error");
				restore(ps);
				return(SYSERR);
			}
		
			//kprintf("\nafter coming from bsm_lookup");
			bs_id = store;
			page_no = pg;
		
			//kprintf("\nAfter BSM_LOOKUP in replace: pdbr %d, pid %d: ",proctab[currpid].pdbr, currpid);
			//kprintf("\nAfter BSM_LOOKUP in replace: page_dir %d: ",proctab[currpid].pdbr, currpid);
			page_dir = (pd_t*)(NBPG*(proctab[currpid].pdbr));
			//getReturnValue = check_access_bit_set(currpid, page_dir, page_no);
		
		
		
			//kprintf("\n-----------before checking all pt_acc-----------------");
			page_table = (pt_t*)(1031*4096);
			
			getReturnValue = check_acc_bit_for_all_proc_in_one_bs(bs_id, curr_frm_vpno);
			//kprintf("\n-----------after checking all pt_acc----------------%d ---", getReturnValue);
			if( getReturnValue == 0 )
			{
				finalFrame = pointer->frm_no;
				final_bs = bs_id;
				final_page_no = ((((unsigned int)vaddr)>>12)&1023);
				mark_page_table_not_present(bs_id, finalFrame);
				kprintf("\nFrame %d is replaced", (finalFrame+NFRAMES));
				
				fr_list_ptr = pointer->frm_next;
				break;
				
			}
			
			
			pointer = pointer->frm_next;
			//kprintf("\n acc bit set, moving forward..............................fr_ist_ptr %d, .... pointer %d...........pointer next %d", fr_list_ptr->frm_no, pointer->frm_no, pointer->frm_next->frm_no);
	
						
		}while(pointer!= fr_list_ptr);
		
		
		
		if((pointer == fr_list_ptr) && (getReturnValue!=0))
		{
			finalFrame = fr_list_ptr->frm_no;
			mark_page_table_not_present(bs_id, finalFrame);
			kprintf("\nFrame %d is replaced", (finalFrame+NFRAMES));
				
			fr_list_ptr = fr_list_ptr->frm_next;
		}
		
		//kprintf("\n Fr list is now %d and frma no is %d", fr_list_ptr, fr_list_ptr->frm_no);
		/*for( k=0; k<(NBPG/4); k++)
		{
			kprintf("\t again pt_acc bit is %d for entry %d", page_table[k].pt_acc, k);
		}*/
		
		
	 
		//kprintf("\n------------------------------------------------------------------");
		//kprintf("\n---------------FREE FRAME----------------%d---------------------",finalFrame);
		//kprintf("\n------------------------------------------------------------------");
		//free_frm(finalFrame);
		//kprintf("\n before RETURNING bs is %d, page no is : %d, ...%d", finalFrame, final_bs, final_page_no,((FRAME0+finalFrame)*4096));
		//write_bs( ((FRAME0+finalFrame)*4096), final_bs, final_page_no);
	//	kprintf("\n before RETURNING frma no is %d", finalFrame);
		restore(ps);
		return(finalFrame);
	}
	
		
	restore(ps);
	return(SYSERR);
}






int check_access_bit_and_reset_it(int pid, pd_t* page_dir, int page_no)
{
	pt_t* page_table;
	int i, acc_bit;

	acc_bit = 0;
	//kprintf("\n Inside check_acc bit and reset func(), pid %d and page_dir %d", pid, page_dir);
	page_dir = (pd_t*)(NBPG*(proctab[pid].pdbr));
	for ( i=4;i<(NBPG/4); i++)
	{
		if( page_dir[i].pd_pres )
		{
			page_table = (pt_t*)((page_dir[i].pd_base)*NBPG);
			
			if( page_table[page_no].pt_pres )
			{
				if( page_table[page_no].pt_acc )
				{
					page_table[page_no].pt_acc = 0;
					acc_bit = 1;
					//kprintf("\n acc bit reset page tabel %d, page no %d", ((unsigned int)page_table/NBPG), page_no);
				}
			}
		}
	}
	return(acc_bit);
	
}	
	
	
	
int check_acc_bit_for_all_proc_in_one_bs(int bs_id, int frm_vpno)
{	
	
	bs_map_t* main_bs;
	bs_next_proc* proc_list_bs_present;
	//bs_next_proc* proc_list_bs_back;
	pd_t* page_dir;
	int pid, get_acc_bit, final_bit_return, i, address;
	unsigned int offset_in_page_table;//, offset_for_page, offset, off_dir, temp;offset_in_dir, 
	
	get_acc_bit = 0;
	final_bit_return = 0;
	
	//kprintf("\n Inside check_acc for all proc in one bs()");
	//for(i=0; i<(MAX_ID+1); i++)
	{
	main_bs = &bsm_tab[bs_id];
	
	proc_list_bs_present = main_bs->proc_next;
		
	//proc_list_bs_back = NULL;
	
	address = (frm_vpno*4096);
	
	
	
	//offset_in_dir = (((unsigned int)address)>>22);//address->pd_offset;       //gives offset page directory
	
	offset_in_page_table = ((((unsigned int)address)>>12)&1023);//address->pt_offset;  //gives offset in page table 

	//offset_for_page = (((unsigned int)address)&1023);//address->pg_offset; 		//gives offset for page


		//page_dir[offset_in_dir].pd_pres = 1;
		//page_dir[offset_in_dir].pd_write = 1;

	//	page_dir[offset_in_dir].pd_base = ((unsigned int)page_table/NBPG);

	//page_table = (page_dir[offset_in_dir].pd_base)*NBPG;
	
	
	//page_table[offset_in_page_table].pt_pres = 1;
	
//	page_table[offset_in_page_table].pt_write = 1;

	//page_table[offset_in_page_table].pt_base = (FRAME0 + frame_no);
	
	
	
	
	
	while(proc_list_bs_present)	// !=NULL)
	{
		//get PID
		//kprintf("\n Inside bs_id: %d with proc_list bs_is: %d", bs_id, proc_list_bs_present->bs_no);
		pid = proc_list_bs_present->bs_pid;
		//common_offset_page = (curr_frm_vpno - proc_list_bs_present->bs_vpno);
		//Now get PDBR
		page_dir = (pd_t*)(NBPG*(proctab[pid].pdbr));
		
		get_acc_bit = check_access_bit_and_reset_it(pid, page_dir, offset_in_page_table);
		
		if(get_acc_bit == 1)
		{
			//find_pid_reset_old_pt_acc_bits(int bs_id, int curr_frm_vpno, int type)
			final_bit_return = 1;
			
		}
		
		//proc_list_bs_back = proc_list_bs_present;
		proc_list_bs_present = proc_list_bs_present->next_entry;
	}
	}
	return(final_bit_return);
	
}




/*








if(grpolicy == NRU)
	{
		
		head_nru_q = nru_head;
		do
		{
			
			
			
			//getReturnValue = check_acc_bit_for_all_proc_in_one_bs(bs_id, curr_frm_vpno);
			
		
		}



int nru_policy()
{
	int class;


	curr_frame_no = head_nru_q->frm_no;
		
			curr_frm_pid = frm_tab[curr_frame_no].fr_pid;
			curr_frm_vpno = frm_tab[curr_frame_no].fr_vpno;
			vaddr = (curr_frm_vpno*4096);
			
			
			//kprintf("\nInside do while with frame no %d    ===================here=============================== %d", curr_frame_no, curr_frame_no);
			
			//kprintf("\nbefore bsm_lookup: pid %d, vpno %d , vaddr %d: ",curr_frm_pid, curr_frm_vpno, (vaddr/4096));
		
			getReturnValue = bsm_lookup( curr_frm_pid, vaddr, &store, &pg);
			//kprintf("\nreturn from bsm_lookup: store");// %d, page_no %d: ",(*store), (*pageth));
			if(getReturnValue == SYSERR)
			{
				//kprintf("\nError in page replace: bsm lookup error");
				restore(ps);
				return(SYSERR);
			}
		
			//kprintf("\nafter coming from bsm_lookup");
			bs_id = store;
			page_no = pg;
		
			//kprintf("\nAfter BSM_LOOKUP in replace: pdbr %d, pid %d: ",proctab[currpid].pdbr, currpid);
			//kprintf("\nAfter BSM_LOOKUP in replace: page_dir %d: ",proctab[currpid].pdbr, currpid);
			page_dir = (pd_t*)(NBPG*(proctab[currpid].pdbr));
			//getReturnValue = check_access_bit_set(currpid, page_dir, page_no);
		
		
		
			//kprintf("\n-----------before checking all pt_acc-----------------");
			page_table = (pt_t*)(1031*4096);

	class = check_dirty_and_acc_bit(bs_id, rm_vpno);
		
}







int check_dirty_and_acc_bit(int bs_id, int frm_vpno)
{	
	
	bs_map_t* main_bs;
	bs_next_proc* proc_list_bs_present;
	//bs_next_proc* proc_list_bs_back;
	pd_t* page_dir;
	int pid, max, final_bit_return, i, address, class;
	unsigned int offset_in_page_table;//, offset_for_page, offset, off_dir, temp;offset_in_dir, 
	
	max = 0;
	final_bit_return = 0;
	
	//kprintf("\n Inside check_acc for all proc in one bs()");
	//for(i=0; i<(MAX_ID+1); i++)
	{
	main_bs = &bsm_tab[bs_id];
	
	proc_list_bs_present = main_bs->proc_next;
		

	address = (frm_vpno*4096);
	
	
	
	offset_in_page_table = ((((unsigned int)address)>>12)&1023);//address->pt_offset;  //gives offset in page table 


	
	
	
	while(proc_list_bs_present)	// !=NULL)
	{
		//get PID
		//kprintf("\n Inside bs_id: %d with proc_list bs_is: %d", bs_id, proc_list_bs_present->bs_no);
		pid = proc_list_bs_present->bs_pid;
		//common_offset_page = (curr_frm_vpno - proc_list_bs_present->bs_vpno);
		//Now get PDBR
		page_dir = (pd_t*)(NBPG*(proctab[pid].pdbr));
		
		class = get_class_from_all_pt_of_bs(pid, page_dir, offset_in_page_table);
		
		if(class>=max)
		{
			max = class;
			
		}
		
		//proc_list_bs_back = proc_list_bs_present;
		proc_list_bs_present = proc_list_bs_present->next_entry;
	}
	}
	return(max);
	
}








int get_class_from_all_pt_of_bs(int pid, pd_t* page_dir, int page_no)
{
	pt_t* page_table;
	int i, class;

	acc_bit = 0;
	//kprintf("\n Inside check_acc bit and reset func(), pid %d and page_dir %d", pid, page_dir);
	page_dir = (pd_t*)(NBPG*(proctab[pid].pdbr));
	for ( i=4;i<(NBPG/4); i++)
	{
		if( page_dir[i].pd_pres )
		{
			page_table = (pt_t*)((page_dir[i].pd_base)*NBPG);
			
			if( page_table[page_no].pt_pres )
			{
				if( page_table[page_no].pt_acc )
				{
					if( page_table[page_no].pt_dirty )
					{
						class = 3;
					//kprintf("\n acc bit reset page tabel %d, page no %d", ((unsigned int)page_table/NBPG), page_no);
					}
					else
					{
						class = 2;
					}
				}
				else if(!page_table[page_no].pt_acc )
				{
				if( page_table[page_no].pt_dirty )
					{
						class = 1;
					//kprintf("\n acc bit reset page tabel %d, page no %d", ((unsigned int)page_table/NBPG), page_no);
					}
					else
					{
						class = 0;
					}
				}
			}
		}
	}
	return(class);
	
}	
	




*/












/*-----------------------------------------------------------
here --> 	if type == 1 	___means bit is set and reset it 
else -->	if type == 1 	___mark page table not present
------------------------------------------------------------*/

//mark_page_table_not_present(bs_id, curr_frm_vpno);

int mark_page_table_not_present(int bs_id, int frm_no)
{
	bs_map_t* main_bs;
	bs_next_proc* proc_list_bs_present;
	//bs_next_proc* proc_list_bs_back;
	pd_t* page_dir;
	pt_t* page_table;
	int pid, i, address, curr_frm_vpno;
	unsigned int offset_in_page_table;//, offset_for_page, offset, off_dir, temp;offset_in_dir, 
	
	
	
	
	
	curr_frm_vpno = frm_tab[frm_no].fr_vpno;
	address = (curr_frm_vpno*4096);
	offset_in_page_table = ((((unsigned int)address)>>12)&1023);//address->pt_offset;  //gives offset in page table 

	main_bs = &bsm_tab[bs_id];
	
	proc_list_bs_present = main_bs->proc_next;
		
	//proc_list_bs_back = NULL;
	
	
	
	while(proc_list_bs_present)	// !=NULL)
	{
		//get PID
		pid = proc_list_bs_present->bs_pid;
		
		//common_offset_page = (curr_frm_vpno - proc_list_bs_present->bs_vpno);
		
		
		//Now get PDBR
		page_dir = (pd_t*)(NBPG*(proctab[pid].pdbr));
		
		for ( i=4;i<(NBPG/4); i++)
		{
			if( page_dir[i].pd_pres )
			{
				page_table = (pt_t*)((page_dir[i].pd_base)*NBPG);
			
				if( page_table[offset_in_page_table].pt_pres )
				{
					page_table[offset_in_page_table].pt_pres = 0;
					//kprintf("\n page %d not present in %d pt", offset_in_page_table, (unsigned int)page_table/NBPG);
				}
			}
		}
		
		
		//proc_list_bs_back = proc_list_bs_present;
		proc_list_bs_present = proc_list_bs_present->next_entry;
	}
		
		
		
		
}






/*----------------------------------------------------------------------------
 * set values of frame table when allocated a free frame by page table request
 *----------------------------------------------------------------------------
 */
int set_frm_tab_upon_frame_allocation(int pid, int frame_no, int frame_type)
{
	fr_map_t* frame;
	
	frame = &frm_tab[frame_no];
	
	frame->fr_status = FRM_MAPPED;			/* MAPPED or UNMAPPED		*/
	frame->fr_pid = pid;							/* process id using this frame  */
	frame->fr_vpno = 0;//to be done;						/* corresponding virtual page no*/
	frame->fr_refcnt = 1;						/* reference count		*/
	frame->fr_type = frame_type;//FRM_FREE			/* FR_DIR, FR_TBL, FR_PAGE	*/
	frame->fr_dirty = 0;
		//cookie;										/* private data structure	*/
	frame->fr_loadtime = ctr1000;
	
	//kprintf("\n Frame FAULTED: ref count 1 ");
}





/*-------------------------------------------------------------------------
 * free_frm - free a frame 
 *-------------------------------------------------------------------------
 */
SYSCALL free_frm(int i)
{
	STATWORD ps;
  //  kprintf("\nInside freeing frame : %d", i);
    int store, page_no, getReturnValue, vaddr;
	
	if( i<0 && i>=NFRAMES )
	{
		return(SYSERR);
	}
	
	
	fr_map_t* frame;
	
	disable(ps);
	frame = &frm_tab[i];
  	vaddr = ((frame->fr_vpno)*4096);
  	
  	if(frame->fr_type== FR_PAGE)
  	{
  		getReturnValue = bsm_lookup( frame->fr_pid, vaddr, &store, &page_no);
		if(getReturnValue == SYSERR)
		{
			//kprintf("\nError in page replace: bsm lookup error from replaceent");
			restore(ps);
			return(SYSERR);
		}
	}
  	
  	frame->fr_status = FRM_UNMAPPED;
  	//kprintf("\nInside freeing frame: cleaning lists");
  	free_frame_from_fifo_list(i);
  	
  	//kprintf("\nInside freeing frame: AFTER cleaning lists");
    if (frame->fr_type == FR_PAGE) 
    {  
    	//kprintf("\nInside freeing frame: WRITING TO BS");
       // write_bs( ((FRAME0+i)*4096), store, page_no);
       // kprintf("\nInside freeing frame: FREEING FRAME OF BS");
        free_frame_of_proc_in_bs(store);
        
       // kprintf("\nInside freeing frame: CHECKING PAGE TABLE FREE");         
        check_page_table_to_free(i, frm_tab[i].fr_vpno);

    }
   
 
 	//kprintf("\nFRAME Freed");
   			/* MAPPED or UNMAPPED		*/
	frame->fr_pid = -1;							/* process id using this frame  */
	frame->fr_vpno = 0;						/* corresponding virtual page no*/
	frame->fr_refcnt = 0;						/* reference count		*/
	frame->fr_type = -1;//FR_TBL;//FRM_FREE			/* FR_DIR, FR_TBL, FR_PAGE	*/
	frame->fr_dirty = -1;
		//cookie;										/* private data structure	*/
	frame->fr_loadtime = 0;
  
	
	restore(ps);
	return OK;
}






 
int decrement_frame_refcnt(int frame_no) 
{
	
	//kprintf("\n Inside dec ref count checking MAPPED or unmapped fr_no %d, ref count %d and status %d", frame_no, frm_tab[frame_no].fr_refcnt,frm_tab[frame_no].fr_status);
	if(frm_tab[frame_no].fr_status!= FRM_UNMAPPED)
	{
		
		frm_tab[frame_no].fr_refcnt--;
		//kprintf("\n Inside dec ref count MAPPED, count %d", frm_tab[frame_no].fr_refcnt);

		//kprintf("\n Frame ref count decreased... for frame %d", frame_no);
   	 	if (frm_tab[frame_no].fr_refcnt == 0)
   	 	{
    		//kprintf("\n Ref count reached 0...freeing frame");
        	free_frm(frame_no);
    	}
	}
    return OK;
}






//free frame from the fr_list_ptr or fifo list
int free_frame_from_fifo_list(int frame_no)
{
	int i;
 
	struct fifo_list* head;


	//kprintf("\n9999999999999999--------------99999999-----freeing frame fifo list %d", frame_no);
    head = fr_list_ptr;
    do  
    {
        if (head->frm_no == frame_no) 
        {

			//kprintf("\n frame %d is unmapped and now freeing in list", head->frm_no);
        	
        	if( head->frm_next == head)
        	{
        		head = fr_list_ptr = NULL;
        	}
        	
        	else if( head->frm_next->frm_next == head)
        	{
        		head->frm_next = head;
        		head->frm_prev == head;
        		fr_list_ptr = head;
        	}
        	else
        	{
        		head->frm_prev->frm_next = head->frm_next;
				head->frm_next->frm_prev = head->frm_prev;
				//present = back->frm_next;
				if(head->frm_no == fr_list_ptr->frm_no)
				{
					fr_list_ptr = head;
				}
			
            	freemem((struct mblock *)head, sizeof(struct fifo_list));
            	
            }
            
            return(OK);
        } 

		head = head->frm_next;

    }while (head != fr_list_ptr);

	return(SYSERR);
}
 





//free frames of backing stores
int free_frame_of_proc_in_bs(int bs_id)
{
    int i, address, offset_for_page, npages, count_frames;
    
    bs_map_t *main_bstore;
    
	bs_next_proc* proc_list_bs_present;
	//bs_next_proc* bs_back;
 
 	main_bstore = &bsm_tab[bs_id];
		
	proc_list_bs_present = main_bstore->proc_next;
		
	
	 while (proc_list_bs_present)
	 {
	 
	 	for(count_frames = 0; count_frames<NFRAMES; count_frames++)
	 	{
	 		if( (proc_list_bs_present->bs_pid == frm_tab[count_frames].fr_pid) && (proc_list_bs_present->bs_vpno == frm_tab[count_frames].fr_vpno) )
	 		{
	 			for( i=0; i<=proc_list_bs_present->bs_npages; i++)
	 			{
	 				free_frame_from_fifo_list((count_frames+i));
	 				//break;
	 			}
	 		}
	 	
	 	}
	 
	 	proc_list_bs_present = proc_list_bs_present->next_entry;
	 }
	 
	return(OK);
		
	
		
}
 
 
 
 
