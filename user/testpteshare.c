#include <inc/x86.h>
#include <inc/lib.h>
#include <inc/elf.h>
#define VA	((char *) 0xA0000000)
const char *msg = "hello, world\n";
const char *msg2 = "goodbye, world\n";

void childofspawn(void);

void
umain(int argc, char **argv)
{
	int r;

		int fd;
		struct Elf *elf;
		unsigned char elf_buf[512];

		if ((r = open("/testpteshare", O_RDONLY)) < 0)
			panic("open error\n");
		fd = r;

		// Read elf header
		elf = (struct Elf*) elf_buf;
		ssize_t n = readn(fd, elf_buf, sizeof(elf_buf));
		cprintf("elf magic %08x want %08x\n", elf->e_magic, ELF_MAGIC);

		if ( n != sizeof(elf_buf)
			|| elf->e_magic != ELF_MAGIC) {
			panic("wrong magic\n");
			//close(fd);
			//cprintf("elf magic %08x want %08x\n", elf->e_magic, ELF_MAGIC);
			//return -E_NOT_EXEC;
		}
		//close(fd);

	cprintf("envid:%d\n", thisenv->env_id);
	if (argc != 0)
		childofspawn();

	if ((r = sys_page_alloc(0, VA, PTE_P|PTE_W|PTE_U|PTE_SHARE)) < 0)
		panic("sys_page_alloc: %e", r);

	// check fork
	if ((r = fork()) < 0)
		panic("fork: %e", r);
	if (r == 0) {
		strcpy(VA, msg);
		exit();
	}
	wait(r);
	cprintf("fork handles PTE_SHARE %s\n", strcmp(VA, msg) == 0 ? "right" : "wrong");

	// check spawn
	cprintf("before spawn, va is:%s\n", VA);
	if ((r = spawnl("/testpteshare", "testpteshare", "arg", 0)) < 0)
		panic("spawn: %e", r);
	wait(r);
	cprintf("spawn handles PTE_SHARE %s\n", strcmp(VA, msg2) == 0 ? "right" : "wrong");
	cprintf("after spawn, va is:%s\n", VA);
	breakpoint();
}

void
childofspawn(void)
{
	cprintf("i am spawn's child\n");
	strcpy(VA, msg2);
	exit();
}
