// Ping-pong a counter between two processes.
// Only need to start one of these -- splits into two, crudely.

#include <inc/string.h>
#include <inc/lib.h>

envid_t dumbfork(void);

void
umain(int argc, char **argv)
{
	envid_t who;
	int i;

	// fork a child process
	who = dumbfork();
	//cprintf("who = %d, i = %d \n", who, i);
	// print a message and yield to the other a few times
	for (i = 0; i < (who ? 10 : 20); i++) {
		cprintf("%d: I am the %s!\n", i, who ? "parent" : "child");
		sys_yield();
	}
	//cprintf("who = %d, i = %d \n", who, i);
}


//在这里浪费了很多时间,所以想总结一下.
//刚开始为了简单验证几个系统调用的正确性,我把原版的duppage注释了,实现了一个简单的版本.
//理论上来说仅仅是映射到相同的物理地址,fork的功能基本就完成了.
//实际上只有代码段和数据段是静态的,所以上面的想法没问题.但是如果将栈也在两个进程间共享,那么当父进程从yield()返回时,
//   父进程的栈实际上在函数umain中,而子进程应该在dumbfork中.所以,子进程执行完当前函数的代码后原本
//   应该从dumbfork返回到umain,但是因为栈的共享却从umain返回到了libmain,从而执行了进程的销毁.
//   接着执行子进程destory系统调用的cpu会进程yield(),发现了runnable的父进程.然而父进程的栈中的frame被
//   子进程带到了奇怪的地方,准确的来讲,应该是进行系统调用前最后一个函数,即用户态的syscall.父进程继续执行umain
//   当中的代码,因为需要引用局部变量i, 而此时的栈已经发生了变化,所以i的值不可预测.不过i大概率大于10
//   因此,父进程也返回,不过是从用户态的syscall返回.  syscall -> sys_env_destroy -> exit -> libmain.
//   最终返回到 lib/entry.S 执行
//    1:	jmp 1b   永远执行这一条指令.
//大概花了十个小时卡在这个bug上..真爽.

void
duppage(envid_t dstenv, void *addr)
{
	int r;

	// This is NOT what you should do in your fork.
	if ((r = sys_page_alloc(dstenv, addr, PTE_P|PTE_U|PTE_W)) < 0)
		panic("sys_page_alloc: %e", r);
	if ((r = sys_page_map(dstenv, addr, 0, UTEMP, PTE_P|PTE_U|PTE_W)) < 0)
	//if ((r = sys_page_map(0, addr, dstenv, addr, PTE_P|PTE_U|PTE_W)) < 0)
		panic("sys_page_map: %e", r);
	memmove(UTEMP, addr, PGSIZE);

	if ((r = sys_page_unmap(0, UTEMP)) < 0)
		panic("sys_page_unmap: %e", r);
}

envid_t
dumbfork(void)
{
	envid_t envid;
	uint8_t *addr;
	int r;
	extern unsigned char end[];

	// Allocate a new child environment.
	// The kernel will initialize it with a copy of our register state,
	// so that the child will appear to have called sys_exofork() too -
	// except that in the child, this "fake" call to sys_exofork()
	// will return 0 instead of the envid of the child.
	envid = sys_exofork();
	cprintf("sys_exofork() done...\n");
	if (envid < 0)
		panic("sys_exofork: %e", envid);
	if (envid == 0) {
		// We're the child.
		// The copied value of the global variable 'thisenv'
		// is no longer valid (it refers to the parent!).
		// Fix it and return 0.
		thisenv = &envs[ENVX(sys_getenvid())];
		//cprintf("I am child, i will return. envid = %d\n", envid);
		
		return 0;
	}

	// We're the parent.
	// Eagerly copy our entire address space into the child.
	// This is NOT what you should do in your fork implementation.
	for (addr = (uint8_t*) UTEXT; addr < end; addr += PGSIZE)
		duppage(envid, addr);

	// Also copy the stack we are currently running on.
	duppage(envid, ROUNDDOWN(&addr, PGSIZE));

	// Start the child environment running
	if ((r = sys_env_set_status(envid, ENV_RUNNABLE)) < 0)
		panic("sys_env_set_status: %e", r);

	return envid;
}

