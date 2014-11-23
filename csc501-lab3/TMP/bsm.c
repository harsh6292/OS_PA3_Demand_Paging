/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <mem.h>
#include <proc.h>




//bsm_map_t bsm_tab[MAX_ID+1];

/*-------------------------------------------------------------------------
 * init_bsm- initialize bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_bsm()
{
	STATWORD ps; 
	int i;
	
	//disable(ps);
	for( i=0; i<(MAX_ID+1); i++)
	{
		bsm_tab[i].bs_status = BSM_UNMAPPED;			/* MAPPED or UNMAPPED		*/
		bsm_tab[i].bs_pid = -1;							/* process id using this slot   */
		bsm_tab[i].bs_vpno = -1;						/* starting virtual page number */
		bsm_tab[i].bs_npages = 128;						/* number of pages in the store */
		bsm_tab[i].bs_sem = -1;
		bsm_tab[i].vuse = 0;
		bsm_tab[i].proc_next = NULL;
	}
	
	//restore(ps);
	return OK;
}

/*-------------------------------------------------------------------------
 * get_bsm - get a free entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL get_bsm(int* avail)
{
	int bs_count;
	
	for( bs_count = 0; bs_count<(MAX_ID+1); bs_count++)
	{
		if( bsm_tab[bs_count].bs_status == BSM_UNMAPPED )
		{
			//kprintf("\nInside get BSM func(), BS allocated: %d", bs_count);	//bsm_tab[bs_count].bs_status == BSM_MAPPED;
			*avail = bs_count;
			return(OK);
		}
	}
	
	return(SYSERR);
}




/*int get_bsm_of_hszie(int *avail, int hsize)
{
	int bs_count;
	
	for( bs_count = 0; bs_count<(MAX_ID+1); bs_count++)
	{
		if( bsm_tab[bs_count].bs_status == BSM_UNMAPPED )
		{
			if(bsm_tab[bs_count].bs_npages >= hsize)
			{
				kprintf("\nInside GET_BSM_OF_HSIZE(), BS allocated: %d", bs_count);	//bsm_tab[bs_count].bs_status == BSM_MAPPED;
				*avail = bs_count;
				return(OK);
			}
		}
	}
	
	return(SYSERR);
}
*/


// int get_hsize_bs(int hsize)
// {
	// int bs_count;
	
	// for( bs_count = 0; bs_count<(MAX_ID+1); bs_count++)
	// {
		// if( bsm_tab[bs_count].bs_status == BSM_UNMAPPED )
		// {
			//bsm_tab[bs_count].bs_status == BSM_MAPPED;
			// if
			// *avail = bs_count;
			// return(OK);
		// }
	// }
	
	// return(SYSERR);
// }




//check if 
int check_bs_used_xmmap(int source)
{
	bs_map_t* main_bs;
	
	main_bs = &bsm_tab[source];
	//kprintf("\nInside check_bs..xmap");
	if( main_bs->bs_status == BSM_UNMAPPED)
	{
		//kprintf("\nbs unmapped..xmap..error");
		return(SYSERR);
	}
	
	else if( main_bs->bs_status == BSM_MAPPED)
	{
		//kprintf("\nbs MAPPED..xmap");
		if( main_bs->vuse ==1)
		{
			//kprintf("\nBS being used for vheap..error ..xmmap");
			return(SYSERR);
		}
	}
	//kprintf("\nInside check_bs..All OK..xmap");
	return(OK);
}


//check if it is not being used for vheap
int check_bs_used_getbs(int source)
{
	bs_map_t* main_bs;
	
	main_bs = &bsm_tab[source];
	//kprintf("\nInside check_bs of get_bs()");
	/*if( main_bs->bs_status == BSM_UNMAPPED)
	{
		kprintf("\nbs unmapped");
		return(SYSERR);
	}
	
	else*/
	
	
	if( main_bs->bs_status == BSM_MAPPED)
	{
		//kprintf("\nbs is .. MAPPED..in get_bs");
		if( main_bs->vuse ==1)
		{
			//kprintf("\nBS being used for vheap..in get_bs..error");
			return(SYSERR);
		}
	}
	//kprintf("\nInside check_bs of..getbs()..All OK");
	return(OK);
}




