// implement fork from user space

#include <inc/string.h>
#include <inc/lib.h>

// PTE_COW marks copy-on-write page table entries.
// It is one of the bits explicitly allocated to user processes (PTE_AVAIL).
#define PTE_COW		0x800

//
// Custom page fault handler - if faulting page is copy-on-write,
// map in our own private writable copy.
//
static void
pgfault(struct UTrapframe *utf)
{
	void *addr = (void *) utf->utf_fault_va;
	uint32_t err = utf->utf_err;
	int r;

	// Check that the faulting access was (1) a write, and (2) to a
	// copy-on-write page.  If not, panic.
	// Hint:
	//   Use the read-only page table mappings at uvpt
	//   (see <inc/memlayout.h>).

	// LAB 4: Your code here.
	cprintf("pgfault: addr = %08x\n", addr);
	assert(uvpt[(uint32_t)addr/PGSIZE] & PTE_P);
	assert(uvpt[(uint32_t)addr/PGSIZE] & PTE_U);
	if( !( uvpt[(uint32_t)addr/PGSIZE] & PTE_COW) ){
		panic("pafault: not (write to cow) \n");
	}

	// Allocate a new page, map it at a temporary location (PFTEMP),
	// copy the data from the old page to the new page, then move the new
	// page to the old page's address.
	// Hint:
	//   You should make three system calls.

	// LAB 4: Your code here.
	
	sys_page_map(0, ROUNDDOWN(addr, PGSIZE), 0, PFTEMP, PTE_P | PTE_U);

	r = sys_page_alloc(0,ROUNDDOWN(addr, PGSIZE), PTE_P | PTE_U | PTE_W);
	if(r<0) panic("sys_page_alloc failed\n");

	memcpy(ROUNDDOWN(addr, PGSIZE), (void*)PFTEMP, PGSIZE);

	sys_page_unmap(0, (void*)PFTEMP);
	//panic("pgfault not implemented");
}

//
// Map our virtual page pn (address pn*PGSIZE) into the target envid
// at the same virtual address.  If the page is writable or copy-on-write,
// the new mapping must be created copy-on-write, and then our mapping must be
// marked copy-on-write as well.  (Exercise: Why do we need to mark ours
// copy-on-write again if it was already copy-on-write at the beginning of
// this function?)
//
// Returns: 0 on success, < 0 on error.
// It is also OK to panic on error.
//
static int
duppage(envid_t envid, unsigned pn)
{
	int r;
	
	// LAB 4: Your code here.
	//if((uvpt[pn] & PTE_P) == 0) return 0;
	assert(uvpt[pn] & PTE_P);
	if(uvpt[pn] & PTE_SHARE){
		r = sys_page_map(0, (void*)(pn*PGSIZE), envid, (void*)(pn*PGSIZE), PTE_SYSCALL & uvpt[pn]);
		if(r<0) return r;
		return 0;
	}
	if((uvpt[pn] & PTE_W) || (uvpt[pn] & PTE_COW)){
		r = sys_page_map(0, (void*)(pn*PGSIZE), envid, (void*)(pn*PGSIZE), (PTE_SYSCALL & uvpt[pn]) | PTE_COW);	
		if(r<0) return r;
		r = sys_page_map(envid, (void*)(pn*PGSIZE), 0, (void*)(pn*PGSIZE), (PTE_SYSCALL & uvpt[pn]) | PTE_COW);
		if(r<0) return r;

		return 0;
	} 

	r = sys_page_map(0, (void*)(pn*PGSIZE), envid, (void*)(pn*PGSIZE), PTE_SYSCALL & uvpt[pn]);
	return r;
	//panic("duppage not implemented");
	//return 0;
}

