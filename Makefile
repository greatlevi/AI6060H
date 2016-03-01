### DEfine the CPU directory
include Makefile.mk

MAC_BIN = ssv6060_mac.hex
CONTIKI_PROJECT=ssv6060-main
CONTIKI_SRC=$(CONTIKI_PROJECT).c    ###You can add other source code here!!! ###
#CONTIKI_SRC=$(CONTIKI_PROJECT).c test1.c
CONTIKI_OBJ=$(patsubst %.c,%.o,$(CONTIKI_SRC))



TARGET=cabrio
CRT0 = boot.s
FLASH_LAYOUT=flash_layout_boot_8k
ADD_LIB=-L ./thirdPartyLib -lairkiss

CROSS_COMPILE ?= arm-none-eabi-
CROSS ?= $(CROSS_COMPILE)

### Compiler definitions
CC       = $(CROSS)gcc
LD       = $(CROSS)gcc
AS       = $(CROSS)as
NM       = $(CROSS)nm
OBJCOPY  = $(CROSS)objcopy
STRIP    = $(CROSS)strip
OBJDUMP  = $(CROSS)objdump

# -nostdlib -falign-functions=4 -falign-jumps -falign-loops -falign-labels
DBGFLAGS   = -g
OPTFLAGS   = -Os -fomit-frame-pointer
INCLUDE    = -I./icomlib/include -I./icomlib/include/net -I./icomlib/include/bsp -I./icomlib/include/net/ieee80211_bss -I./icomlib/include/sys -I./icomlib/include/net/mac -I./icomlib/include/dev  -I./icomlib/include/matrixssl -I./include

CPPFLAGS  += $(INCLUDE) $(OPTFLAGS) $(DBGFLAGS)
ARCH_FLAGS = -march=armv3m -mno-thumb-interwork 
CFLAGS    += -Wall -Wno-trigraphs -fno-builtin $(CPPFLAGS) $(ARCH_FLAGS) -fdata-sections -ffunction-sections -DMODULE_ID=$(MODULE_ID) -DBOOT_SECTOR_UPDATE=$(BOOT_SECTOR_UPDATE) -DDISABLE_ICOMM_DISCOVER=$(DISABLE_ICOMM_DISCOVER) -Werror=implicit-function-declaration -Werror=maybe-uninitialized
CFLAGS += -DAUTOSTART_ENABLE


LDFLAGS += --specs=nosys.specs --specs=nano.specs -flto -Wl,-T$(LINKERSCRIPT) 
LDFLAGS += -flto
LDFLAGS += -Xlinker -M -Xlinker -Map=contiki-$(TARGET).map -nostartfiles $(CRT0)
#LDFLAGS += -Wl,--gc-sections

AROPTS = rcf

CLEAN += %.elf $(CONTIKI_PROJECT).elf

### Compilation rules

# Don't treat %.elf %.bin as an imtermediate file!
.PRECIOUS: %.elf %.bin

all:
	$(MAKE) -C icomlib
	$(MAKE) -C icomapps
	$(MAKE) -C contikilib
	make ssv6060-main.bin