/*-------------------------------------------------------------------------
 * free_bsm - free an entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL free_bsm(int i)
{

	//what if this bsm still has some mappings
	
	
	//else
	bsm_tab[i].bs_status = BSM_UNMAPPED;
	
	
	//bsm_tab[i].bs_status = BSM_UNMAPPED;			/* MAPPED or UNMAPPED		*/
	bsm_tab[i].bs_pid = -1;							/* process id using this slot   */
	bsm_tab[i].bs_vpno = -1;						/* starting virtual page number */
	bsm_tab[i].bs_npages = 128;						/* number of pages in the store */
	bsm_tab[i].bs_sem = -1;
	bsm_tab[i].vuse = 0;
	bsm_tab[i].proc_next = NULL;

  
	/*-------------------------------
		clear contents of memory ?
	---------------------------------*/
	
	return(OK);
}





int check_for_mapping_before_releasing_bs(bsd_t bs_id, int *flag)
{
	bs_map_t* main_bstore;
	
	main_bstore = &bsm_tab[bs_id];
	
	if( main_bstore->proc_next == NULL)
	{
		*flag = 1;	//flag true indicates no mapping, you can release bs
		return(OK);
	}
	
	*flag = 0;
	return(OK);
}








/*-------------------------------------------------------------------------
 * bsm_lookup - lookup bsm_tab and find the corresponding entry
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_lookup(int pid, long vaddr, int* store, int* pageth)
{
	
	STATWORD ps;
	bs_map_t* main_bstore;
	
	int vpno, bs_count;;
	bs_next_proc* proc_list_bs_present;

	
	
	//kprintf("\n Inside BSM Lookup func()");
	
	disable(ps);
	vpno = ((unsigned int)vaddr)/NBPG;

	for(bs_count = 0; bs_count<(MAX_ID+1); bs_count++ )
	{
		main_bstore = &bsm_tab[bs_count];
		
		if( (main_bstore->bs_status == BSM_UNMAPPED) || (main_bstore->proc_next == NULL) ) 
		{
			continue;
		}
		
	
			//kprintf("\n BSM_LOOKUP : Looping for BS %d", bs_count);
			
		proc_list_bs_present = main_bstore->proc_next;

		
		if( (proc_list_bs_present->bs_pid == pid) && (vpno >= proc_list_bs_present->bs_vpno) && (vpno < (proc_list_bs_present->bs_vpno + proc_list_bs_present->bs_npages)) )
		{
	
			*store = proc_list_bs_present->bs_no;
			

			*pageth = vpno - proc_list_bs_present->bs_vpno;//((vaddr - ((proc_list_bs_present->bs_vpno)*4096))/4096);

			restore(ps);
			return(OK);
		}
		
		while(proc_list_bs_present)	
		{
			if( (proc_list_bs_present->bs_pid == pid) && (vpno >= proc_list_bs_present->bs_vpno) && (vpno < (proc_list_bs_present->bs_vpno + proc_list_bs_present->bs_npages)) )
			{
		
				*store = proc_list_bs_present->bs_no;
			
				*pageth = vpno - proc_list_bs_present->bs_vpno;//((vaddr - (proc_list_bs_present->bs_vpno)*4096)/4096);
			
	
				restore(ps);
				return(OK);
			}
			
	
			proc_list_bs_present = proc_list_bs_present->next_entry;
		}
		
	}
	
		//kprintf("\n BSM_LOOKUP: returning error");
	restore(ps);
	return(SYSERR);
}






int bsm_lookup_for_page_fault(int pid, long vaddr, int* store, int* pageth, int* secondary_bs_vpno)
{
	
	STATWORD ps;
	bs_map_t* main_bstore;
	
	int vpno, bs_count;;
	bs_next_proc* proc_list_bs_present;
	//bs_next_proc* proc_list_bs_back;
	

	//kprintf("\n Inside BSM Lookup func() for page fault");
	
	disable(ps);
	vpno = ((unsigned int)vaddr)/NBPG;
	//kprintf("\n Inside BSM Lookup func()");
	for(bs_count = 0; bs_count<(MAX_ID+1); bs_count++ )
	{
		main_bstore = &bsm_tab[bs_count];
		
		if( (main_bstore->bs_status == BSM_UNMAPPED) || (main_bstore->proc_next == NULL) ) 
		{
			continue;
		}
		
		//kprintf("\n BSM_LOOKUP : Looping for BS %d", bs_count);
		proc_list_bs_present = main_bstore->proc_next;
		
		//proc_list_bs_back = NULL;
		
		
		//kprintf("\n BSM_LOOKUP : after LOOPING");// %d, pid %d, .. vpno %d, ..bs_vpno %d, bs_npages %d", bs_count, proc_list_bs_present->bs_pid, vpno, proc_list_bs_present->bs_vpno, proc_list_bs_present->bs_npages);
		//fr_vpno == vaddr, frm pid == bsm_pid
		//*vaddr = j +1024
		//return ok
		
		
		if( (proc_list_bs_present->bs_pid == pid) && (vpno >= proc_list_bs_present->bs_vpno) && (vpno < (proc_list_bs_present->bs_vpno + proc_list_bs_present->bs_npages)) )
		{
			//found first mapping which has to be unmapped;
			//kprintf("\nInside if testing");
			//kprintf("\nInside if in bsm_LOOKUP, %d, %d ", (vpno - proc_list_bs_present->bs_vpno), proc_list_bs_present->bs_no);
			
			*store = proc_list_bs_present->bs_no;
			
			//kprintf("\nafter store testing");
			
			
			*pageth = vpno - proc_list_bs_present->bs_vpno;//((vaddr - ((proc_list_bs_present->bs_vpno)*4096))/4096);
			//kprintf("\n BSM_LOOKUP : Looping for BS %d", bs_count);
			//kprintf("\nafter pageth testing");
		
		
			*secondary_bs_vpno = proc_list_bs_present->bs_vpno;
			//kprintf("\nInside if before return bsm_LOOKUP second %d!...... proc list->bs_no %d\n", *secondary_bs_vpno, proc_list_bs_present->bs_vpno);
			restore(ps);
			return(OK);
		}
		
		while(proc_list_bs_present)	// !=NULL)
		{
			if( (proc_list_bs_present->bs_pid == pid) && (vpno >= proc_list_bs_present->bs_vpno) && (vpno < (proc_list_bs_present->bs_vpno + proc_list_bs_present->bs_npages)) )
			{
				//found first mapping which has to be unmapped;
				
				*store = proc_list_bs_present->bs_no;
			
				*pageth = vpno - proc_list_bs_present->bs_vpno;//((vaddr - (proc_list_bs_present->bs_vpno)*4096)/4096);
			
			//STORE PAGE TH
			 //   kprintf("\n BSM_LOOKUP: (Not first) BS no %d has process %d and page offset is %d ", bs_count, pid, *pageth);
			//
				
				
				
				*secondary_bs_vpno = proc_list_bs_present->bs_vpno;
				
				//kprintf("\n BSM_LOOKUP : Looping for BS (second one) %d..  proc list->bs_no %d\n", *secondary_bs_vpno, proc_list_bs_present->bs_vpno);
				restore(ps);
				return(OK);
			}
			
			//proc_list_bs_back = proc_list_bs_present;
			proc_list_bs_present = proc_list_bs_present->next_entry;
		}
		
	}
	//kprintf("\n BSM_LOOKUP: returning error");
	restore(ps);
	return(SYSERR);
}








int find_if_frame_already_present(int bs_id, int frame_in_bs)
{

	int i;
	int bs_count, vpno;
	bs_map_t* main_bstore;
	
	bs_next_proc* proc_list_bs_present;
	bs_next_proc* proc_list_bs_back;
	
	//kprintf("\n Inside finding frame if present");
	main_bstore = &bsm_tab[bs_id];
	
	proc_list_bs_present = main_bstore->proc_next;
	proc_list_bs_back = NULL;
		
	
	while(proc_list_bs_present)	// !=NULL)
	{
	
		for( i=0; i<NFRAMES; i++)
		{
			if( (frm_tab[i].fr_pid == proc_list_bs_present->bs_pid) && (frm_tab[i].fr_type== FR_PAGE) && ((frm_tab[i].fr_vpno - proc_list_bs_present->bs_vpno)==frame_in_bs) )
			{
					//kprintf("\nFRAME fouND  %d",i);
					return(i);
			}
		
		}	
			
		proc_list_bs_back = proc_list_bs_present;
		proc_list_bs_present = proc_list_bs_present->next_entry;
	}
		
	
	return(-1);
	

}







/*-------------------------------------------------------------------------
 * bsm_map - add an mapping into bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_map(int pid, int vpno, int source, int npages)		//here source is backing store id or bsd_t
{
	bs_map_t* main_bstore;
	bs_next_proc* proc_list_bs;
	STATWORD ps;

	main_bstore = &bsm_tab[source];
 //   bs_t * bsptr;
 //   bs_map_t * bsmptr;

	disable(ps);
   // kprintf("\n Inside BSM Mapping func()");
    proc_list_bs = (bs_next_proc *) getmem(sizeof(bs_next_proc));
    if (!proc_list_bs) 
    {
        //kprintf("\nError in allocating memory in kernel");
		restore(ps);
        return SYSERR;
    }

   proc_list_bs->bs_status = BSM_MAPPED;			/* MAPPED or UNMAPPED		*/
   proc_list_bs->bs_no = source;
   proc_list_bs->bs_pid = pid;				/* process id using this slot   */
   proc_list_bs->bs_vpno = vpno;				/* starting virtual page number */
   proc_list_bs->bs_npages = npages;			/* number of pages in the store */


	proc_list_bs->next_entry = main_bstore->proc_next;

	main_bstore->proc_next = proc_list_bs;
	
	
	//kprintf("\n Inside BSM_MAP(): BS no %d pid %d main_bstore->proc_next %d	 proc_list_bs %d proc_list_bs->bs_status %d", source, pid, main_bstore->proc_next, proc_list_bs, proc_list_bs->bs_status);
	
	//kprintf("\n Inside BSM_MAP(): proc_list_bs->bs_no: %d ..pid: %d..  vpno: %d..	npages: %d ", proc_list_bs->bs_no, proc_list_bs->bs_pid, proc_list_bs->bs_vpno, proc_list_bs->bs_npages);
	
	//kprintf("\n Inside BSM_MAP(): From BS point: main_bstore->proc_next->bs_no ..:%d   main_bstore->proc_next->bs_pid....:%d     main_bstore->proc_next->bs_vpno..:%d,   main_bstore->proc_next->bs_npages  ..:%d", (struct bs_next_proc*)(main_bstore->proc_next)->bs_no, main_bstore->proc_next->bs_pid, main_bstore->proc_next->bs_vpno, main_bstore->proc_next->bs_npages);
	restore(ps);
    return OK;
}







