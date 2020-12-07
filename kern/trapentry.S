/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>
#include <kern/macro.h>

#include <kern/picirq.h>

#ifdef CONFIG_KSPACE
.comm intr_ret_rip, 8
.comm intr_rsp_reg, 8
.comm intr_cs, 8
.comm intr_ss, 8
.comm intr_rflags, 8
#endif

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
#define TRAPHANDLER(name, num)                                   \
  .globl name;            /* define global symbol for 'name' */  \
  .type name, @function;  /* symbol type is function */          \
  .align 2;               /* align function definition */        \
  name:                   /* function starts here */             \
  pushq $(num);                                                  \
  jmp _alltraps

/* Use TRAPHANDLER_NOEC for traps where the CPU doesn't push an error code.
 * It pushes a 0 in place of the error code, so the trap frame has the same
 * format in either case.
 */
#define TRAPHANDLER_NOEC(name, num) \
  .globl name;                      \
  .type name, @function;            \
  .align 2;                         \
  name:                             \
  pushq $0;                         \
  pushq $(num);                     \
  jmp _alltraps

.text

.globl _alltraps
.type _alltraps, @function;
.align 2
_alltraps:
  subq $8,%rsp
  movw %ds,(%rsp)
  subq $8,%rsp
  movw %es,(%rsp)
  PUSHA
  movq $GD_KD,%rax
  movq %rax,%ds
  movq %rax,%es
  movq %rsp,%rdi
  call trap
  jmp .

.globl clock_thdlr
.type clock_thdlr, @function;
.align 2;
#ifdef CONFIG_KSPACE
clock_thdlr:
  popq intr_ret_rip(%rip)
  popq intr_cs(%rip)
  popq intr_rflags(%rip)
  popq intr_rsp_reg(%rip)
  popq intr_ss(%rip)
  leaq bootstacktop(%rip),%rsp
  pushq intr_ss(%rip)
  pushq intr_rsp_reg(%rip)
  pushq intr_rflags(%rip)
  pushq intr_cs(%rip)
  pushq intr_ret_rip(%rip)
  pushq $0x0
  pushq $(IRQ_OFFSET + IRQ_CLOCK)
  pushq $0x0 // %ds
  pushq $0x0 // %es

  pushq %rax
  pushq %rbx
  pushq %rcx  extern void (*divide_thdlr)(void);
	extern void (*debug_thdlr)(void);
	extern void (*nmi_thdlr)(void);
	extern void (*brkpt_thdlr)(void);
	extern void (*oflow_thdlr)(void);
	extern void (*bound_thdlr)(void);
	extern void (*illop_thdlr)(void);
	extern void (*device_thdlr)(void);
	extern void (*tss_thdlr)(void);
	extern void (*segnp_thdlr)(void);
	extern void (*stack_thdlr)(void);
	extern void (*gpflt_thdlr)(void);
	extern void (*pgflt_thdlr)(void);
	extern void (*fperr_thdlr)(void);

  
    
  extern void (*syscall_thdlr)(void);

	SETGATE(idt[T_DIVIDE], 0, GD_KT, (uint64_t) &divide_thdlr, 0);
	SETGATE(idt[T_DEBUG], 0, GD_KT, (uint64_t) &debug_thdlr, 0);
	SETGATE(idt[T_NMI], 0, GD_KT, (uint64_t) &nmi_thdlr, 0);
	SETGATE(idt[T_BRKPT], 0, GD_KT, (uint64_t) &brkpt_thdlr, 3);
	SETGATE(idt[T_OFLOW], 0, GD_KT, (uint64_t) &oflow_thdlr, 0);
	SETGATE(idt[T_BOUND], 0, GD_KT, (uint64_t) &bound_thdlr, 0);
	SETGATE(idt[T_ILLOP], 0, GD_KT, (uint64_t) &illop_thdlr, 0);
	SETGATE(idt[T_DEVICE], 0, GD_KT, (uint64_t) &device_thdlr, 0);
	SETGATE(idt[T_TSS], 0, GD_KT, (uint64_t) &tss_thdlr, 0);
	SETGATE(idt[T_SEGNP], 0, GD_KT, (uint64_t) &segnp_thdlr, 0);
	SETGATE(idt[T_STACK], 0, GD_KT, (uint64_t) &stack_thdlr, 0);
	SETGATE(idt[T_GPFLT], 0, GD_KT, (uint64_t) &gpflt_thdlr, 0);
	SETGATE(idt[T_PGFLT], 0, GD_KT, (uint64_t) &pgflt_thdlr, 0);
	SETGATE(idt[T_FPERR], 0, GD_KT, (uint64_t) &fperr_thdlr, 0);
    
  SETGATE(idt[T_SYSCALL], 0, GD_KT, (uint64_t) &syscall_thdlr, 3);
  // LAB 8 code
  pushq %rdx
  pushq %rbp
  pushq %rdi
  pushq %rsi
  pushq %r8
  pushq %r9
  pushq %r10
  pushq %r11
  pushq %r12
  pushq %r13
  pushq %r14
  pushq %r15

  xorl %ebp, %ebp
  movq %rsp,%rdi
  call trap
  jmp .
#else
TRAPHANDLER_NOEC(clock_thdlr, IRQ_OFFSET + IRQ_CLOCK)
// LAB 8: Your code here.
TRAPHANDLER_NOEC(divide_thdlr, T_DIVIDE)
TRAPHANDLER_NOEC(debug_thdlr, T_DEBUG)
TRAPHANDLER_NOEC(nmi_thdlr, T_NMI)
TRAPHANDLER_NOEC(brkpt_thdlr, T_BRKPT)
TRAPHANDLER_NOEC(oflow_thdlr, T_OFLOW)
TRAPHANDLER_NOEC(bound_thdlr, T_BOUND)
TRAPHANDLER_NOEC(illop_thdlr, T_ILLOP)
TRAPHANDLER_NOEC(device_thdlr, T_DEVICE)
TRAPHANDLER(dblflt_thdlr, T_DBLFLT)
TRAPHANDLER(tss_thdlr, T_TSS)
TRAPHANDLER(segnp_thdlr, T_SEGNP)
TRAPHANDLER(stack_thdlr, T_STACK)
TRAPHANDLER(gpflt_thdlr, T_GPFLT)
TRAPHANDLER(pgflt_thdlr, T_PGFLT)
TRAPHANDLER_NOEC(fperr_thdlr, T_FPERR)
TRAPHANDLER(align_thdlr, T_ALIGN)
TRAPHANDLER_NOEC(mchk_thdlr, T_MCHK)
TRAPHANDLER_NOEC(simderr_thdlr, T_SIMDERR)

TRAPHANDLER_NOEC(syscall_thdlr, T_SYSCALL)
#endif
