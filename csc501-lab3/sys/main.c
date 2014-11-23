/* user.c - main */


/*-----------------------------TEST - 1-------------------
----------------------------------------------------------*/

/*
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <paging.h>

void halt();

/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */
 
 
 /*
 
#define LOOP 50

/* test1.c
 * This test program creates three processes, prA, prB, and prC, at
 * priority 20.  The main process also has priority 20.
 */


/*
int prA, prB, prC;

prch(char c)
{

	int count=0;
	char *addr = (char*)0x50000000; //1G	(4098*4096);//
	bsd_t bs=1;

	int  i= ((unsigned long)addr)>>12;	// the ith page

	kprintf("\n\nHello World, Xinu lives\n\n");



	get_bs(bs, 100);

	if (xmmap(i, bs, 100) == SYSERR) {
	   kprintf("xmmap call failed\n");
	     //return 0;
	 }

	 for (i=0; i<16; i++){
	     *addr = 'A'+i;
	    // kprintf("\n inside for");
	     addr += NBPG;	//increment by one page each time
	 }
	 
	 


struct pentry *pptr;
	pptr= &proctab[currpid];

	while(count++ < LOOP) {
		kprintf("%c", c);
		for (i = 0; i< 1000000; i++);
	}
	kprintf("\n\nCalling Vgetmem() from Proc A:\n");
	unsigned int * vaddr = vgetmem(4096);
	
	*vaddr = 'H';
	kprintf("\n\nAfter returning from Vgetmem(): values of Vaddr %d and *vaddr %c\n", vaddr, *vaddr);
	vaddr+=1000;
	kprintf("\n\nMain() : proctab[pid] vaddr %d and len %d", (proctab[currpid].vmemlist)->mnext->vaddr, (pptr->vmemlist)->mnext->vlen);
	vfreemem(vaddr, 96);
	
	kprintf("\n\nMain() : proctab[pid] vaddr %d", vaddr);
	
	kprintf("\n Inside Create VMEMLIST(): pptr->vmemlist is %d and vmemlist->mnext is %d, addr %d, len %d\n.. ", pptr->vmemlist, (struct virtualblock*)(pptr->vmemlist)->mnext, ((pptr->vmemlist)->mnext)->vaddr, (pptr->vmemlist)->mnext->vlen);
    
	//vfreemem(vaddr, 1000);
	//kprintf("\n\nMain() : proctab[pid].vmemlist->mnext is %d", (proctab[currpid].vmemlist)->mnext);
}



int main()
{
	char *addr = (char*)0x40000000; //1G	(4098*4096);//
	bsd_t bs=1;

	int  i= ((unsigned long)addr)>>12;	// the ith page

	kprintf("\n\nHello World, Xinu lives\n\n");

*/

	//get_bs(bs, 100);

	//if (xmmap(i, bs, 100) == SYSERR) {
	//   kprintf("xmmap call failed\n");
	     //return 0;
	// }

/*	 for (i=0; i<16; i++){
	     *addr = 'A'+i;
	    // kprintf("\n inside for");
	     addr += NBPG;	//increment by one page each time
	 }
	 
	 


	*addr = 'A';
	kprintf("\n VALUE value of addr %c",*(addr));
	*addr = 'B';
 	kprintf("\n value of addr second%c",*addr);*/

      
   
   
  /* 
   addr = (char*)0x40000000; //1G	(4098*4096);//
    for (i=0; i<16; i++)
    {
        kprintf("\nHEllo    0x%08x: %c", addr, *addr);
        addr += NBPG;       //increment by one page each time
    }

	 */
	//kprintf("\n\nTEST1:\n");
	//resume(prA = vcreate(prch,2000,25,20,"proc A",1,'A'));
	//resume(prA = create(prch,2000,20,"proc A",1,'A'));
	//kprintf("\n\nMain() : proctab[pid].vmemlist->mnext is %d", (proctab[prA].vmemlist)->mnext);
	//char *ad = (2047*4096);

	//xmunmap(0x40000000>>12);
	
	
	//basic_test();
    //vheap_test();*/
/*	return 0;
}

*/








/*----------------------------------------------------------------------------------------
								TEST - 2
------------------------------------------------------------------------------------------*/




/* user.c - main */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <paging.h>

void halt();

/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */

void testSC_func()
{
		int PAGE0 = 0x40000;
		int i,j,temp;
		int addrs[1200];
		int cnt = 0; 
		//can go up to  (NFRAMES - 5 frames for null prc - 1pd for main - 1pd + 1pt frames for this proc)
		//frame for pages will be from 1032-2047
		int maxpage = (NFRAMES - (5 + 1 + 1 + 1));

		srpolicy(SC); 
	//kprintf("\nstarting allocating bs() \n");
		for (i=0;i<=maxpage/100;i++){
				if(get_bs(i,100) == SYSERR)
				{
						kprintf("get_bs call failed \n");
						return;
				}
				if (xmmap(PAGE0+i*100, i, 100) == SYSERR) {
						kprintf("xmmap call failed\n");
						return;
				}
				for(j=0;j < 100;j++)
				{  
						//store the virtual addresses
						addrs[cnt++] = (PAGE0+(i*100) + j) << 12;
				}			
		}



	//kprintf("DONE with GET_BS calls \n");
		/* all of these should generate page fault, no page replacement yet
		   acquire all free frames, starting from 1032 to 2047, lower frames are acquired first
		   */
		
		//sleep(10);
	kprintf("\nNow generating page fault");
		
		for(i=0; i < maxpage; i++)
		{  
				*((int *)addrs[i]) = i + 1; 
				
				//kprintf("\n fault %d", i);
		}

		
		
		kprintf("\ntrigger page fault");
		//trigger page replacement, this should clear all access bits of all pages
		//expected output: frame 1032 will be swapped out
		*((int *)addrs[maxpage]) = maxpage + 1; 

		for(i=1; i <= maxpage; i++)
		{

				if (i != 500)  //reset access bits of all pages except this
					*((int *)addrs[i])= i+1;
			
					//kprintf("\nNot setting bit.. %d", i);
				

		}
		
		//sleep(10);
		//kprintf("\nNow swapping");
		//Expected page to be swapped: 1032+500 = 1532
		*((int *)addrs[maxpage+1]) = maxpage + 2; 
		temp = *((int *)addrs[maxpage+1]);


		kprintf("\n going to do xmunmap():");
		sleep(5);




		for (i=0;i<=maxpage/100;i++)
		{
			kprintf("\n backing store present before xmunmap %d, and npages %d", bsm_tab[i].bs_status, bsm_tab[i].bs_npages);
		
		}
		
		for (i=0;i<=maxpage/100;i++)
		{
				xmunmap(PAGE0+(i*100));
				kprintf("\n===================RelEasINg=======");
				release_bs(i);			
		}
		
		
		
		for (i=0;i<=maxpage/100;i++)
		{
			kprintf("\n backing store present %d, and npages %d", bsm_tab[i].bs_status, bsm_tab[i].bs_npages);
		
		}
		
		

}

void testSC(){
		int pid1;


		kprintf("\nTest SC page replacement policy\n");

		pid1 = create(testSC_func, 2000, 20, "testSC_func5", 0 , NULL);
	//kprintf("\nnow resuming test proc\n");
		resume(pid1);
		sleep(10);
		//kill(pid1);

}
/*
void testNRU_func()
{
		STATWORD ps;
		int PAGE0 = 0x40000;
		int i,j,temp=0;
		int addrs[1200];
		int cnt = 0; 

		//can go up to  (NFRAMES - 5 frames for null prc - 1pd for main - 1pd + 1pt frames for this proc)
		int maxpage = (NFRAMES - (5 + 1 + 1 + 1)); //=1016


		srpolicy(NRU); 

		for (i=0;i<=maxpage/100;i++){
				if(get_bs(i,100) == SYSERR)
				{
						kprintf("get_bs call failed \n");
						return;
				}
				if (xmmap(PAGE0+i*100, i, 100) == SYSERR) {
						kprintf("xmmap call failed\n");
						return;
				}
				for(j=0;j < 100;j++)
				{  
						//store the virtual addresses
						addrs[cnt++] = (PAGE0+(i*100) + j) << 12;
				}			
		}
*/

		/* all of these should generate page fault, no page replacement yet
		   acquire all free frames, starting from 1032 to 2047, lower frames are acquired first
		   */
/*		for(i=0; i < maxpage; i++)
		{  
				*((int *)addrs[i]) = i + 1;  //bring all pages in, only referece bits are set

		}

		sleep(3); //after sleep, all reference bits should be cleared

		disable(ps); //reduce the possibility of trigger reference bit clearing routine while testing

		for(i=0; i < maxpage/2; i++)
		{  
				*((int *)addrs[i]) = i + 1; //set both ref bits and dirty bits for these pages			
		}		


		enable(ps);  //to allow page fault
		// trigger page replace ment, expected output: frame 1032+maxpage/2=1540 will be swapped out
		// this test might have a different result (with very very low possibility) if bit clearing routine is called before executing the following instruction		
		*((int *)addrs[maxpage]) = maxpage + 1; 

		for (i=0;i<=maxpage/100;i++){
				xmunmap(PAGE0+(i*100));
				release_bs(i);			
		}

*/
//}
/*
void testNRU(){
		int pid1;

		kprintf("\nTest NRU page replacement policy\n");

		pid1 = create(testNRU_func, 2000, 20, "testNRU_func", 0, NULL);

		resume(pid1);
		sleep(10);
		kill(pid1);


}*/


int main() {
	int i;
		testSC();
		//sleep(100);
		for(i=0;i<1024;i++)
		{
			kprintf("\n FRAMES present %d and fr is %d", frm_tab[i].fr_status, i);
		}
		//testNRU();
		return 0;
}