/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int virtpage, int flag)
{
	
	bs_map_t* main_bstore;
	
	
	bs_next_proc* proc_list_bs_present;
	bs_next_proc* proc_list_bs_back;
	
	int bs_count, vpno;
	
	vpno = virtpage;
	//kprintf("\n Inside bsm_unmap()");
	for(bs_count = 0; bs_count<(MAX_ID+1); bs_count++ )
	{
		main_bstore = &bsm_tab[bs_count];
		
		if( (main_bstore->bs_status == BSM_UNMAPPED) || (main_bstore->proc_next == NULL) ) 
		{
			continue;
		}
		
		
		proc_list_bs_present = main_bstore->proc_next;
		
		proc_list_bs_back = NULL;
		
		if( (proc_list_bs_present->bs_pid == pid) && (vpno >= proc_list_bs_present->bs_vpno) && (vpno < (proc_list_bs_present->bs_vpno + proc_list_bs_present->bs_npages)) )
		{
			//found first mapping which has to be unmapped;
			//kprintf("\nBSM_UNMAP(): YES FIRST ENTRY!");
			//kprintf("\nbsm_unmap(): free proc entry %d", proc_list_bs_present);
			//kprintf("\nbsm_unmap(): bfore main bstore next %d",main_bstore->proc_next);
			//kprintf("\nbsm_unmap(): bfore proc next entry %d",proc_list_bs_present->next_entry);
			if( virtpage == proc_list_bs_present->bs_vpno)
			{
				//kprintf("\nBSM_UNMAP(): freeing");
				main_bstore->proc_next = proc_list_bs_present->next_entry;
				freemem((struct mblock*)proc_list_bs_present, sizeof(bs_next_proc));
			}
			
			else
			{
			//	kprintf("\nBSM_UNMAP():decrement pages");
				proc_list_bs_present->bs_npages = (proc_list_bs_present->bs_npages - virtpage);
				
				//proc_list_bs_present->bs_vpno = (proc_list_bs_present->bs_vpno + virtpage);
			}
			
		//	kprintf("\nbsm_unmap():after main bstore next %d",main_bstore->proc_next);
		//	kprintf("\nbsm_unmap(): after proc next entry %d",proc_list_bs_present->next_entry);
			
		//	kprintf("\nbsm_unmap(): BSM NPAGES VALUE AFTER FREEING %d\n",proc_list_bs_present->bs_npages);
			
			return(OK);
		}
		
		while(proc_list_bs_present)	// !=NULL)
		{
			if( (proc_list_bs_present->bs_pid == pid) && (vpno >= proc_list_bs_present->bs_vpno) && (vpno < (proc_list_bs_present->bs_vpno + proc_list_bs_present->bs_npages)) )
			{
				//found mapping which has to be unmapped;
				//kprintf("\nBSM_UNMAP(): NOT FIRST ENTRY!");
				//kprintf("\nbsm_unmap(): free proc entry %d", proc_list_bs_present);
				//kprintf("\nbsm_unmap(): bfore main bstore next %d",main_bstore->proc_next);
			 //	kprintf("\nbsm_unmap(): bfore proc next entry %d",proc_list_bs_present->next_entry);
			
			if( virtpage == proc_list_bs_present->bs_vpno)
			{
				//kprintf("\nBSM_UNMAP(): freeing");
				main_bstore->proc_next = proc_list_bs_present->next_entry;
				freemem((struct mblock*)proc_list_bs_present, sizeof(bs_next_proc));
			}
			
			else
			{
				//kprintf("\nBSM_UNMAP():decrement pages");
				proc_list_bs_present->bs_npages = (proc_list_bs_present->bs_npages - virtpage);
				
				//proc_list_bs_present->bs_vpno = (proc_list_bs_present->bs_vpno + virtpage);
			}
			
			
				//kprintf("\nbsm_unmap():after main bstore next %d",main_bstore->proc_next);
				//kprintf("\nbsm_unmap(): after proc next entry %d",proc_list_bs_present->next_entry);
			 
				return(OK);
			}
			
			proc_list_bs_back = proc_list_bs_present;
			proc_list_bs_present = proc_list_bs_present->next_entry;
		}
		
	}
	
	return(SYSERR);
}





 
int proc_dies_clean_bs(int pid)
{
    int i;
   
	
    bs_map_t *main_bstore;
    
	bs_next_proc* proc_list_bs_present;
	bs_next_proc* proc_list_bs_back;


//	kprintf("\n Proc dying........................");
    for (i=0; i < (MAX_ID+1); i++) 
    {

        
        main_bstore = &bsm_tab[i];

        if (main_bstore->bs_status == BSM_UNMAPPED)
        {
			continue;
		}


        proc_list_bs_back = NULL;
        proc_list_bs_present = main_bstore->proc_next;

        while (proc_list_bs_present) 
		{
			

            if (proc_list_bs_present->bs_pid == pid) 
			{

		//		kprintf("\n Proc dying..........INSIDE.............%d", proc_list_bs_present->bs_pid);
				//bsm_lookup(pid, (proc_list_bs_present->bs_vpno*4096), &getStoreValue, &getPageNo);
				// *pageth = vpno - proc_list_bs_present->bs_vpno;
                bstore_dec_fr_refcnt(proc_list_bs_present->bs_no, proc_list_bs_present->bs_vpno);

                if (proc_list_bs_back == NULL) 
				{
                    main_bstore->proc_next = proc_list_bs_present->next_entry;
                    
                    freemem((struct mblock *)proc_list_bs_present, sizeof(bs_next_proc));
                    proc_list_bs_present = main_bstore->proc_next;
                } 
				else 
				{
                    proc_list_bs_back->next_entry = proc_list_bs_present->next_entry;
                    
                    freemem((struct mblock *)proc_list_bs_present, sizeof(bs_next_proc));
                    
                    proc_list_bs_present = proc_list_bs_back->next_entry;
                }

            }
			else 
			{
                proc_list_bs_back = proc_list_bs_present;
                proc_list_bs_present = proc_list_bs_present->next_entry;

            }

        }


        if (main_bstore->proc_next == NULL)
        {
            free_bsm(i);
		}
		
    }

    return OK;
}






 

