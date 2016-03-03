include ../Makefile.mk


# -nostdlib -falign-functions=4 -falign-jumps -falign-loops -falign-labels
DBGFLAGS   = -g
OPTFLAGS   = -Os -fomit-frame-pointer
INCLUDE    = -I. -I./AC/inc -I./ZC/inc/aes -I./ZC/inc/tropicssl -I./ZC/inc/zc -I../icomlib/include/sys
INCLUDE    += -I../include -I../icomlib/include -I../icomlib/include/dev -I../icomlib/include/net -I../icomlib/include/net/mac -I../icomlib/include/net/ieee80211_bss

CPPFLAGS  += $(INCLUDE) $(OPTFLAGS) $(DBGFLAGS)
ARCH_FLAGS = -march=armv3m -mno-thumb-interwork 
CFLAGS    += -Wall -Wno-trigraphs -fno-builtin $(CPPFLAGS) $(ARCH_FLAGS) -fdata-sections -ffunction-sections -DMODULE_ID=$(MODULE_ID) -DFUNC_OTA_ON=$(FUNC_OTA_ON) -DBOOT_SECTOR_UPDATE=$(BOOT_SECTOR_UPDATE) -DDISABLE_ICOMM_DISCOVER=$(DISABLE_ICOMM_DISCOVER) -Werror=implicit-function-declaration -Werror=maybe-uninitialized
#CFLAGS += -DAUTOSTART_ENABLE
CFLAGS += -DZC_MODULE_VERSION=0
CFLAGS += -DZC_MODULE_TYPE=0


CLEAN += %.elf $(CONTIKI_PROJECT).elf

### Compilation rules

# Don't treat %.elf %.bin as an imtermediate file!
.PRECIOUS: %.elf %.bin

OBJECTDIR=objs

CONTIKI_SRC = \
AC/src/ac_api.c \
AC/src/ac_hal.c \
ZC/src/aes/aes_cbc.c \
ZC/src/aes/aes_core.c \
ZC/src/crc/crc.c \
ZC/src/tropicssl/bignum.c \
ZC/src/tropicssl/rsa.c \
ZC/src/zc/zc_bc.c \
ZC/src/zc/zc_client_manager.c \
ZC/src/zc/zc_cloud_event.c \
ZC/src/zc/zc_common.c \
ZC/src/zc/zc_configuration.c \
ZC/src/zc/zc_message_queue.c \
ZC/src/zc/zc_moudle_manager.c \
ZC/src/zc/zc_protocol_controller.c \
ZC/src/zc/zc_sec_engine.c \
ZC/src/zc/zc_timer.c \
zc_hf_adpter.c

CONTIKI_OBJS=${addprefix $(OBJECTDIR)/,$(CONTIKI_SRC:.c=.o) $(CONTIKI_SRC:.c=.o)}

all:
	@mkdir -p objs/AC/src
	@mkdir -p objs/ZC/src/aes
	@mkdir -p objs/ZC/src/crc
	@mkdir -p objs/ZC/src/tropicssl
	@mkdir -p objs/ZC/src/zc	
	make accloud.a 

accloud.a: $(CONTIKI_OBJS)
	echo "$(AR) $(AROPTS) $@ $^"
	$(AR) $(AROPTS) $@ $^

print:
	echo "CONTIKI_SRC=$(CONTIKI_SRC)"
	echo "CONTIKI_OBJS=$(CONTIKI_OBJS)"

$(OBJECTDIR)/%.o: %.c | $(OBJECTDIR)
	@echo "-----------------make z1 target:$@ $(makefn3)-----------------"
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	/bin/rm -f *.o *.elf *.a *.bin *.asm 
	/bin/rm -rf $(OBJECTDIR)

