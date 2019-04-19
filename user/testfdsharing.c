#include <inc/x86.h>
#include <inc/lib.h>

char buf[512], buf2[512];
char buf3[512];

//test code 
void printfd(int fd, int no){
	int e;
	struct Fd * fdstrc;
	e = fd_lookup(fd, &fdstrc);
	if (e<0) 
		panic("printfd:bad fd\n");


	cprintf("=====Fd Info [%d]=========\n", no);
	cprintf("fd_file id = %d\n", fdstrc->fd_file.id);
	cprintf("fd_offset = %d\n", fdstrc->fd_offset);
	cprintf("fd_mode = %d\n", fdstrc->fd_omode);
	cprintf("=====End of Fd Info==\n");
}

//end of test code 


void
umain(int argc, char **argv)
{
	int fd, r, n, n2;
	int n3;
	if ((fd = open("motd", O_RDONLY)) < 0)
		panic("open motd: %e", fd);
	seek(fd, 0);
	if ((n = readn(fd, buf, sizeof buf)) <= 0)
		panic("readn: %e", n);

//test code 

	printfd(fd,1);
	// seek(fd, 0);
	// cprintf("test seek\n");
	// if ((n3 = readn(fd, buf3, sizeof buf3)) != n)
	// 	panic("read first got %d, sencond got %d", n, n3);
	// if (memcmp(buf, buf3, n) != 0)
	// 	panic("read 1 got different bytes from read in 2");
	// cprintf("seek test succeeded\n");

//end of test code

	if ((r = fork()) < 0)
		panic("fork: %e", r);
	if (r == 0) {
		printfd(fd, 2);
		seek(fd, 0);
		printfd(fd, 3);
		cprintf("going to read in child (might page fault if your sharing is buggy)\n");
		if ((n2 = readn(fd, buf2, sizeof buf2)) != n)
			panic("read in parent got %d, read in child got %d", n, n2);
		//test
		printfd(fd, 4);
		//tset
		if (memcmp(buf, buf2, n) != 0)
			panic("read in parent got different bytes from read in child");
		cprintf("read in child succeeded\n");
		seek(fd, 0);
		//test
		printfd(fd, 5);
		//tset
		close(fd);
		exit();
	}
	wait(r);
	//test
	printfd(fd, 6);
	//tset
	if ((n2 = readn(fd, buf3, sizeof buf3)) != n)
		//panic("read in parent got %d, then got %d", n, n2);
	cprintf("n = %d, buf2:%s\n",n2, buf3);
	if (memcmp(buf, buf3, n) != 0)
		//panic("read in parent got different bytes ");

	//test
	printfd(fd, 7);
	//tset
	cprintf("read in parent succeeded\n");
	close(fd);

	breakpoint();
}
