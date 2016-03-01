        /* Sample initialization file */
        .extern boot_main
        .extern exit
        .text
        .code 32

        .align  0

        .extern __bss_beg__
        .extern __bss_end__
        .extern __boot_bss_beg__
        .extern __boot_bss_end__
        .extern __stack_end__
		.extern _flash_size_
		.extern _ram_size_

        .global start
        .global endless_loop
		.global SVC_STACK_SIZE
		.global TOTAL_STACK_SIZE  

        /* Stack Sizes */
    .set  UND_STACK_SIZE, 0x00000040
    .set  ABT_STACK_SIZE, 0x00000040
    .set  FIQ_STACK_SIZE, 0x00000400
    .set  IRQ_STACK_SIZE, 0X00000800
    .set  SVC_STACK_SIZE, 0x00001000

	/* This TOTAL_STACK_SIZE will affect the malloc's heap end */	
    .set  TOTAL_STACK_SIZE, SVC_STACK_SIZE + IRQ_STACK_SIZE + FIQ_STACK_SIZE + ABT_STACK_SIZE + UND_STACK_SIZE 


/*
	.set  UART_PAD28_ADDR, 0xc0000340
    .set  UART_PAD28_VAL,  0x10001008
	.set  UART_PAD25_ADDR, 0xc0000338
    .set  UART_PAD25_VAL,  0x1800002b
	.set  UART_SEL0_ADDR,  0xc00003cc
    .set  UART_SEL0_VAL,   0x2000
*/

	.set  UART_PAD53_ADDR, 0xc0000388
    .set  UART_PAD53_VAL,  0x1000002b
	.set  UART_PAD54_ADDR, 0xc000038c
    .set  UART_PAD54_VAL,  0x00003000
	.set  UART_SEL1_ADDR,  0xc00003cc
    .set  UART_SEL1_VAL,   0x00004000


	.set  UART_BAUDRATE_ADDR,  0xc0000c1c
    .set  UART_BAUDRATE_VAL,   0xe
	.set  UART_DATA,  		0xc0000c00
	.set  REJUMP_FLASH,		0x03000058	
	.set  REMAP,			0xc0000004	


        /* Standard definitions of Mode bits and Interrupt (I & F) flags in PSRs */
    .set  MODE_USR, 0x10            /* User Mode */
    .set  MODE_FIQ, 0x11            /* FIQ Mode */
    .set  MODE_IRQ, 0x12            /* IRQ Mode */
    .set  MODE_SVC, 0x13            /* Supervisor Mode */
    .set  MODE_ABT, 0x17            /* Abort Mode */
    .set  MODE_UND, 0x1B            /* Undefined Mode */
    .set  MODE_SYS, 0x1F            /* System Mode */

    .equ  I_BIT, 0x80               /* when I bit is set, IRQ is disabled */
    .equ  F_BIT, 0x40               /* when F bit is set, FIQ is disabled */

.section .startup2,"ax"
        .code 32
        .align 0
_start:
	ldr r0, =REJUMP_FLASH	//redump to let pc + 0x300-0000
	mov pc,r0
	nop
	nop
	nop
	nop

	
    ldr r3, =REMAP			//let 0x0 mapping to sram
    mov r2,#0x1
    str r2,[r3]

/*
    ldr r1, =UART_PAD25_ADDR
    ldr r2, =UART_PAD25_VAL
    str r2, [r1]

    ldr r1, =UART_PAD28_ADDR
    ldr r2, =UART_PAD28_VAL
    str r2, [r1]

    ldr r1, =UART_SEL0_ADDR
    ldr r2, =UART_SEL0_VAL
    str r2, [r1]
*/


	ldr r1, =UART_PAD53_ADDR
    ldr r2, =UART_PAD53_VAL
    str r2, [r1]

    ldr r1, =UART_PAD54_ADDR
    ldr r2, =UART_PAD54_VAL
    str r2, [r1]

    ldr r1, =UART_SEL1_ADDR
    ldr r2, =UART_SEL1_VAL
    str r2, [r1]


    ldr r1, =UART_BAUDRATE_ADDR
    ldr r2, =UART_BAUDRATE_VAL
    str r2, [r1]
//-------------write uart data -----------------
/*
    ldr r1, =UART_DATA
    mov r2,#0x62	//'b'
    str r2,[r1]
    mov r2,#0xa
    str r2,[r1]
    mov r2,#0xd
    str r2,[r1]
*/
//---------------------------Copy KEEP_IN_SRAM-----------------------

//      clear register
        movs r0, #0
        movs r1, #0 
        movs r2, #0
        movs r3, #0
_CopyTextSectionInit:
        ldr r0, = _btext_beg_src
        ldr r1, = _btext_beg_dst
        ldr r2, = _btext_end_dst
        //  r3: temp register

        //start to copy!!
        b _LoopCopyTextSection

_LoopCopyTextSection:
        ldr r3,[r0],#4   // NOR ->r3
        str r3,[r1],#4
        cmp r1,r2
        bcc _LoopCopyTextSection 


