// implement fork from user space

#include <inc/string.h>
#include <inc/lib.h>

// PTE_COW marks copy-on-write page table entries.
// It is one of the bits explicitly allocated to user processes (PTE_AVAIL).
#define PTE_COW		0x800

// //
// // Custom page fault handler - if faulting page is copy-on-write,
// // map in our own private writable copy.
// //
// static void
// pgfault(struct UTrapframe *utf)
// {
// 	void *addr = (void *) utf->utf_fault_va;
// 	uint32_t err = utf->utf_err;
// 	int r;

// 	// Check that the faulting access was (1) a write, and (2) to a
// 	// copy-on-write page.  If not, panic.
// 	// Hint:
// 	//   Use the read-only page table mappings at uvpt
// 	//   (see <inc/memlayout.h>).

// 	// LAB 4: Your code here.
// 	//if(!(uvpd[PDX(addr)]&PTE_P))
// 	//		panic("pgfault: user space, invalid addr\n");

// 	pte_t pte = uvpt[(uintptr_t)addr >> PGSHIFT];
// 	//pte_t pte = uvpt[(uintptr_t)addr/PGSIZE];
// 	if (!(err & 2)) {
// 		panic("pgfault was not a write. err: %x", err);
// 	} else if (!(pte & PTE_COW)) {
// 		panic("pgfault is not copy on write");
// 	}

// 	// if(!(err&2))
// 	// 	panic("pgfault: not write, err code %x\n", err);
// 	// if(!(pte&PTE_COW))
// 	// 	panic("pgfault: not to COW\n");	

// 	// Allocate a new page, map it at a temporary location (PFTEMP),
// 	// copy the data from the old page to the new page, then move the new
// 	// page to the old page's address.
// 	// Hint:
// 	//   You should make three system calls.

// 	// LAB 4: Your code here.
	
// 	pte &= ~PTE_COW;
// 	pte |= PTE_W;

// 	sys_page_map(0, ROUNDDOWN(addr, PGSIZE), 0, PFTEMP, pte & PTE_SYSCALL);

// 	r = sys_page_alloc(0,ROUNDDOWN(addr, PGSIZE), pte & PTE_SYSCALL);
// 	if(r<0) 
// 		panic("sys_page_alloc failed\n");

// 	memcpy(ROUNDDOWN(addr, PGSIZE), (void*)PFTEMP, PGSIZE);

// 	sys_page_unmap(0, (void*)PFTEMP);
// 	//panic("pgfault not implemented");
// }
static void
pgfault(struct UTrapframe *utf)
{
	void *addr = (void *) utf->utf_fault_va;
	uint32_t err = utf->utf_err;
	int r;

//	cprintf("[%x] pgfault handler: %x\n", sys_getenvid(),  (uintptr_t)addr);

	// Check that the faulting access was (1) a write, and (2) to a
	// copy-on-write page.  If not, panic.
	// Hint:
	//   Use the read-only page table mappings at uvpt
	//   (see <inc/memlayout.h>).

	// LAB 4: Your code here.
	pte_t pte = uvpt[(uintptr_t)addr >> PGSHIFT];

	if (!(err & 2)) {
		panic("pgfault was not a write. err: %x", err);
	} else if (!(pte & PTE_COW)) {
		cprintf("err:%d,  user addr: %08x\n", err, (uint32_t)addr);
		panic("pgfault, pte is not COW");
	}


	// if (!(err & 2)) {
	// 	panic("pgfault was not a write. err: %x", err);
	// } else if (!(pte & PTE_COW)) {
	// 	panic("pgfault is not copy on write");
	// }


	// Allocate a new page, map it at a temporary location (PFTEMP),
	// copy the data from the old page to the new page, then move the new
	// page to the old page's address.
	// Hint:
	//   You should make three system calls.
	// LAB 4: Your code here.
	pte &= ~PTE_COW;
	pte |= PTE_W;
	int perm = PTE_SYSCALL & pte;

	if ((r = sys_page_alloc(0, PFTEMP, perm)) < 0) {
		panic("sys_page_alloc: %e", r);
	}
	memmove(PFTEMP, ROUNDDOWN(addr, PGSIZE), PGSIZE);
	if ((r = sys_page_map(0,
			      PFTEMP,
			      0,
			      ROUNDDOWN(addr, PGSIZE),
			      perm)) < 0) {
		panic("sys_page_map %e", r);
	}
	if ((r = sys_page_unmap(0, PFTEMP)) < 0) {
		panic("unmap %e", r);
	}
}


// Map our virtual page pn (address pn*PGSIZE) into the target envid
// at the same virtual address.  If the page is writable or copy-on-write,
// the new mapping must be created copy-on-write, and then our mapping must be
// marked copy-on-write as well.  (Exercise: Why do we need to mark ours
// copy-on-write again if it was already copy-on-write at the beginning of
// this function?)

// Returns: 0 on success, < 0 on error.
// It is also OK to panic on error.

// static int
// duppage(envid_t envid, unsigned pn)
// {
// 	// LAB 4: Your code here.
// 	int r;

