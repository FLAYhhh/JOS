#include "ns.h"

extern union Nsipc nsipcbuf;
//union  Nsipc  nsipc_pkt __attribute__((aligned(PGSIZE)));
struct jif_pkt *pkt = (struct jif_pkt *)UTEMP;

void
input(envid_t ns_envid)
{
	binaryname = "ns_input";

	// LAB 6: Your code here:
	// 	- read a packet from the device driver
	// 	- send it to the network server
	// Hint: When you IPC a page to the network server, it will be
	// reading from it for a while, so don't immediately receive
	// another packet in to the same physical page.
	int r;
	if ((r = sys_page_alloc(0, pkt, PTE_P|PTE_U|PTE_W)) < 0)
		panic("sys_page_map: %e", r);
	//cprintf("input.c: pkt:%08x\n", pkt);

	while(1){
		int cnt = 0;
		while((r=sys_rx_pkt(pkt)) < 0 ){
			if(cnt % 100 == 0)
				//cprintf("input try to rx_pkt, cnt:%d\n", cnt);
			cnt +=1;
		}
		//cprintf("input.c: befor send, pkt:%08x\n", pkt);
		ipc_send(ns_envid, NSREQ_INPUT, (void*)pkt, PTE_P |  PTE_U | PTE_W);
		
		int i;
		for(i=0; i<10; i++)
			sys_yield();
	}
}
