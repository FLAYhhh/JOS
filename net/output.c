#include "ns.h"

extern union Nsipc nsipcbuf;
union Nsipc nsipc_pkt  __attribute__((aligned(PGSIZE)));

void
output(envid_t ns_envid)
{
	binaryname = "ns_output";

	// LAB 6: Your code here:
	// 	- read a packet from the network server
	//	- send the packet to the device driver


	while(1){
		envid_t from_env = 123;
		int perm;
		int r = ipc_recv(&from_env, &nsipc_pkt, &perm);
		//cprintf("output env:From env:%d,  NSREQ TYPE:%d\n",from_env,  r);
		if( r == NSREQ_OUTPUT){
			// if tx ring is full, try again.
			while((r = sys_tx_pkt((uint8_t *)nsipc_pkt.pkt.jp_data, nsipc_pkt.pkt.jp_len))<0);
		}
	}
}