/*
    ldr r1, =UART_DATA
    mov r2,#0x66	//'f'
    str r2,[r1]
    mov r2,#0xa
    str r2,[r1]
    mov r2,#0xd
    str r2,[r1]
*/
//---------------------------Copy DATA NOR->SRAM-----------------------

//      clear register
        movs r0, #0
        movs r1, #0 
        movs r2, #0
        movs r3, #0
_CopyDataSectionInit:
        ldr r0, = _boot_dbeg_src
        ldr r1, = _boot_dbeg_dst
        ldr r2, = _boot_dend_dst
        //  r3: temp register

        //start to copy!!
        b _LoopCopyDataSection

_LoopCopyDataSection:
        ldr r3,[r0],#4   // NOR ->r3
        str r3,[r1],#4
        cmp r1,r2
        bcc _LoopCopyDataSection 

/*
    ldr r1, =UART_DATA
    mov r2,#0x67	//'g'
    str r2,[r1]
    mov r2,#0xa
    str r2,[r1]
    mov r2,#0xd
    str r2,[r1]
*/

_mainCRTStartup:

        /* Disable WatchDog*/

        /* start timer_us_03 & timer_ms_03 */
        /*
        ldr  r1, =0xffff
        ldr  r3, =0xc0000500
        str  r1, [r3]
        ldr  r3, =0xc0000900
        str  r1, [r3]
        */

        /* Setup a stack for each mode - note that this only sets up a usable stack
        for system/user, SWI and IRQ modes.   Also each mode is setup with
        interrupts initially disabled. */
        ldr   r0, .LC6
        msr   CPSR_c, #MODE_UND|I_BIT|F_BIT /* Undefined Instruction Mode */
        mov   sp, r0
        sub   r0, r0, #UND_STACK_SIZE
        msr   CPSR_c, #MODE_ABT|I_BIT|F_BIT /* Abort Mode */
        mov   sp, r0
        sub   r0, r0, #ABT_STACK_SIZE
        msr   CPSR_c, #MODE_FIQ|I_BIT|F_BIT /* FIQ Mode */
        mov   sp, r0
        sub   r0, r0, #FIQ_STACK_SIZE
        msr   CPSR_c, #MODE_IRQ|I_BIT|F_BIT /* IRQ Mode */
        mov   sp, r0
        sub   r0, r0, #IRQ_STACK_SIZE
        msr   CPSR_c, #MODE_SVC|I_BIT|F_BIT /* Supervisor Mode */
        mov   sp, r0
        sub   r0, r0, #SVC_STACK_SIZE
        msr   CPSR_c, #MODE_SYS|I_BIT|F_BIT /* System Mode */
        mov   sp, r0

        /* We want to start in supervisor mode.  Operation will switch to system
        mode when the first task starts. */
        msr   CPSR_c, #MODE_SVC|I_BIT|F_BIT

        /* Clear BSS. */

        mov     a2, #0                  /* Fill value */
        mov     fp, a2                  /* Null frame pointer */
        mov     r7, a2                  /* Null frame pointer for Thumb */

        ldr     r1, .LC1                /* Start of memory block */
        ldr     r3, .LC2                /* End of memory block */
        subs    r3, r3, r1      /* Length of block */
        beq     .end_clear_loop
        //b     .end_clear_loop
        mov     r2, #0

.clear_loop:
        str     r2, [r1], #4
        str     r2, [r1], #4
        str     r2, [r1], #4
        str     r2, [r1], #4
        subs    r3, r3, #16

/*
        strb    r2, [r1], #1
        subs    r3, r3, #1
*/
        bgt     .clear_loop

.end_clear_loop:



//-------------write uart data -----------------
/*
    ldr r1, =UART_DATA
    mov r2,#0x69	//'i'
    str r2,[r1]
    mov r2,#0xa
    str r2,[r1]
    mov r2,#0xd
    str r2,[r1]
*/

       	mov     r0, #0          /* no arguments  */
        mov     r1, #0          /* no argv either */

        bl      boot_main

endless_loop:
        b      endless_loop

        .align 0

        .LC1:
        .word   __boot_bss_beg__
        .LC2:
        .word   __boot_bss_end__
        .LC6:
        .word   __stack_end__
/*
        //------real uart------
        .LC_UART_PAD28_ADDR:
        .word   0xc0000340
        .LC_UART_PAD28_VAL:
        .word   0x10001008

        .LC_UART_PAD25_ADDR:
        .word   0xc0000338
        .LC_UART_PAD25_VAL:
        .word   0x1800002b

        .LC_UART_SEL0_ADDR:
        .word   0xc00003cc
        .LC_UART_SEL0_VAL:
        .word   0x2000

        .LC_UART_BAUDRATE_ADDR:
        .word   0xc0000c1c
        .LC_UART_BAUDRATE_VAL:
        .word   0xe

        .LC_UART_DATA:
        .word   0xc0000c00

        .LC_REMAP:
        .word   0xc0000004

        .SRAM_DBG_ADDR0:
        .word   0x2000000

        .SRAM_DBG_ADDR1:
        .word   0x2000004
*/
        /* Setup vector table.  Note that undf, pabt, dabt, fiq just execute
        a null loop. */

