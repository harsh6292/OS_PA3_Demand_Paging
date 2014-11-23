#include <conf.h>
#include <kernel.h>
#include <stdio.h>
#include <proc.h>
#include <paging.h>


SYSCALL page_fault()
{
	STATWORD ps;
	
	unsigned long get_cr2, pdbr;
	virt_addr_t* address;
	pd_t* page_dir;
	struct pentry * pptr;
	bs_map_t* main_bs;
	bs_next_proc* bs_proc_list_ptr;
	
	int secondary_bs_vpno;
	
	int* dummy;
	int frame_no, num_entry_pt, get_frame_num, curr_frame_vpno, bs_id, page_no, getReturnValue;
	int frame_in_bs;
	pt_t* page_table;
	unsigned int offset_in_dir, offset_in_page_table, offset_for_page, offset, off_dir, temp;
	
	disable(ps);
	
	get_cr2 = read_cr2();
	
	address = ((virt_addr_t*)get_cr2);
	
	pptr = &proctab[currpid];
	
	pdbr = pptr->pdbr;
//	temp>>12;
	page_dir = pdbr*4096;//(pd_t*)temp;
	
	
	curr_frame_vpno = ((unsigned int)address/4096);
	
	
//	kprintf("\n Page fault handler called");



	//kprintf("\n Page fault handler called second  %d, and vpno%d", address, ((unsigned int)address/4096));//, get_cr2, ((unsigned int)address/4096), ((unsigned int)get_cr2/4096));
	

//	set_frm_tab_upon_frame_allocation(currpid, frame_no, FR_PAGE);
	
	offset_in_dir = (((unsigned int)address)>>22);//address->pd_offset;       //gives offset page directory
	
	offset_in_page_table = ((((unsigned int)address)>>12)&1023);//address->pt_offset;  //gives offset in page table 

	offset_for_page = (((unsigned int)address)&1023);//address->pg_offset; 		//gives offset for page





//	unsigned int abc = (unsigned int)offset_in_page_table;

//	offset = (((unsigned int)address)&1023);
//	off_dir = (((unsigned int)address)>>22); 
	//kprintf("\n values of offsets: dir %d, table %d, page, %d", offset_in_dir, offset_in_page_table, offset_for_page);



	//kprintf("\n offset in page table: "); 


	//kprintf("\n page_dir[offset_in_dir]. %d and offset_in_dir %d", page_dir[offset_in_dir], offset_in_dir);
	if(page_dir[offset_in_dir].pd_pres!=1)
	{
	//	kprintf("\n page table not present");
		page_table = create_a_page_table(currpid, FR_TBL);
	//	kprintf("\n page table created %d", ((unsigned int)page_dir/NBPG));
		if(page_table==NULL)
		{
			kill(currpid);
    		restore(ps);
    		return SYSERR;
		}

		page_dir[offset_in_dir].pd_pres = 1;
		page_dir[offset_in_dir].pd_write = 1;

		page_dir[offset_in_dir].pd_base = ((unsigned int)page_table/NBPG);
	}



	page_table = (page_dir[offset_in_dir].pd_base)*NBPG;
	//kprintf("\n out  of creating page table");
	
	
	//*dummy = 1;
	//secondary_bs = (bs_next_proc*)dummy;
	//secondary_bs = (bs_next_proc *) getmem(sizeof(bs_next_proc));
	
	getReturnValue = bsm_lookup_for_page_fault(currpid, address, &bs_id, &page_no, &secondary_bs_vpno);
    if (getReturnValue == SYSERR) {
        //kprintf("\npage fault(): bsm lookup error!\n");
        kill(currpid);
    	restore(ps);
    	return SYSERR;
    }


	
	//bs_proc_list_ptr = (bs_next_proc*)secondary_bs;
	// kprintf("\npage fault(): after bsm lookup second %d..... !\n", secondary_bs_vpno);
	frame_in_bs = (curr_frame_vpno - secondary_bs_vpno);//(bs_proc_list_ptr->bs_vpno));

	//bs_id = bs_proc_list_ptr->bs_no;
	main_bs = &bsm_tab[bs_id];
	
	
	frame_no = find_if_frame_already_present(bs_id, frame_in_bs);
	
	 //kprintf("\npage fault(): find frame present %d..... !\n", frame_no);
	if(frame_no == -1)
	{
		//kprintf("\npage fault(): frame not found..sp creating frame");
		get_frame_num = FR_PAGE;
		get_frm(&get_frame_num);		//get free frame and get frame no in pointer get_frame_num
		frame_no = get_frame_num;	//extract frame number from address of get_frame_num
	
		if( frame_no == SYSERR )
		{
			//kprintf("\n error");
			kill(currpid);
    		restore(ps);
    		return SYSERR;
		}

		set_frm_tab_upon_frame_allocation(currpid, frame_no, FR_PAGE);
		frm_tab[frame_no].fr_vpno = curr_frame_vpno;//((unsigned int)address/4096);
		//kprintf("\n frame values : pid %d, vpno %d, fr_type %d ", frm_tab[frame_no].fr_pid, frm_tab[frame_no].fr_vpno, frm_tab[frame_no].fr_type);
	
	
		//bsm_lookup( currpid, address, k,h);//store, pageth);
		//kprintf("\n Page Fault: bsm lookup store %d, page_no %d", *k, *h);
		
		read_bs((void *)((frame_no + FRAME0)*4096), bs_id, frame_in_bs);
	
	
	
	}
	else
	{
		//kprintf("\n ---------------------------------------------------------------------Increasing reference count");
		frm_tab[frame_no].fr_refcnt++;
	}
	

	
	//bsm_lookup( currpid, address, k,h);//store, pageth);
	//kprintf("\n Page Fault: bsm lookup store %d, page_no %d", *k, *h);
	
	
	
	page_table[offset_in_page_table].pt_pres = 1;
	
	page_table[offset_in_page_table].pt_write = 1;

	page_table[offset_in_page_table].pt_base = (FRAME0 + frame_no);

	//kprintf("\n done pdating page table %d", page_table[offset_in_page_table].pt_base);

	pdbr = (unsigned int)page_dir/NBPG;
	temp = pdbr<<12;
	//kprintf("\n PDBR value : %d , temp %d", pdbr, temp);
	write_cr3(temp);
//	write_cr3(pptr->pdbr);


	restore(ps);
	return(OK);
	
	
}