#%.bin:%.elf
ssv6060-main.bin: ssv6060-main.elf
		@echo "-----------------make 3 target:$@ 1st-dep:$< $(makefn3)-----------------"
		@test -s $(MAC_BIN) || { printf "\n\n$(MAC_BIN)(MAC address embedded inside) does not exist or file size=0 !!!\n";echo "###Please use ./util/gen_mac_bin.exe to generate it once ###\n"; exit 1; }
		@test `stat -c %s $(MAC_BIN)` -eq 8192 || { echo "Please generate $(MAC_BIN),and file size must be 8192(Bytes) "; exit 1; }
		$(OBJCOPY) -O binary $< $@
		@echo -e "\n-----Check bin file size:($@)----->>>"
		test `stat -c %s $@` -lt 384000	###acceptable if < 384KB### 
		@echo "-----Done:bin file size:ok-----<<<"	
		@echo "-----Generate bin for uart-upgrade-----"
		dd skip=$(BOOT_CODE_AND_CONFIG_SIZE) if=$@ of=$(CONTIKI_PROJECT)_os.bin bs=1
		dd skip=0 count=$(BOOT_CODE_SIZE) if=$(CONTIKI_PROJECT).bin of=boot_tmp.bin bs=1
		cat boot_tmp.bin $(MAC_BIN) $(CONTIKI_PROJECT)_os.bin > $(CONTIKI_PROJECT).bin

		@/bin/rm -rf boot_tmp.bin $(CONTIKI_PROJECT)_os.bin dump.log 

		@arm-none-eabi-objdump -h $(CONTIKI_PROJECT).elf > dump.log 
		@chmod +x ./util/*.pl; 
		@./util/rpt_sram_usage.pl dump.log
		@printf "\n\n"; 
		@ls -al *.bin *.elf
		@make layout #show flash layout
		@#make chksum #add checksum to binfile


		@echo "#############################################################"
		@echo "### You can use 'make asm' to generate dis-assembly file!!###"
		@echo "#############################################################"

clean:
	/bin/rm -f *.o *.elf
	/bin/rm -f icomlib.a icomapps.a contikilib.a *.bin *.asm *.map
	cd icomlib && make clean
	cd icomapps && make clean
	cd contikilib && make clean

#ssv6060-main.o:ssv6060-main.c
#	$(CC) $(CFLAGS) -DAUTOSTART_ENABLE -c ssv6060-main.c -o ssv6060-main.o

%.o: %.c
		@echo "-----------------make z0 target:$@ $(makefn3)-----------------"
		$(CC) $(CFLAGS) $< -c

asm:
	$(OBJDUMP) -D -S ssv6060-main.elf > ssv6060-main.asm

layout:
		@cd ./util; make $(FLASH_LAYOUT); 

chksum:
		@../util/gen_checksum $(MODULE_ID) $(join $(CONTIKI_PROJECT), .bin); 
		
chk:
		@./util/chk_code_size.pl contiki-cabrio.map
sramf:
		@./util/rpt_in_sram_function.pl contiki-cabrio.map

version:
#ifneq ($(IS_SVN_CHECKED_OUT),yes)
#    $(error "<Error>Cannot get svn information. Please svn commit first. Or you won't get svn information in this directory.")
#endif
		@svn info | grep "Revision" | awk '{printf "#define SVN_VERSION \".%s\"\n",$$2}' > include/svn_ver.h
#		@date +%y%m%d | awk '{printf "#define DATE_VERSION \".%s\"\n",$$1}' >> svn_ver.h
		
warn:
		@rm -f compile.log;rm -f warn.log.txt
		@make clean
		@make -j2 > compile.log 2>&1  #due to ubunu's dash shell 
		@printf "\n\n######## Please check warn.log.txt ########\n\n";
		@./util/chk_warning.pl	compile.log > warn.log.txt  

print:
		echo $(CONTIKI_OBJ)

ssv6060-main.elf: $(CONTIKI_OBJ) icomlib/icomlib.a icomapps/icomapps.a contikilib/contikilib.a $(LINKERSCRIPT)
	# WARNING: if you need math.h. You need to add -lm into this linking stage.Please reference where -lm being added 
	#$(CC) $(LDFLAGS) $(CFLAGS) $(filter-out %.a %.lds,$^) $(filter %.a,$^) -lc $(filter %.a,$^) -lm -o $@ $(ADD_LIB)
	/bin/cp icomlib/icomlib.a . ### use cp to copy library or the linker script will have problem for hierachy issue ###
	/bin/cp icomapps/icomapps.a . ### use cp to copy library or the linker script will have problem for hierachy issue ###
	/bin/cp contikilib/contikilib.a . ### use cp to copy library or the linker script will have problem for hierachy issue ###
	$(CC) $(LDFLAGS) $(CFLAGS) $(CONTIKI_OBJ) icomlib.a icomapps.a icomlib.a contikilib.a -o ssv6060-main.elf -L ./thirdPartyLib -lairkiss 

