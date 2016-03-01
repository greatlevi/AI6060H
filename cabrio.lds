
MEMORY
{
        flash          (rx)   : ORIGIN = 0x3000000, LENGTH = 512K
        ram           (rwx)   : ORIGIN = 0x2000000, LENGTH = 192K
}

/*__stack_end__ = 0x02000000 + 160K - 4;*/
_flash_size_ = LENGTH(flash); 
_ram_size_ = LENGTH(ram); 
__stack_end__ = ORIGIN(ram)+ LENGTH(ram) - 4;
_boot_size = 0x4000;	/*8KB:boot 4KB: wifi-config 4KB:feature_config*/

SECTIONS
{
/*        . = 0x3000000;*/
        . = ORIGIN(flash);

	_sitxt0 = .;
        startup : AT(_sitxt0)
        {
        *(.startup1)
        *(.startup2)
/*		icomlib.a:boot-hiflying.o (.text .rodata .rodata*) */
		. = ALIGN(16);
        } > flash

		_vma_boot_text_beg = ORIGIN(ram) + LENGTH(ram) - 28K;
		_sitxt0a = _sitxt0 + SIZEOF(startup); 


    . = ORIGIN(flash) + _boot_size;
	_sitxt2 = .;

    prog_in_flash _sitxt2 : AT( _sitxt2)
        {
				*(.vars_second_boot)
				icomlib.a:contiki-main.o (.text.startup.main )
/* *(.text*) *(.rodata) *(.rodata*) */
				*(EXCLUDE_FILE (icomlib.a:boot-hiflying.o icomlib.a:AES_decrypt.o contikilib.a:uip.o icomlib.a:timer.o icomlib.a:clock.o icomlib.a:etimer.o contikilib.a:tcpip.o icomlib.a:ieee80211_net.o icomlib.a:ieee80211_rdc.o icomlib.a:packetbuf.o icomlib.a:drv_pktbuf.o icomlib.a:ssvradio.o icomlib.a:ssv6200_rx.o icomlib.a:ssv6200_tx.o icomlib.a:throughput.o icomlib.a:rate_control.o icomlib.a:dbg-printf.o icomlib.a:strformat.o icomlib.a:dbg-puts.o  icomlib.a:debug-uart.o icomlib.a:soft_ap.o icomlib.a:xmem.o icomlib.a:drv_flash.o icomlib.a:flash_api.o icomlib.a:hw_regs_api.o icomlib.a:powersave.o contikilib.a:socket_driver.o icomlib.a:pstm.o icomlib.a:pstmmonreduce.o icomlib.a:pstmmulcomba.o icomlib.a:pstmsqrcomba.o icomlib.a:asn1.o icomlib.a:x905.o icomlib.a:sha512.o icomlib.a:sha384.o icomlib.a:sha256.o icomlib.a:ssv_lib.o) .text* )
				*(EXCLUDE_FILE (icomlib.a:boot-hiflying.o icomlib.a:AES_decrypt.o contikilib.a:uip.o icomlib.a:timer.o icomlib.a:clock.o icomlib.a:etimer.o contikilib.a:tcpip.o icomlib.a:ieee80211_net.o icomlib.a:ieee80211_rdc.o icomlib.a:packetbuf.o icomlib.a:drv_pktbuf.o icomlib.a:ssvradio.o icomlib.a:ssv6200_rx.o icomlib.a:ssv6200_tx.o icomlib.a:throughput.o icomlib.a:rate_control.o icomlib.a:dbg-printf.o icomlib.a:strformat.o icomlib.a:dbg-puts.o  icomlib.a:debug-uart.o icomlib.a:soft_ap.o icomlib.a:xmem.o icomlib.a:drv_flash.o icomlib.a:flash_api.o icomlib.a:hw_regs_api.o icomlib.a:powersave.o contikilib.a:socket_driver.o icomlib.a:pstm.o icomlib.a:pstmmonreduce.o icomlib.a:pstmmulcomba.o icomlib.a:pstmsqrcomba.o icomlib.a:asn1.o icomlib.a:x905.o icomlib.a:sha512.o icomlib.a:sha384.o icomlib.a:sha256.o icomlib.a:ssv_lib.o) .text.* )
				*(EXCLUDE_FILE (icomlib.a:boot-hiflying.o icomlib.a:AES_decrypt.o contikilib.a:uip.o icomlib.a:timer.o icomlib.a:clock.o icomlib.a:etimer.o contikilib.a:tcpip.o icomlib.a:ieee80211_net.o icomlib.a:ieee80211_rdc.o icomlib.a:packetbuf.o icomlib.a:drv_pktbuf.o icomlib.a:ssvradio.o icomlib.a:ssv6200_rx.o icomlib.a:ssv6200_tx.o icomlib.a:throughput.o icomlib.a:rate_control.o icomlib.a:dbg-printf.o icomlib.a:strformat.o icomlib.a:dbg-puts.o  icomlib.a:debug-uart.o icomlib.a:soft_ap.o icomlib.a:xmem.o icomlib.a:drv_flash.o icomlib.a:flash_api.o icomlib.a:hw_regs_api.o icomlib.a:powersave.o contikilib.a:socket_driver.o icomlib.a:pstm.o icomlib.a:pstmmonreduce.o icomlib.a:pstmmulcomba.o icomlib.a:pstmsqrcomba.o icomlib.a:asn1.o icomlib.a:x905.o icomlib.a:sha512.o icomlib.a:sha384.o icomlib.a:sha256.o icomlib.a:ssv_lib.o) .rodata )
				*(EXCLUDE_FILE (icomlib.a:boot-hiflying.o icomlib.a:AES_decrypt.o contikilib.a:uip.o icomlib.a:timer.o icomlib.a:clock.o icomlib.a:etimer.o contikilib.a:tcpip.o icomlib.a:ieee80211_net.o icomlib.a:ieee80211_rdc.o icomlib.a:packetbuf.o icomlib.a:drv_pktbuf.o icomlib.a:ssvradio.o icomlib.a:ssv6200_rx.o icomlib.a:ssv6200_tx.o icomlib.a:throughput.o icomlib.a:rate_control.o icomlib.a:dbg-printf.o icomlib.a:strformat.o icomlib.a:dbg-puts.o  icomlib.a:debug-uart.o icomlib.a:soft_ap.o icomlib.a:xmem.o icomlib.a:drv_flash.o icomlib.a:flash_api.o icomlib.a:hw_regs_api.o icomlib.a:powersave.o contikilib.a:socket_driver.o icomlib.a:pstm.o icomlib.a:pstmmonreduce.o icomlib.a:pstmmulcomba.o icomlib.a:pstmsqrcomba.o icomlib.a:asn1.o icomlib.a:x905.o icomlib.a:sha512.o icomlib.a:sha384.o icomlib.a:sha256.o icomlib.a:ssv_lib.o) .rodata* )
                *(.glue_7)
                *(.glue_7t)
                *(.ARM.extab)
				*(.gnu.linkonce.armextab.*)
                *(.vfp11_veneer)
                *(.v4_bx)
                *(.iplt)
                *(.rel.dyn)
                *(.rel.iplt)
/*		*(ARM.attributes) */
/* 		*(.ARM.attributes) */
		. = ALIGN(4);
        } > flash

		prog_in_sram : AT( _sitxt3)
        {
            *(.startup_clone)
       		*(keep_in_sram)
			icomlib.a:AES_decrypt.o (.text .text* .rodata .rodata*)
            contikilib.a:uip.o (.text .text* .rodata .rodata*)
			icomlib.a:timer.o (.text .text* .rodata .rodata*)
			icomlib.a:clock.o (.text .text* .rodata .rodata*)
			icomlib.a:etimer.o (.text .text* .rodata .rodata*)
			contikilib.a:tcpip.o (.text .text* .rodata .rodata*)
			icomlib.a:ieee80211_net.o (.text .text* .rodata .rodata*)
			icomlib.a:ieee80211_rdc.o (.text .text* .rodata .rodata*)
			icomlib.a:packetbuf.o (.text .text* .rodata .rodata*)
			icomlib.a:drv_pktbuf.o (.text .text* .rodata .rodata*)
			icomlib.a:ssvradio.o (.text .text* .rodata .rodata*)
			icomlib.a:ssv6200_rx.o (.text .text* .rodata .rodata*)
			icomlib.a:ssv6200_tx.o (.text .text* .rodata .rodata*)
			icomlib.a:throughput.o (.text .text* .rodata .rodata*)
			icomlib.a:rate_control.o (.text .text* .rodata .rodata*)
			icomlib.a:dbg-printf.o (.text .text* .rodata .rodata*)
			icomlib.a:strformat.o (.text .text* .rodata .rodata*)
			icomlib.a:dbg-puts.o (.text .text* .rodata .rodata*)
			icomlib.a:debug-uart.o (.text .text* .rodata .rodata*)
			icomlib.a:soft_ap.o (.text .text* .rodata .rodata*)
			icomlib.a:xmem.o (.text .text* .rodata .rodata*)
			icomlib.a:drv_flash.o (.text .text* .rodata .rodata*)
			icomlib.a:flash_api.o (.text .text* .rodata .rodata*)
			icomlib.a:hw_regs_api.o (.text .text* .rodata .rodata*)
			icomlib.a:powersave.o (.text .text* .rodata .rodata*)
			contikilib.a:socket_driver.o (.text .text* .rodata .rodata*)
			icomlib.a:pstm.o (.text .text* .rodata .rodata*)
			icomlib.a:pstmmonreduce.o (.text .text* .rodata .rodata*)
			icomlib.a:pstmmulcomba.o (.text .text* .rodata .rodata*)
			icomlib.a:pstmsqrcomba.o (.text .text* .rodata .rodata*)
			icomlib.a:asn1.o (.text .text* .rodata .rodata*)
			icomlib.a:x905.o (.text .text* .rodata .rodata*)
 			icomlib.a:sha512.o (.text .text* .rodata .rodata*)
			icomlib.a:sha384.o (.text .text* .rodata .rodata*)
			icomlib.a:sha256.o (.text .text* .rodata .rodata*)
			icomlib.a:ssv_lib.o (.text .text* .rodata .rodata*)
   		*(.text.process_thread*)
            . = ALIGN(4);
		} > ram


  _tbeg_dst = ORIGIN(ram); 
	

		_sitxt3 = LOADADDR(prog_in_flash) + SIZEOF(prog_in_flash);


	_tbeg_src = LOADADDR(prog_in_sram);
	_tend_dst = _tbeg_dst + SIZEOF(prog_in_sram);


/*
	if .ARM.exidx removed,there will error message like: "section .ARM.exidx 
	loaded at [000xxxx,0001xxxxx] overlaps section .data loaded at [000xxxxx,000xxxxx]"
*/

	.ARM : 
	{
		*(.ARM.exidx*)	
		_tend_dst = .;
	} > ram

        .data : 
        {
		_dbeg_dst = .;
		*(EXCLUDE_FILE (icomlib.a:boot-hiflying.o) .data)
		*(EXCLUDE_FILE (icomlib.a:boot-hiflying.o) .data*) _dend_dst = .; 
		} >ram 
		_dbeg_src = LOADADDR(.data);


        .bss ALIGN(16):
        {
         	. = ALIGN(16);
                __bss_beg__ = .;
				*(EXCLUDE_FILE (icomlib.a:boot-hiflying.o) .bss )
				*(EXCLUDE_FILE (icomlib.a:boot-hiflying.o) .bss*)
				*(EXCLUDE_FILE (icomlib.a:boot-hiflying.o) COMMON)
        		__bss_end__ = . ;
                . = ALIGN(16);
        } >ram


	_btext_beg_src = LOADADDR(boot_in_sram);
	_btext_beg_dst = _vma_boot_text_beg; 
	_btext_end_dst =  _btext_beg_dst + SIZEOF(boot_in_sram);

		boot_in_sram _vma_boot_text_beg : AT(_sitxt0a)
        {
            . = ALIGN(4);
/*		icomlib.a:boot-hiflying.o (.text .text.* .rodata .rodata*) */
		icomlib.a:boot-hiflying.o (.text )
		icomlib.a:boot-hiflying.o (.text.* )
		icomlib.a:boot-hiflying.o (.rodata )
		icomlib.a:boot-hiflying.o (.rodata* )
            . = ALIGN(4);
		} > ram

		_sitxt1 = _sitxt0a + SIZEOF(boot_in_sram);


/*		_vma_boot_data_beg = 0x2000000 + 160K - 16K;  */ 
		_vma_boot_data_beg = ORIGIN(ram) + LENGTH(ram) - 16K;


        .boot_data _vma_boot_data_beg : AT( _sitxt1) 
        {
			_boot_dbeg_dst = .;
			icomlib.a:boot-hiflying.o (.data .data*)
			. = ALIGN(16);
			_boot_dend_dst = .;
        } >ram
		_boot_dbeg_src = LOADADDR(.boot_data);

		_vma_boot_bss_beg =_vma_boot_data_beg - 16K;   
        .boot_bss :
        {
			__boot_bss_beg__ = .;
			icomlib.a:boot-hiflying.o (.bss .bss* COMMON)
			__boot_bss_end__ = .;
        } >ram


/*
		_boot_dbeg_src = LOADADDR(.boot_data);
		_boot_dbeg_dst =  __boot_bss_end__ - SIZEOF(.boot_data);
*/


        . = ALIGN(4);
        .eh_frame :
        {
                 KEEP (*(.eh_frame))
        } > ram

        /* Align here to ensure that the .bss section occupies space up to
        _end.  Align after .bss to ensure correct alignment even if the
        .bss section disappears because there are no input sections.  */
        . = ALIGN(32 / 8);
        . = ALIGN(32 / 8);

        _end = .;

		 __end__ = . ;


/*TOTAL_STACK_SIZE comes from boot.s*/
/*__heap_start__ = __bss_end__ + 0x4000;*/
__heap_start__ = __bss_end__;
__heap_end__   = ORIGIN(ram)+ LENGTH(ram) - TOTAL_STACK_SIZE;

/*
	.ARM.attributes : 
	{ 
		*(.ARM.attributes) 
	} > ram
*/

	.ARM.attributes 0 : { *(.ARM.attributes) *(ARM.attributes) }

	
        PROVIDE (end = .);
}