//
// User-level fork with copy-on-write.
// Set up our page fault handler appropriately.
// Create a child.
// Copy our address space and page fault handler setup to the child.
// Then mark the child as runnable and return.
//
// Returns: child's envid to the parent, 0 to the child, < 0 on error.
// It is also OK to panic on error.
//
// Hint:
//   Use uvpd, uvpt, and duppage.
//   Remember to fix "thisenv" in the child process.
//   Neither user exception stack should ever be marked copy-on-write,
//   so you must allocate a new page for the child's user exception stack.
//
envid_t
fork(void)   //basicly done, except some bit checks.
{
	// LAB 4: Your code here.
	set_pgfault_handler(pgfault);

	envid_t envid;
	envid = sys_exofork();
	if(envid < 0) panic("fork: envid < 0!\n");

	extern unsigned char end[];
	uint8_t *addr;
	int r;
	cprintf("user end addr is %08x\n", (uint8_t*)end);
	if(envid==0){
		//child
		set_pgfault_handler(pgfault);
		thisenv =  &envs[ENVX(sys_getenvid())];
		return 0;
	}else{
		//father
		for(addr = (void*)UTEXT; addr < end; addr += PGSIZE){
			duppage(envid, (uintptr_t)addr/PGSIZE);
		}

		int r;
		//char *strp = (char *)0xA0000000;
		//int strpgno = (uint32_t)strp/PGSIZE;
		int pn=0, maxpn = UTOP/PGSIZE;
		//cprintf("strpgno = %d\n", strpgno);
		
		for(pn=0;pn<maxpn;pn++){
			if(!(uvpd[PDX(pn*PGSIZE)] & PTE_P))
				continue;
			//cprintf("pn = %d\n", pn);
			if(uvpt[pn] & PTE_SHARE){
				//cprintf("va[%08x] is shared\n", pn*PGSIZE);
		 		if(uvpt[pn] & PTE_P){
					//cprintf("va is valid\n");
					r = sys_page_map(0, (void*)(pn*PGSIZE), envid, (void*)(pn*PGSIZE), PTE_SYSCALL & uvpt[pn]);
				}else{
					//cprintf("va is invalid\n");
				}
			}else{
//				cprintf("va is not shared\n");
			}
		}
		//cprintf("strpgno = %d\n", strpgno);
		// if(uvpt[strpgno]&PTE_P){
		// 	cprintf("va is valid\n");
		// 	if(uvpt[strpgno] & PTE_SHARE){
		// 		cprintf("va is shared\n");
		// 		r = sys_page_map(0, (void*)(strp), envid, (void*)strp, PTE_SYSCALL & uvpt[strpgno]);
		// 		if(r<0) return r;
		// 	}
		// 	else{
		// 		cprintf("va is not shared");
		// 	}
		// }else{
		// 	cprintf("va is invalid\n");
		// }

		void *stack_va  = ROUNDDOWN(&addr, PGSIZE);
		sys_page_alloc(envid, stack_va, PTE_P | PTE_U | PTE_W);
		sys_page_map(envid, stack_va, 0, (void*)UTEMP, PTE_P | PTE_U | PTE_W);
		memcpy((void*)UTEMP, stack_va, PGSIZE);
		

		void *xstack_va = (void*)(UXSTACKTOP - PGSIZE);
		sys_page_alloc(envid, xstack_va, PTE_P | PTE_U | PTE_W);
		sys_page_map(envid, xstack_va, 0, (void*)UTEMP, PTE_P | PTE_U | PTE_W);
		memcpy((void*)UTEMP, xstack_va, PGSIZE);

		sys_page_unmap(0, (void*)UTEMP);
		
		//envs[ENVX(envid)].env_pgfault_upcall = thisenv->env_pgfault_upcall;
		//duppage(envid,(uintptr_t)ROUNDDOWN(&addr, PGSIZE)/PGSIZE);
		
		sys_env_set_status(envid, ENV_RUNNABLE);

		return envid;
	}
	//panic("fork not implemented");
}

// Challenge!
int
sfork(void)
{
	panic("sfork not implemented");
	return -E_INVAL;
}