int bstore_dec_fr_refcnt(int bs_id, int virtpage) 
{

    int i, address, offset_for_page, npages, count_frames;
    
    bs_map_t *main_bstore;
    
	bs_next_proc* proc_list_bs_present;
 
 	main_bstore = &bsm_tab[bs_id];
		
	proc_list_bs_present = main_bstore->proc_next;
	
	
	address = (virtpage*4096);
	

	offset_for_page = (((unsigned int)address)&1023);
	
	
		
	//kprintf("\nIN BS FRM DECREMENT reference COUNT %d, bs id recvd %d and virtpage %d", proc_list_bs_present->bs_no, bs_id, virtpage);
	 while (proc_list_bs_present)
	 {
	 
	 	for(count_frames = 0; count_frames<NFRAMES; count_frames++)
	 	{
	 		//if(count_frames%100 == 0)
	 		if((proc_list_bs_present->bs_pid == frm_tab[count_frames].fr_pid))
	 		{
	 			//kprintf("\n Inside looping of count frames for dec ref cnt proc->bsid %d, fr_pid %d, bs_vpno %d, fr_vpno %d", proc_list_bs_present->bs_pid ,frm_tab[count_frames].fr_pid, proc_list_bs_present->bs_vpno,frm_tab[count_frames].fr_vpno);
	 			if( ( (frm_tab[count_frames].fr_vpno - proc_list_bs_present->bs_vpno)== offset_for_page) )
	 			{
	 			//	kprintf("\n Frame found for decrementing ref count : %d", count_frames);
	 		
	 				for( i=offset_for_page; i<=proc_list_bs_present->bs_npages; i++)
	 				{
	 					//kprintf("\nDecrementing reference count for frame: %d..............offset %d, npages%d", (count_frames+i), offset_for_page, proc_list_bs_present->bs_npages);
	 					decrement_frame_refcnt((count_frames+i));
	 				//break;
	 				}
	 			}
	 		}
	 	
	 	}
	 
	 	proc_list_bs_present = proc_list_bs_present->next_entry;
	 }
	 
	return(OK);
	
	
}