//-------------------------startup -------------------------
.section .startup1,"ax"
        .code 32
        .align 0
        bl _start
        .word 0x5e11aa11
_exception:
        ldr   pc, _swi                  /* SWI - _swi                   */
        ldr   pc, _pabt                 /* program abort - _pabt        */
        ldr   pc, _dabt                 /* data abort - _dabt           */
        nop                             /* reserved                     */
        ldr   pc, _irq                  /* IRQ - _irq                   */
        ldr   pc, _fiq                  /* FIQ - _fiq                   */


_undf:  .word __undf                    /* undefined                    */
_swi:   .word __swi       /* SWI                                */
_pabt:  .word __pabt                    /* program abort                */
_dabt:  .word __dabt                    /* data abort                   */
_irq:   .word __irq                     /* IRQ                          */
_fiq:   .word __fiq                     /* FIQ                          */

__undf: b     .                         /* undefined                    */
__swi:  b     .
__pabt: b     .                         /* program abort                */
__dabt: b     .                         /* data abort                   */
__irq:  b     do_irq                    /* IRQ                              */
__fiq:  b     .                         /* FIQ                          */

//-----------------related  src_start | des_start | des_end -----
.section .vars_second_boot,"ax"
        .code 32
        .align 0
        .word _tbeg_src 
        .word _tbeg_dst
        .word _tend_dst
        .word _dbeg_src 
        .word _dbeg_dst
        .word _dend_dst
		.word __bss_beg__ 
		.word __bss_end__ 
		.word __stack_end__
        .word 0x5e11aa11	/*magic number*/
		.word 0x00010001	/*version number*/
		.word UND_STACK_SIZE 
		.word ABT_STACK_SIZE
		.word FIQ_STACK_SIZE
		.word IRQ_STACK_SIZE
		.word SVC_STACK_SIZE
		.word _flash_size_
		.word _ram_size_

//-------------------------startup clone-------------------------
.section .startup_clone,"ax"
        .code 32
        .align 0
        bl _start
        .word 0x5e11aa11
_exceptionx:
        ldr   pc, _swix                  /* SWI - _swi                   */
        ldr   pc, _pabtx                 /* program abort - _pabt        */
        ldr   pc, _dabtx                 /* data abort - _dabt           */
        nop                             /* reserved                     */
        ldr   pc, _irqx                  /* IRQ - _irq                   */
        ldr   pc, _fiqx                  /* FIQ - _fiq                   */

_undfx:  .word __undfx                    /* undefined                    */
_swix:   .word __swix       /* SWI                                */
_pabtx:  .word __pabtx                    /* program abort                */
_dabtx:  .word __dabtx                    /* data abort                   */
_irqx:   .word __irqx                     /* IRQ                          */
_fiqx:   .word __fiqx                     /* FIQ                          */

__undfx: b     .                         /* undefined                    */
__swix:  b     .
__pabtx: b     .                         /* program abort                */
__dabtx: b     .                         /* data abort                   */
__irqx:  b     do_irq                    /* IRQ                              */
__fiqx:  b     .                         /* FIQ                          */

//---------------------------TEMP START-------------------------------------
//      special debug address:0xc3001008
//        ldr r1, =.LC_USR_PRINTF_LEN
//        ldr r2, =.LC_USR_PRINTF_STR

//       mov r3,#0x4
//       str r3,[r1]
        //"A","B","C","D"
//        mov r3,#0x41
//       str r3,[r2]
//        mov r3,#0x42
//        str r3,[r2]
//        mov r3,#0x43
//        str r3,[r2]
//        mov r3,#0x44
//        str r3,[r2]

//-------------setup uart start-----------------
//    ldr r1, .LC_UART_PAD28_ADDR
//    ldr r2, .LC_UART_PAD28_VAL
//    str r2, [r1]
//
//    ldr r1, .LC_UART_PAD25_ADDR
//    ldr r2, .LC_UART_PAD25_VAL
//    str r2, [r1]
//
//    ldr r1, .LC_UART_SEL0_ADDR
//    ldr r2, .LC_UART_SEL0_VAL
//    str r2, [r1]
//
//    ldr r1, .LC_UART_BAUDRATE_ADDR
//    ldr r2, .LC_UART_BAUDRATE_VAL
//    str r2, [r1]


//-------------write uart data -----------------
//    ldr r1, .LC_UART_DATA
//
//    mov r2,#0x61
//    str r2,[r1]
//    mov r2,#0x63
//    str r2,[r1]
//
//    mov r2,#0xa
//    str r2,[r1]
//
//    mov r2,#0xd
//    str r2,[r1]

//---------------------------TEMP   END-------------------------------------