// 	if(!(uvpd[PDX(pn*PGSIZE)] & PTE_P)
// 		|| !(uvpt[pn] & PTE_P)) 
// 		return 0;
	
// 	pte_t pte = uvpt[pn];

// 	if( !(pte & (PTE_W | PTE_COW))
// 		|| pte & PTE_SHARE){
// 		r = sys_page_map(0, (void*)(pn*PGSIZE), envid, (void*)(pn*PGSIZE), pte & PTE_SYSCALL);	
// 		if(r<0) panic("duppage: sys_page_map, %e", r);
// 	}

// 	pte &= ~PTE_W;
// 	pte |= PTE_COW;

// 	r = sys_page_map(0, (void*)(pn*PGSIZE), envid, (void*)(pn*PGSIZE), pte & PTE_SYSCALL);	
// 	if(r<0) 
// 		panic("duppage: sys_page_map, %e", r);

// 	r = sys_page_map(envid, (void*)(pn*PGSIZE), 0, (void*)(pn*PGSIZE), pte & PTE_SYSCALL);
// 	if(r<0)
// 		panic("duppage: sys_page_map, %e", r);
// 	//panic("duppage not implemented");
// 	return 0;
// }


// Map our virtual page pn (address pn*PGSIZE) into the target envid
// at the same virtual address.  If the page is writable or copy-on-write,
// the new mapping must be created copy-on-write, and then our mapping must be
// marked copy-on-write as well.  (Exercise: Why do we need to mark ours
// copy-on-write again if it was already copy-on-write at the beginning of
// this function?)

// Returns: 0 on success, < 0 on error.
// It is also OK to panic on error.

static int
duppage(envid_t envid, unsigned pn)
{
	// LAB 4: Your code here.
	int r;
	pte_t pte = uvpt[pn];

	// map page directly if not writable or if it's set as shared page
	if ((!(pte & PTE_W) && !(pte & PTE_COW)) || (pte & PTE_SHARE)) {
		if ((r = sys_page_map(thisenv->env_id,
				      (void *)(pn * PGSIZE),
				      envid,
				      (void *)(pn * PGSIZE),
				      pte & PTE_SYSCALL)) < 0) {
			panic("sys_page_map: %e", r);
		}
		return 0;
	}

	// remove write bit and set copy on write
	pte &= ~PTE_W;
	pte |= PTE_COW;

	if ((r = sys_page_map(thisenv->env_id,
			      (void *)(pn * PGSIZE),
			      envid,
			      (void *)(pn * PGSIZE),
			      pte & PTE_SYSCALL)) < 0) {
		panic("sys_page_map: %e", r);
	}

	// remap our page to have copy on write
	if ((r = sys_page_map(thisenv->env_id,
			      (void *)(pn * PGSIZE),
			      thisenv->env_id,
			      (void *)(pn * PGSIZE),
			      pte & PTE_SYSCALL)) < 0) {
		panic("sys_page_map: %e", r);
	}

	return 0;
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
	//cprintf("user end addr is %08x\n", (uint8_t*)end);
	if(envid==0){
		//child
		//set_pgfault_handler(pgfault);
		thisenv =  &envs[ENVX(sys_getenvid())];
		return 0;
	}else{
		//father
		
		for(addr = (uint8_t*)0; addr < (uint8_t *)USTACKTOP; addr += PGSIZE){
			if(!(uvpd[PDX(addr)]&PTE_P)
				|| !(uvpt[(uintptr_t)addr/PGSIZE] & PTE_P))
				continue;
			duppage(envid, (uintptr_t)addr/PGSIZE);
		}

	
		// stack is cow too

		// void *stack_va  = ROUNDDOWN(&addr, PGSIZE);
		// sys_page_alloc(envid, stack_va, PTE_P | PTE_U | PTE_W);
		// sys_page_map(envid, stack_va, 0, (void*)UTEMP, PTE_P | PTE_U | PTE_W);
		// memcpy((void*)UTEMP, stack_va, PGSIZE);
		

		// void *xstack_va = (void*)(UXSTACKTOP - PGSIZE);
		// sys_page_alloc(envid, xstack_va, PTE_P | PTE_U | PTE_W);
		// sys_page_map(envid, xstack_va, 0, (void*)UTEMP, PTE_P | PTE_U | PTE_W);
		// memcpy((void*)UTEMP, xstack_va, PGSIZE);

		// sys_page_unmap(0, (void*)UTEMP);
		
		//envs[ENVX(envid)].env_pgfault_upcall = thisenv->env_pgfault_upcall;
		//duppage(envid,(uintptr_t)ROUNDDOWN(&addr, PGSIZE)/PGSIZE);

		
		// why should we alloc a uxstack when creating a process?/ why not cow?
		// Ans:
		// 	uxstack cannot be cow, because if it's cow, then, when calling page fault
		// 	handle, it will try to write to uxstack, but it's cow, so you encount a pagefault
		// 	again, and you keep going invoke the page fault handle and so on... it will be endless

		sys_page_alloc(envid, (void *)(UXSTACKTOP - PGSIZE), PTE_P | PTE_W | PTE_U);

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
