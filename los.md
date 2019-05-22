
<!-- TOC -->

- [导论](#%E5%AF%BC%E8%AE%BA)
- [Boot](#boot)
    - [导言](#%E5%AF%BC%E8%A8%80)
    - [PC Bootstrap](#pc-bootstrap)
        - [pc物理地址布局](#pc%E7%89%A9%E7%90%86%E5%9C%B0%E5%9D%80%E5%B8%83%E5%B1%80)
        - [BIOS](#bios)
    - [Bootloader](#bootloader)
        - [实模式转换至保护模式](#%E5%AE%9E%E6%A8%A1%E5%BC%8F%E8%BD%AC%E6%8D%A2%E8%87%B3%E4%BF%9D%E6%8A%A4%E6%A8%A1%E5%BC%8F)
        - [加载内核](#%E5%8A%A0%E8%BD%BD%E5%86%85%E6%A0%B8)
    - [内核](#%E5%86%85%E6%A0%B8)
    - [控制台IO](#%E6%8E%A7%E5%88%B6%E5%8F%B0io)
    - [Stack](#stack)
- [内存管理](#%E5%86%85%E5%AD%98%E7%AE%A1%E7%90%86)
    - [导言](#%E5%AF%BC%E8%A8%80)
    - [物理内存管理](#%E7%89%A9%E7%90%86%E5%86%85%E5%AD%98%E7%AE%A1%E7%90%86)
        - [内存探测](#%E5%86%85%E5%AD%98%E6%8E%A2%E6%B5%8B)
        - [内存分配](#%E5%86%85%E5%AD%98%E5%88%86%E9%85%8D)
    - [虚拟内存](#%E8%99%9A%E6%8B%9F%E5%86%85%E5%AD%98)
        - [虚拟地址(逻辑地址),线性地址,和物理地址](#%E8%99%9A%E6%8B%9F%E5%9C%B0%E5%9D%80%E9%80%BB%E8%BE%91%E5%9C%B0%E5%9D%80%E7%BA%BF%E6%80%A7%E5%9C%B0%E5%9D%80%E5%92%8C%E7%89%A9%E7%90%86%E5%9C%B0%E5%9D%80)
        - [内核地址空间的初始化](#%E5%86%85%E6%A0%B8%E5%9C%B0%E5%9D%80%E7%A9%BA%E9%97%B4%E7%9A%84%E5%88%9D%E5%A7%8B%E5%8C%96)
        - [操控页目录和页表的函数](#%E6%93%8D%E6%8E%A7%E9%A1%B5%E7%9B%AE%E5%BD%95%E5%92%8C%E9%A1%B5%E8%A1%A8%E7%9A%84%E5%87%BD%E6%95%B0)
- [用户态进程](#%E7%94%A8%E6%88%B7%E6%80%81%E8%BF%9B%E7%A8%8B)
    - [引入进程概念](#%E5%BC%95%E5%85%A5%E8%BF%9B%E7%A8%8B%E6%A6%82%E5%BF%B5)
    - [创建进程](#%E5%88%9B%E5%BB%BA%E8%BF%9B%E7%A8%8B)
    - [从内核态到用户态](#%E4%BB%8E%E5%86%85%E6%A0%B8%E6%80%81%E5%88%B0%E7%94%A8%E6%88%B7%E6%80%81)
    - [从用户态到内核态](#%E4%BB%8E%E7%94%A8%E6%88%B7%E6%80%81%E5%88%B0%E5%86%85%E6%A0%B8%E6%80%81)
    - [用户态进程的入口点](#%E7%94%A8%E6%88%B7%E6%80%81%E8%BF%9B%E7%A8%8B%E7%9A%84%E5%85%A5%E5%8F%A3%E7%82%B9)
    - [从用户态创建进程](#%E4%BB%8E%E7%94%A8%E6%88%B7%E6%80%81%E5%88%9B%E5%BB%BA%E8%BF%9B%E7%A8%8B)
        - [使用copy on write 机制的fork()](#%E4%BD%BF%E7%94%A8copy-on-write-%E6%9C%BA%E5%88%B6%E7%9A%84fork)
        - [用户态的page fault处理函数](#%E7%94%A8%E6%88%B7%E6%80%81%E7%9A%84page-fault%E5%A4%84%E7%90%86%E5%87%BD%E6%95%B0)
    - [进程间通信](#%E8%BF%9B%E7%A8%8B%E9%97%B4%E9%80%9A%E4%BF%A1)
    - [抢占式多任务](#%E6%8A%A2%E5%8D%A0%E5%BC%8F%E5%A4%9A%E4%BB%BB%E5%8A%A1)
    - [调度](#%E8%B0%83%E5%BA%A6)
- [文件系统](#%E6%96%87%E4%BB%B6%E7%B3%BB%E7%BB%9F)
- [网卡驱动](#%E7%BD%91%E5%8D%A1%E9%A9%B1%E5%8A%A8)

<!-- /TOC -->

# 导论

操作系统名称:LOS

操作系统的运行环境是qemu模拟器,虽然也能运行在真实的x86上,但是选择qemu的原因是便于开发和调试.因为调试的原理是通过异常来陷入内核态,从而对程序的行为和状态进行追踪.对于开发操作系统来说,只能通过模拟器来进行调试.


# Boot

## 导言
对于操作系统这样的大型软件系统而言,仅仅有基本的设计构思是不充分的,一切都必须落实到细节.这要求了,除了设计部分,还需要通过实现来验证它的正确性.操作系统内部有相当多的模块和抽象层,各个模块之间的耦合度和复杂度也相当高,甚至必须依赖硬件的功能.这样的特性导致了系统的实现必须是自底向上的.对于操作系统来说,一切都从boot开始.

## PC Bootstrap

### pc物理地址布局
``` 
+------------------+  <- 0xFFFFFFFF (4GB)  
|      32-bit      |  
|  memory mapped   |  
|     devices      |  
|                  |  
/\/\/\/\/\/\/\/\/\/\  

/\/\/\/\/\/\/\/\/\/\  
|                  |  
|      Unused      |  
|                  |  
+------------------+  <- depends on amount of RAM  
|                  |  
|                  |  
| Extended Memory  |  
|                  |  
|                  |  
+------------------+  <- 0x00100000 (1MB)  
|     BIOS ROM     |  
+------------------+  <- 0x000F0000 (960KB)  
|  16-bit devices, |  
|  expansion ROMs  |  
+------------------+  <- 0x000C0000 (768KB)  
|   VGA Display    |  
+------------------+  <- 0x000A0000 (640KB)  
|                  |  
|    Low Memory    |  
|                  |  
+------------------+  <- 0x00000000  
```

由于intel的每一代处理器都要向后兼容(backward compatibility),x86平台的内存布局显得很不规整.早期的Intel 8088处理器的地址总线是20位,所以能够寻址的物理地址空间大小为1MB.参照上图,将地址用16进制表示也就是0x00000000到0x000FFFFF.标记为"Low Memory"的640KB用于RAM,剩余的地址留给硬件作为特殊用途(如VGA缓存),或是用于非易失性存储来存放固件(firmware).在这些预留的地址中,最重要的是0x000F0000(960KB)到0x00100000(1MB)这段用于存放BIOS的地址.在早期的pc中,BIOS的载体是不可更新的ROM,现在一般都是用闪存来存储BIOS,为BIOS提供了很大了灵活性.

从80386开始,地址总线变为32位,但仍然保留了8086的物理内存布局,从而形成了上图所示的整体的内存布局.从软件的角度,可用的RAM分为两个部分,0到640KB的低地址和1MB到4GB的地址.某些情况,1MB到4GB这段地址也并不能全部用于RAM.实际上接近4GB的地址被BIOS预留给PCI设备用作内存映射IO.

### BIOS

BIOS的基本功能是对系统进行基本的初始化,如激活video card和检查已安装的物理内存等.完成初始化的功能之后,BIOS会将bootloader从其他存储介质(硬盘等)加载到内存当中,并且将控制权转交给bootloader.


对于Intel 8088处理器,PC启动时处于实模式,且CS=0xf000, IP=0xfff0.因此cpu取得的第一条指令位于CS:IP处.根据实模式的地址计算方式,CS:IP = CS<<4 + IP,即0xffff0.参照物理内存的布局,这个地址属于BIOS.
BIOS被称为固件,实际上固件和软件唯一的区别在于,固件存储在非易失性存储介质上.因此BIOS实际上和其他的程序没什么本质的区别.PC启动时的执行的第一条指令也就是BIOS的第一条指令.

```
[f000:fff0] 0xffff0:	ljmp   $0xf000,$0xe05b
```

这是pc启动后执行的第一条指令.


## Bootloader
磁盘或者软盘的最小单元是sector,一般来说一个sector的大小是512Byte,从与硬件直接交互的角度来看,每次读写磁盘也只能读写一个或多个sector.当某个磁盘的第一个sector存放了bootloader的代码时,这个磁盘就是可引导的,这个sector也被称为boot sector. 上面提到过BIOS所做的最后一个工作是将boot sector里的内容读取到内存0x7c00-0x7dff处.从上图来看,这个地址处于低地址处,更加直观一点是64KB(0xffff)以下.加载完之后将控制权转交给bootloader,也就是使用jmp指令跳转到CS:IP=0000:7c00的地方.

从bootloader的角度来考虑,需要保证整个bootloader(代码+数据)的大小在512字节以内.在将bootloader存进boot sector时也需要保证sector内0字节偏移处必须存放着第一条指令.

LOS的bootloader由两个文件组成:boot/boot.S和boot/main.c.

bootloader需要完成两个主要的功能
1. 首先,bootloader需要将cpu从实模式转换成保护模式.Intel向后兼容的特性导致了,cpu在启动时是处于实模式,寻址的方式也是通过8088的方式.因此能够寻址的范围是0-1MB.开启保护模式后,段寄存器的意义发生改变,段内的偏移也变成了32位,所以能够完成4GB大小的寻址.
2. 开启A20总线.由于历史原因,x86平台的地址总线的第20位总是关闭的,也就是说第20位总是为0.如果跳过这一步,那么即使进入到保护模式,我们能使用的地址大小也只有2GB,因为我们总是无法访问第20位为1的地址.开启方式是通过键盘控制器.同样也是出于历史原因,键盘控制器除了控制键盘以外还有控制A20地址线的作用.
3. 然后bootloader将内核从磁盘加载到内存中,并将控制权转交给内核.


### 实模式转换至保护模式

### 加载内核

为了完成bootloader的第二个功能,会面临很多的问题.比如
1. 内核镜像的格式究竟是怎样的?
2. 内核镜像中的数据和代码需要被加载到内存中的什么位置?
3. 内核的入口点在哪(内核的第一条指令)?

实际上内核和普通的程序没有本质的区别,都是由很多source编译为很多object,再将很多object链接成最终的可执行文件.最终的可执行文件可以选择多种格式,对于类UNIX的系统来说,一般是EFL格式(excutable and linkable format).在ELF格式中记录了数据和代码需要被加载到的内存位置,以及内核的入口点.因此以上面临的所有问题都能够解决.

ELF可执行文件中包含固定长度的ELF header,可变长度的program header, 以及program section.在处理ELF格式的文件时,能够在固定的地方得到ELF header的信息,进而找到program header的信息,进而找到program section的信息.其中ELF header里面记录了加载的信息以及入口点.program section里面记录了具体的代码和数据.关于ELF header的具体描述参见inc/elf.h.在编写bootloader时需要考虑的program section是:
- .text: 二进制编码格式的程序指令
- .rodata: 只读的数据.
- .data: 程序的数据区,例如已经初始化的全局变量,或者已经初始化的static变量.
- .bss: 未初始化的全局/static变量.实际上.bss在文件中占用的空间为0,只有在加载时才会在内存中分配空间.具体在内存中分配多少空间都记录在相应的program header里.
- 其他section,如包含debug信息的section等.

通过ELF格式,程序可以轻松地被加载到内存中.program section中的LMA(load address)和section的文件内偏移量描述了加载过程的映射关系.而VMA(link address)和LMA描述了运行时的映射关系,这点在内存映射时再做讨论.

LOS的LMA = 1MB,也就是说内核会被加载到extend memmory的开始处.完成加载之后,bootloader将控制权转交给ELF头中记录的kernel入口地址.在LOS中,e_entry = 0x10000c.

## 内核
从内核接管控制权开始,包括之后的用户态进程,都会启用内存映射,而不是使用实际的物理地址.ELF可执行文件中描述的VMA就是程序的指令和数据认为自己所处于的地址,并且CPU在进行取址时使用的也是VMA的地址.

kernel的LMA是0x100000(1MB),VMA是0xf0100000(4G-255MB).为了完成这样的映射需要借助硬件-MMU(memmory management unit).mmu是介于cpu和内存之间的一个硬件,地址总线会经过mmu,cpu使用的虚拟地址通过mmu转换成物理地址.mmu使用一个页目录来完成映射.页目录是一个内核数据结构,跟随kernel image一同被载入内存.内核需要将页目录的基地址(需要传递实际的物理地址)写进CR3寄存器中,mmu通过cr3寄存器寻找页目录的位置,进而通过页目录内的映射规则来完成地址的转换.

在LOS的kernel设计中,0xf000000-0xffffffff的虚拟地址会被映射到0x00000000-0x0fffffff(256M).可以看出其中0xf0000000之后的1MB地址是640k的低地址以及BIOS使用的空间.内核镜像从0xf0100000开始.

以上的映射规则需要在内核运行的早期被记录在页目录和页表中.由于内核的正确运行需要页目录和页表,所以在内核运行页目录这个结构必须是已经存在于内核中的(静态结构).但是要完全映射出256MB地址的地址映射规则需要填写大约16448个entry.因为每个页表包含1024个4k大小的页的地址,因此每个页表能够描述4MB的映射关系.为了描述256MB的地址需要64个页表,也就是说在页目录中要使用64项.一共要填写的条目包括64个页目录条目+64*每个页表中1024个条目 = 16448个条目.因此要静态构建出这么多页表和页目录是十分复杂的.所以LOS采用的设计是,首先只映射4MB大小的空间(一个页表的大小),这是合理的,因为LOS的内核大小大约为1.8M,所以映射4MB足够让内核正确的运行.然后在内核运行的早期进一步完善内存初始化,完成剩下的内核空间的地址映射.

## 控制台IO
当我们开始创造一个内核时,可能最想做的事就是首先让内核输出一些东西以证明它确实正在运行,并且和预期的行为一致.为了完成这个目的,我们需要完成VGA或者串口的驱动.

内核的io函数在以下三个文件中实现.
- kern/printf.c
    - putch():putch()调用具体的console IO ->cputchar()->cons_putc().
    - vcprintf():vcprintf将可变参数转换成可变参数列表
    - cprintf():cprintf内核io最主要的接口,所有的输出都通过cprintf()函数
- kern/printfmt.c
    - vprintfmt():格式化输出的具体实现.
- kern/console.c
    - cputchar() VGA或者串口写字符
    - getchar() 键盘或串口读字符
    - iscons() 判断是否为终端,如果是终端总是需要回显


其中console.c 包含键盘,VGA,串口的驱动和对外提供的接口.


## Stack

# 内存管理
```

 Virtual memory map:                                Permissions
                                                    kernel/user

    4 Gig -------->  +------------------------------+
                     |                              | RW/--
                     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                     :              .               :
                     :              .               :
                     :              .               :
                     |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~| RW/--
                     |                              | RW/--
                     |   Remapped Physical Memory   | RW/--
                     |                              | RW/--
    KERNBASE, ---->  +------------------------------+ 0xf0000000      --+
    KSTACKTOP        |     CPU0's Kernel Stack      | RW/--  KSTKSIZE   |
                     | - - - - - - - - - - - - - - -|                   |
                     |      Invalid Memory (*)      | --/--  KSTKGAP    |
                     +------------------------------+                   |
                     |     CPU1's Kernel Stack      | RW/--  KSTKSIZE   |
                     | - - - - - - - - - - - - - - -|                 PTSIZE
                     |      Invalid Memory (*)      | --/--  KSTKGAP    |
                     +------------------------------+                   |
                     :              .               :                   |
                     :              .               :                   |
    MMIOLIM ------>  +------------------------------+ 0xefc00000      --+
                     |       Memory-mapped I/O      | RW/--  PTSIZE
 ULIM, MMIOBASE -->  +------------------------------+ 0xef800000
                     |  Cur. Page Table (User R-)   | R-/R-  PTSIZE
    UVPT      ---->  +------------------------------+ 0xef400000
                     |          RO PAGES            | R-/R-  PTSIZE
    UPAGES    ---->  +------------------------------+ 0xef000000
                     |           RO ENVS            | R-/R-  PTSIZE
 UTOP,UENVS ------>  +------------------------------+ 0xeec00000
 UXSTACKTOP -/       |     User Exception Stack     | RW/RW  PGSIZE
                     +------------------------------+ 0xeebff000
                     |       Empty Memory (*)       | --/--  PGSIZE
    USTACKTOP  --->  +------------------------------+ 0xeebfe000
                     |      Normal User Stack       | RW/RW  PGSIZE
                     +------------------------------+ 0xeebfd000
                     |                              |
                     |                              |
                     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                     .                              .
                     .                              .
                     .                              .
                     |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
                     |     Program Data & Heap      |
    UTEXT -------->  +------------------------------+ 0x00800000
    PFTEMP ------->  |       Empty Memory (*)       |        PTSIZE
                     |                              |
    UTEMP -------->  +------------------------------+ 0x00400000      --+
                     |       Empty Memory (*)       |                   |
                     | - - - - - - - - - - - - - - -|                   |
                     |  User STAB Data (optional)   |                 PTSIZE
    USTABDATA ---->  +------------------------------+ 0x00200000        |
                     |       Empty Memory (*)       |                   |
    0 ------------>  +------------------------------+                 --+

```

## 导言
内核的内存管理分为两个部分.首先内核在运行时除了栈空间之外,还需要动态的分配内存来存储其他的一些结构如页表\进程控制块\各种用途的缓存等.并且当内核创建用户进程,将用户程序的elf格式的镜像从磁盘读入内存时也需要分配内存.因此内核需要一个能对物理地址进行分配与回收的allocator.除此之外,为了完成对地址的抽象,让程序只需要关注链接地址\虚拟地址,因此还需要完成虚拟内存的功能.上一章讨论内核运行时已经对虚拟内存做了初步的讨论.x86的内存映射机制由MMU实现,对于内核来说,最主要的任务是合理的分配和维护页表和页目录.当进程切换时,也要保证页目录的正确性.

## 物理内存管理

### 内存探测
kernnel如何获取RAM的信息?实际上内存探测的功能由bios完成,kernel想要获取ram的信息,可以通过bios中断,也可直接从CMOS中直接读取BIOS已经得到的ram信息.在LOS中,我们采用后者.

CMOS的IO端口号为0x70和0x71.CMOS内有50字节大小的信息,通过向0x70输出offset来选择要读取的字节,通过0x71进行实际的信息读取.CMOS中记录内存大小的字节偏移为:
- 0x15:base memory size的低八位
- 0x16:base memory size的高八位
- 0x17:1MB - 16MB mem size的低八位
- 0x18:1MB - 16MB mem size的高八位
- 0x34:16MB - 4GB mem size的低八位
- 0x35:16MB - 4GM mem size的高八位

以上mem size的单位为KB.因此总共的ram大小(KB和页数)的计算方式为:
```
	if (ext16mem)      //16MB < mem_size 
		totalmem = 16 * 1024 + ext16mem;
	else if (extmem)   // 1MB < mem_size <= 16MB 
		totalmem = 1 * 1024 + extmem;
	else               // 0 < mem_size < 1MB
		totalmem = basemem;

    //物理页的个数
    npages = totalmem / (PGSIZE / 1024);
```
### 内存分配
内核的链接脚本中记录了kernel的地址范围.end记录了kernel的末尾地址(end是内核之后的第一个空闲地址).因此内核在早期使用内存时可以从end开始顺序的分配内存.理论上可以使用的空间为 end ~ min(0xfffffff, KERNBASE + totalmem).在LOS中我们在end之后创建了以下一些结构:
- kern_pgdir
    - 大小:PGSIZE
    - 作用:真正的页目录, 用于替换早期使用的entry pgdir.
- pages
    - 大小:npages * sizeof(struct PageInfo) 
    - 作用:物理页描述符的数组,记录物理页的引用信息.
- envs  
    - 大小:NENV * sizeof(struct Env)
    - 作用:进程描述符的数组,记录进程的信息.
- tdba
    - 大小:NTD * sizeof(struct tx_desc)
    - 作用:网卡驱动的发送描述符,关于网卡驱动将在网络的一章讨论
- pkt_bufs
    - 大小:NTD * MAX_PKT_SIZE
    - 作用:网卡驱动的发送缓冲区
- rdba
    - 大小:NRD * sizeof(struct rx_desc)
    - 作用:网卡驱动的接收描述符
- rx_bufs
    - 大小:NRD * RX_BUF_SIZE
    - 作用:网卡驱动的接收缓冲区

分配好这些内核需要的基本结构之后,就不应该再使用这种顺序分配的方式来分配内存了.下一步需要进行的是页的初始化,将每个物理页的描述符都进行初始化,并初始化一个由物理页描述符组成的空闲页链表.之后所有分配内存与释放内存的行为都由page_alloc()和page_free()操作空闲链表来完成.

## 虚拟内存
### 虚拟地址(逻辑地址),线性地址,和物理地址
> 详见Intel 80386 Reference Manual chapter 5\6
```

           Selector  +--------------+         +-----------+
          ---------->|              |         |           |
                     | Segmentation |         |  Paging   |
Software             |              |-------->|           |---------->  RAM
            Offset   |  Mechanism   |         | Mechanism |
          ---------->|              |         |           |
                     +--------------+         +-----------+
            Virtual                   Linear                Physical
```

在80386的地址模型中,段机制是一直存在的,也就是说不可以关闭.而页机制是可选的.在类UNIX的系统中,一般选择淡化段机制的功能,并且总是使用页机制.那么,如何做到这一点?

首先在开启保护模式和页机制使能的情况下,CPU使用的所有地址(不管是指令还是数据)都是虚拟地址(逻辑地址).由虚拟地址向线性地址转换需要两个结构:段寄存器和段描述符.x86的段寄存器有CS(code segment),DS(data segment)等.段寄存器中的内容在实模式和保护模式下代表的意义完全不同.在实模式下的意义我们在boot一章中已经讨论过,现在需要关注的它在保护模式下的意义.段寄存器的大小是16bit,0~1bit区分不同的权限,bit 2区分使用GDT(global descriptor table)还是LDT(local descriptor table),高13bit叫做段选择子,内核中有一个段描述符组成的数组,段选择子是数组的索引.因此段寄存器的主要功能是唯一的确定一个存储在内核中的段描述符.由虚拟地址向线性地址转换所需要的某些信息存储在段描述符中,具体来说,是段的起始地址(base)和段的大小(limit).有了base和limit之后就能够计算出线性地址了.根据惯例,我们用offset来表示虚拟地址(逻辑地址),用linear来表示线性地址,那么线性地址的计算方式为:linear = base + offset.(硬件通过limit检查offset是否合法).可以看到,当base等于0,limit等于0xffffffff时,虚拟地址和线性地址是一个恒等映射,并且能有使用0~4GB的全部空间.使用这种方法,可以忽略段机制.

那么下一个问题是,线性地址如何转换为物理地址?之前已经讨论过,内核中存在页目录和页表这两个结构.mmu通过线性地址的高10位作为页目录的索引从而得到相应页表的物理地址,再通过线性地址的中间10位作为刚刚得到的页表的索引从而得到物理页的地址.物理页的地址总是对齐于页大小,所以物理页地址的低12位总是0.线性地址的低12位作为物理页内的偏移加上物理页的地址就能够得到相应字节的具体物理地址.

以上就是虚拟地址,线性地址,物理地址之间的关系与转换方式.

### 内核地址空间的初始化
LOS的内核地址空间的映射策略是, 加载内核时,将内核放置于物理地址1MB处, 然后内核所处的虚拟地址空间的范围是0xf0100000 ~ 0xffffffff. 在映射时, 选择将0xf0000000 ~ 0xffffffff 映射到 0x0 ~ 0x0fffffff(256M).也就是说通过0xf0000000 ~ 0xf0100000这段虚拟地址能够访问1M以下的低地址.我们将0xf0000000称为内核的基地址KERNBASE,虽然实际上内核镜像真正的开始处位于0xf0100000.这么做的原因是,1M以下的地址如vga缓存等需要经常的被内核访问,但使用页机制之后又不能直接的使用物理地址.如果直接将1M以下的低地址和内核镜像一起通过加上一个偏移量映射到高256MB的地址空间,将是简单,方便,合理的一种方法.另外,之所以不能采用恒等映射是因为,0 ~ UTOP之间的虚拟地址空间都预留给了用户进程.

在boot一章中,内核在刚开始运行时使用的是手动构建的页目录和页表,能够映射的虚拟地址范围是0xf0000000 ~ 0xf0400000.所以在内核完成entry阶段后,进入内存初始化时,首先需要在页目录和页表中构建上述的地址映射关系.也就是说,将0xf0000000 ~ 0xffffffff这段虚拟地址映射到 0 ~ 0x0fffffff(256M)这段物理地址.

除了KERNBASE以上的地址空间需要映射之外,还需要将用户空间的高地址处以只读的方式映射到一些内核数据结构.这么做的原因是,用户进程在运行时并没有访问内核空间的权限,但是用户进程在某些情况下又确实需要获取内核中某些结构的信息.为了达到这个目的,用户进程只能通过两种方式,其中之一是我们在这采取的,通过只读的方式将用户空间的地址映射到需要的内核数据,采用这种方式,用户进程在获取内核数据时没有额外的开销.另一种方式是用过系统调用, 进程在系统调用时需要在用户态和内核态之间切换, 引入了不可忽视的额外开销.

UVPT: 某些时候,用户态的进程会想知道某个虚拟地址的信息,是否存在?读?写?COW?shared?等等.这都要求进程能得到这个地址对应的页表项.在LOS中,我们把UVPT ~ UVPT+4M 这段虚拟地址映射到所有的页表项. 完成这种映射关系只需要进行一个很简单的操作:kern_pgdir[PDX(UVPT)] = PADDR(kern_pgdir) | PTE_U | PTE_P;首先确保UVPT的低22位为0,有了这个前提后,UVPT~UVPT+4M这段空间的高10位都是相同的,也就是说在通过MMU转换时会用同一个值去索引页目录.由于我们把页目录中索引为PDX(UVPT)的条目中指向的页表地址指定为页目录本身的地址,所以在MMU使用UVPT~UVPT+4M这段范围当中的任何一个虚拟地址的中间10位进行索引时,使用的页表仍然是页目录(页目录充当了二级页表).使用这种映射方式的效果是,UVPT~UVPT+4M这段范围中的任何一个地址的中间10位指定了页目录项在页目录中的偏移,配合低十二位可以精确的访问到每一个页表项.完成映射之后,如果我们想要查看va对应的页表项,首先我们需要知道va的页目录项是否存在.如何查看页目录?通过刚才的映射我们只得到了全部的页表,而并没有页目录,但是不难发现从uvpd = (UVPT | UVPT>>10)这个地址开始的整个页必然映射到页目录的物理地址.所以为了检查va对应的页目录项是否存在,我们使用PDX(va)作为索引,从uvpd中查看uvpd[PDX(va)]的存在标志位.如果确实存在,则能够通过UVPT[pageno = va/PGSIZE]来查看页表项的存在标志位.

UPAGES: 将物理页描述符的数组映射到用户空间
UENVS: 将进程描述符的数组映射到用户空间

### 操控页目录和页表的函数

# 用户态进程

## 引入进程概念
到目前为止,我们仅仅讨论了操作系统在内核态下的一些初始化行为.当完成基本的内存映射初始化后,内核需要做更多的事情.在用户的视角中,操作系统最直观的功能就是支持用户态程序的运行.

为了简化模型,假设LOS使用的是单核单线程的CPU.CPU+内存本质上是一个状态机,程序在运行时对应着某一个状态.为了对程序在运行时的状态有直观的认识,我们首先观察程序执行过程中的一个静态视图,即一个状态.这里我们选择观察的是初始状态.当程序刚刚被加载进内存时,内存中记录了代码,数据,栈和堆信息.CPU寄存器组的初始状态确保我们能够通过CS:IP找到入口点的代码,DS:offset能够找到内存中的数据,SS:ESP能够找到栈,页目录基地址寄存器能够将CPU和内存关联起来.在程序运行时,通用寄存器和标志寄存器中还会存储着动态的信息.也就是说,通过内存和寄存器组的状态就能确定一个程序的运行时状态.因此,我们完全可以在内核中创建一种数据结构来保存这种状态.需要保存的寄存器只有数十个,确定内存的状态也只需要一张页目录(内存映射机制确保了虚拟地址并不会冲突).当需要恢复运行时,只需要将寄存器中的值切换为保存的值,使页目录基地址寄存器指向正确的地址,最后切换CS:EIP指向的指令就行了,状态转移的方式取决于下一条指令执行的操作,我们在执行状态转移之前已经完全恢复了状态,CS:EIP也正确的指向下一条指令的地址,所以转移后的下一个状态也必然是正确的.这种程序运行时的模型也就是进程.在UNIX中,进程就是地址空间+线程,线程是将程序的执行流比作线,程序的执行流由代码段和CS:IP寄存器共同决定,所以地址空间+线程等于之前讨论的地址空间+寄存器组的状态.另外,尽管内核本身没有相应的进程结构,但是内核也符合这种地址空间+线程的模型,所以内核在运行时也是一个进程.

以下是定义在内核中的进程控制块的结构:
```
struct Env {
	struct Trapframe env_tf;	// Saved registers
	struct Env *env_link;		// Next free Env
	envid_t env_id;			// Unique environment identifier
	envid_t env_parent_id;		// env_id of this env's parent
	enum EnvType env_type;		// Indicates special system environments
	unsigned env_status;		// Status of the environment
	uint32_t env_runs;		// Number of times environment has run
	int env_cpunum;			// The CPU that the env is running on

	// Address space
	pde_t *env_pgdir;		// Kernel virtual address of page dir

	// Exception handling
	void *env_pgfault_upcall;	// Page fault upcall entry point


	bool env_ipc_recving;		// Env is blocked receiving
	void *env_ipc_dstva;		// VA at which to map received page
	uint32_t env_ipc_value;		// Data value sent to us
	envid_t env_ipc_from;		// envid of the sender
	int env_ipc_perm;		// Perm of page mapping received
};
```
env_tf: 保存了通用寄存器,段寄存器和指令指针寄存器等内容.此外还有中断时的一些额外信息,关于中断,之后再讨论.

env_link: 内核使用一个数组来记录env结构,数组中的env如果没有用来记录进程信息就是空闲的,所有空闲的env结构都会通过env_link链接到空间链表中,这样在需要为进程分配env结构时只需要从空闲链表中取出一个env结构.

env_id: 进程的唯一标示符

env_parent_id: 进程的父进程ID

env_type:进程的类型,用来区别普通的用户进程和特殊的进程,之后再讨论.

env_status: 进程的状态有:
- ENV_FREE: env结构是空闲的,所以这个env结构应该在空闲链表中
- ENV_RUNNABLE: 表示当前的env可以被调度,即正在等待被CPU运行
- ENV_RUNNING: 表示当前的env正在被运行,实际上是表示发出int请求的是env代表的进程.
- ENV_NOTRUNNABLE: 不可运行,进程正在等待某个条件,之后再讨论
- ENV_DYING: 僵尸进程,env可以被释放.

env_runs: 进程运行的次数/被调度的次数

env_cpunum: 运行进程的CPUid

env_pgdir: 进程使用的页目录

env_pgfault_upcall: 处理页错误的回调函数, 之后讨论

env_ipc_*: 关于ipc的一些控制信息, 之后讨论


除了寄存器组的内容和页目录的地址之外,进程控制块中还需要包含一些元数据,例如进程ID,父进程ID,进程类型,进程状态等.这些元数据用于进程间通信,进程调度,空闲进程控制块的分配等作用.

## 创建进程

创建进程总是可以分解为以下几个步骤:
1. 为进程分配一个空闲的进程控制块
2. 初始化进程id等元信息. 初始化进程控制块中的寄存器信息:将CS以外的段寄存器初始化为用户数据段, 将CS初始化为用户代码段, 将通用寄存器的值初始化为0, 将esp指向用户栈, 将标志寄存器中的中断标志置位(使能中断). 为该进程分配一个新的页目录,页目录中暂时只映射内核空间.
3. 为进程分配内存:将程序镜像(ELF)载入内存, 顺便将eip指向程序的入口点(因为程序的入口点包含在elf头中,所以eip的值之后在得到elf镜像后才能初始化).为用户栈分配空间.

当执行完这些步骤之后,进程控制块中就已经包含了运行该进程所需要的所有信息了.此时进程的状态应该是runnable,剩下的只是等待内核对其进行调度.

## 从内核态到用户态

当某个进程得到调度,在离开内核的最后阶段,总是需要完成下面的操作:
1. 将进程的状态设置为running
2. 将进程控制块中页目录地址载入页目录基地址寄存器.完成这一步时,我们仍然处于内核进程中,虽然使用了进程的页目录,但内核仍然能够正常运行的原因是:所有进程的地址空间中,都包含了内核的地址映射,只不过只有在内核态下,这段地址才是允许被访问的.
3. 还原寄存器中的值:依次还原通用寄存器, 段寄存器最后使用iret指令还原SS:ESP, CS:EIP和EFLAGS寄存器. 每当CS:EIP被改变时, CPU的行为就会被改变,在这里,当执行完iret指令之后,控制权就已经回到了用户态进程,下一条指令是用户进程的指令, 改变的状态也是用户进程的地址空间或者是寄存器组中某个寄存器的状态.




## 从用户态到内核态

在Intel 80386的硬件手册中,有两种特殊的控制权转移方式:
1. Interrupt(中断):这是硬件中断,由外部设备引起,通常称为硬中断.硬中断又分为两种:可屏蔽中断(IRQ:Interrupt request 中断请求)和不可屏蔽中断(NMI:Non-Maskable interrupt).中断请求通过cpu的INTR引脚连接,一般用于输入输出设备, NMI通过CPU的NMI引脚连接,一般用于内存或电源这些如果容易出现致命错误的设备.

2. Exceptions(异常): 异常分为处理器检测到的指令异常和程序主动请求的异常.
- 处理器检测到的指令异常: 当指令的行为不正确时(除0, MMU无法完成地址转换等等),处理器能够检测到的指令异常行为进一步分为:faults, traps, aborts.当处理器检测到的异常行为类型为faults时,在异常处理程序完成后,控制权重新回到引发异常的进程时,仍然执行引发异常的那一条指令,因为faults这个类型一般认为是可修复的,也就是说,在异常处理程序完成之后再次执行这条指令就能成功,而不会再次引发异常.典型的例子是page fault,之后会详细讨论用户态下page fault的异常是如何处理的.如果异常行为的类型是traps时, 在控制权重新回来时会执行下一条指令,因为在trap的情况下,程序只是想暂时陷入内核态观察程序的静态视图(寄存器中的值等).aborts通常是严重错误,CPU无法精确的获得正确的CS:EIP,在这种情况下,程序会终止.
- 程序主动请求的异常: 可以通过INTO, INT 3, INT n, 和BOUND这几个指令来触发异常．我们通常将这种异常称为软中断，但是处理器实际上将它们当成是一种异常行为． 

不管是中断还是异常，处理器和操作系统处理它们的方式都是一样的．首先讨论一些必要的数据结构:
- 中断描述符表: 之前已经遇到过全局描述符表,关于x86中描述符这个术语,我想做一些额外的解释.所有的描述符都是直接或间接描述"某个例程"或者"某个结构"所在位置的一个结构.如果某个描述符直接的完成了这个目的,通常这样的描述符中会包含目标的基地址和size(就像全局描述符中包含了某个段的 base 和 limit一样).也有间找到目标的描述符,这种描述符的结构和段寄存器类似,其中包含了一个选择子,通过选择子可以在另一张表(如GDT)中通过偏移的方式找到真正的描述符.所以描述符描述的是找到目标所需要的一些信息.中断描述符表中的描述符类型分为三种:
    - task gate: 间接的描述符, 包含一个tss(task state segment)选择子用于从GDT或者LDT中找到相应的TSS结构.TSS结构中记录了目标任务的所有信息.
    - interrupt gage: 直接的描述符, 记录了处理例程的段和段内的偏移.
    - trap gate: 和interrup gate大致一样,唯一的区别在于:trap gate不屏蔽中断,而Interrupt gate需要屏蔽中断.

当中断发生时(不管是Interrupt还是exception), 都有一个唯一的中断向量号和这个中断对应.在x86中,0-31对应着处理器检测的异常行为.硬件产生中断都会有对应的IRQ号,中断控制器将IRQ号映射到不同的中断向量号, 它们之间的映射规则由中断控制器决定,但是需要遵守的原则是,映射之后中断向量号与0-31以及软中断使用的中断向量号都不冲突.最后，INT指令使用的中断向量号就是int n中的n.每个中断都通过中断向量号作为索引,通过中断描述符表找到目标例程的位置.例如,软中断通过中断向量号找到一个trap gate,trap gate中记录了处理历程的段和段内的偏移.接下来需要做的就是当前运行进程和中断处理程序之间的切换.


当中断发生后，CPU使用中断向量号找到相应的trap gate或者interrupt gate， 然后将控制权转交到由gate决定的中断处理例程。由于我们现在讨论的是从用户态到内核态，也就是说特权级发生了变化，所以在控制权转交时隐含从用户态的栈切换到了内核的栈。内核栈的位置由TR寄存器决定。当控制权转交之后CPU自动地将一些数据保存在了内核栈中，比如SS:ESP, EFLAGS, CS:EIP以及可选的error code。为了在完成中断例程后能精确的恢复引发中断的例程的运行状态，更多的寄存器信息需要被保存。这些额外的寄存器保存操作需要由中断处理例程来完成。LOS的中断处理例程分为两个阶段，第一个阶段是保存所有寄存器的信息，以及保存中断向量号。将这些信息构造成一个结构通过参数传递的方式传递给处理例程的第二个阶段。处理例程的第二个阶段通过中断向量号来决定使用怎样的方式来处理这个中断。完成处理之后再根据已保存的寄存器信息恢复进程的执行。
```
                     +--------------------+ KSTACKTOP             
                     | 0x00000 | old SS   |     " - 4
                     |      old ESP       |     " - 8
                     |     old EFLAGS     |     " - 12
                     | 0x00000 | old CS   |     " - 16
                     |      old EIP       |     " - 20
                     |     error code     |     " - 24 <---- ESP
                     +--------------------+   
```


进程的被保存的寄存器状态通过TrapFrame结构来表示:
```
struct Trapframe {
	struct PushRegs tf_regs;
	uint16_t tf_es;
	uint16_t tf_padding1;
	uint16_t tf_ds;
	uint16_t tf_padding2;
	uint32_t tf_trapno;
	/* below here defined by x86 hardware */
	uint32_t tf_err;
	uintptr_t tf_eip;
	uint16_t tf_cs;
	uint16_t tf_padding3;
	uint32_t tf_eflags;
	/* below here only when crossing rings, such as from user to kernel */
	uintptr_t tf_esp;
	uint16_t tf_ss;
	uint16_t tf_padding4;
} __attribute__((packed));

```

trapframe作为参数传递给trap()函数，trap()将trapframe保存到相应进程的进程控制块中，因为中断处理例程可能使用调度器恢复另一个进程，所以如果不保存到进程控制块中，相应进程的执行状态可能会丢失。然后，trap()进一步使用trap_dispatch()通过trap number选择合适的中断处理例程进行处理。

## 用户态进程的入口点
以往，我们写用户态程序的时候只需要关注从main()开始的执行流。实际上在main函数之前，进程还需要额外进行一些处理。在LOS中，用户态进程的入口点在lib/entry.S中。lib/entry.S定义了一些全局变量，lib/entry.S中的入口是_start, _start检查用户栈上的参数，如果没有参数，就用0构造两个参数来满足gcc函数调用的格式。接着_start调用libmain,libmain.c中定义了两个全局变量用以存放当前进程的进程控制块以及当前进程的img的名称，接着libmain使用同样的参数调用umain.umain()就是传统意义上的程序入口点，对于编写用户态程序而言，只需要关注从umain开始的程序行为。


## 从用户态创建进程


### 使用copy on write 机制的fork()
在用户态下,unix可以使用fork()创建进程.不管fork()的实现策略是怎样的,在fork返回之后,内核中会存在两个一样的进程.少数的区别在于,这两个进程的返回值不同,父进程的返回值是子进程的进程ID,子进程的返回值是0.除此之外,进程使用的地址空间从用户态看是完全一样的,但实际上使用了不同的页目录,就像平行空间一样,父子进程之间是互不干扰的.

LOS将fork实现为用户态的库函数,并且采用写时复制的策略.有些版本的unix在实现fork时会完全复制整个地址空间,也就是说对于页表中每一个存在的项,都要重新申请一个物理页,并且将内容复制过来.这么做的弊端是,在fork()执行期间需要花费大量的用于复制的开销.并且这些开销很多时候是不必要的.比如,如果fork()完之后调用exec()来执行另一个程序.那么该进程的地址空间需要被完全替换,那么原本在fork()期间申请与复制物理页的开销就完全浪费了.所以我们需要采用COW的方式来避免这种无意义的开销.采用COW策略的页表项会有一个标记,当我们试图对标记为COW的页进行写操作时,会引发一个page fault的异常,前面几节已经讨论过,用户态在出现异常之后会自动切换到内核态,控制权会交到内核中的异常处理程序.在LOS中,内核中的异常处理程序根据中断向量号将异常判断为page fault之后再调用具体的page fault的处理例程.在page fault的处理例程纠正"向标记为COW的页进行写操作"的行为时,先申请一个新的物理页,然后将原来页中的内容复制到申请的页中,然后改写引发page fault的进程页表,使他的页表项指向新的物理页,并且将COW标记替换为W标记.

### 用户态的page fault处理函数
上面我们说过,在用户态中遇到向标记为COW的页进行写操作时会引发异常,然后内核中的异常处理程序会处理这个异常.实际上,内核的异常处理程序最终调用的是用户态进程传递给内核的一个函数地址.也就是说,如果用户态进程想要通过某种方式对出现在用户态下的page fault进行处理,就必须事先(引发page fault之前)决定好使用怎样的page fault处理函数,并且将这个函数存放到一个和内核约定好的地方以便让内核在处理异常时能够正确的调用这个函数.



## 进程间通信

## 抢占式多任务

## 调度


# 文件系统

# 网卡驱动