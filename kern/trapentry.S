/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>

#include <kern/picirq.h>


###################################################################
# exceptions/interrupts
###################################################################

/* TRAPHANDLER defines a globally-visible function for handling a trap.
 * It pushes a trap number onto the stack, then jumps to _alltraps.
 * Use TRAPHANDLER for traps where the CPU automatically pushes an error code.
 *
 * You shouldn't call a TRAPHANDLER function from C, but you may
 * need to _declare_ one in C (for instance, to get a function pointer
 * during IDT setup).  You can declare the function with
 *   void NAME();	
 * where NAME is the argument passed to TRAPHANDLER.
 */
#define TRAPHANDLER(name, num)						\
	.globl name;		/* define global symbol for 'name' */	\
	.type name, @function;	/* symbol type is function */		\
	.align 2;		/* align function definition */		\
	name:			/* function starts here */		\
	pushl $(num);							\
	jmp _alltraps

/* Use TRAPHANDLER_NOEC for traps where the CPU doesn't push an error code.
 * It pushes a 0 in place of the error code, so the trap frame has the same
 * format in either case.
 */
#define TRAPHANDLER_NOEC(name, num)					\
	.globl name;							\
	.type name, @function;						\
	.align 2;							\
	name:								\
	pushl $0;							\
	pushl $(num);							\
	jmp _alltraps

#define INTHANDLER_NOEC(name, num)					\
	.globl name;							\
	.type name, @function;						\
	.align 2;							\
	name:								\
	cli;                \
  pushl $0;							\
	pushl $(num);							\
	jmp _alltraps

.text

/*
 * Lab 3: Your code here for generating entry points for the different traps.
 */
.globl _alltraps;

TRAPHANDLER_NOEC(vector0, 0);
TRAPHANDLER_NOEC(vector1, 1);
TRAPHANDLER_NOEC(vector2, 2);
TRAPHANDLER_NOEC(vector3, 3);
TRAPHANDLER_NOEC(vector4, 4);
TRAPHANDLER_NOEC(vector5, 5);
TRAPHANDLER_NOEC(vector6, 6);
TRAPHANDLER_NOEC(vector7, 7);

TRAPHANDLER(vector8, 8);
TRAPHANDLER_NOEC(vector9, 9);
TRAPHANDLER(vector10, 10);
TRAPHANDLER(vector11, 11);
TRAPHANDLER(vector12, 12);
TRAPHANDLER(vector13, 13);
TRAPHANDLER(vector14, 14);
TRAPHANDLER_NOEC(vector15, 15);
TRAPHANDLER_NOEC(vector16, 16);
TRAPHANDLER(vector17, 17)
TRAPHANDLER_NOEC(vector18, 18);
TRAPHANDLER_NOEC(vector19, 19);
TRAPHANDLER_NOEC(vector20, 20);
TRAPHANDLER_NOEC(vector21, 21);
TRAPHANDLER_NOEC(vector22, 22);
TRAPHANDLER_NOEC(vector23, 23);
TRAPHANDLER_NOEC(vector24, 24);
TRAPHANDLER_NOEC(vector25, 25);
TRAPHANDLER_NOEC(vector26, 26);
TRAPHANDLER_NOEC(vector27, 27);
TRAPHANDLER_NOEC(vector28, 28);
TRAPHANDLER_NOEC(vector29, 29);
TRAPHANDLER_NOEC(vector30, 30);
TRAPHANDLER_NOEC(vector31, 31);
INTHANDLER_NOEC(vector32, 32);
INTHANDLER_NOEC(vector33, 33);
INTHANDLER_NOEC(vector34, 34);
INTHANDLER_NOEC(vector35, 35);
INTHANDLER_NOEC(vector36, 36);
INTHANDLER_NOEC(vector37, 37);
INTHANDLER_NOEC(vector38, 38);
INTHANDLER_NOEC(vector39, 39);
INTHANDLER_NOEC(vector40, 40);
INTHANDLER_NOEC(vector41, 41);
INTHANDLER_NOEC(vector42, 42);
INTHANDLER_NOEC(vector43, 43);
INTHANDLER_NOEC(vector44, 44);
INTHANDLER_NOEC(vector45, 45);
INTHANDLER_NOEC(vector46, 46);
INTHANDLER_NOEC(vector47, 47);
INTHANDLER_NOEC(vector48, 48);


/*
 * Lab 3: Your code here for _alltraps
 */


_alltraps:
  # Build trap frame.
  pushl %ds
  pushl %es

  pushal
  
  # Set up data and per-cpu segments.
  movw $(GD_KD), %ax
  movw %ax, %ds
  movw %ax, %es


  # Call trap(tf), where tf=%esp, so %esp is a param
  pushl %esp
  call trap
  addl $4, %esp



.data
.globl vectors
vectors:
  .long vector0
  .long vector1
  .long vector2
  .long vector3
  .long vector4
  .long vector5
  .long vector6
  .long vector7
  .long vector8
  .long vector9
  .long vector10
  .long vector11
  .long vector12
  .long vector13
  .long vector14
  .long vector15
  .long vector16
  .long vector17
  .long vector18
  .long vector19
  .long vector20
  .long vector21
  .long vector22
  .long vector23
  .long vector24
  .long vector25
  .long vector26
  .long vector27
  .long vector28
  .long vector29
  .long vector30
  .long vector31
  .long vector32
  .long vector33
  .long vector34
  .long vector35
  .long vector36
  .long vector37
  .long vector38
  .long vector39
  .long vector40
  .long vector41
  .long vector42
  .long vector43
  .long vector44
  .long vector45
  .long vector46
  .long vector47
  .long vector48
