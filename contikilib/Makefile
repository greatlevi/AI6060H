include ../Makefile.mk


# -nostdlib -falign-functions=4 -falign-jumps -falign-loops -falign-labels
DBGFLAGS   = -g
OPTFLAGS   = -Os -fomit-frame-pointer
INCLUDE    = -I../include -I../icomlib/include/net -I../icomlib/include/bsp -I../icomlib/include/net/ieee80211_bss -I../icomlib/include/sys -I../icomlib/include/net/mac -I../icomlib/include/dev -I../icomlib/include
#INCLUDE    = -I../include -I../icomlib/include -I../icomlib/include/dev -I../icomlib/include/net -I../icomlib/include/net/ieee80211_bss

CPPFLAGS  += $(INCLUDE) $(OPTFLAGS) $(DBGFLAGS)
ARCH_FLAGS = -march=armv3m -mno-thumb-interwork 
CFLAGS    += -Wall -Wno-trigraphs -fno-builtin $(CPPFLAGS) $(ARCH_FLAGS) -fdata-sections -ffunction-sections -DMODULE_ID=$(MODULE_ID) -DBOOT_SECTOR_UPDATE=$(BOOT_SECTOR_UPDATE) -DDISABLE_ICOMM_DISCOVER=$(DISABLE_ICOMM_DISCOVER) -Werror=implicit-function-declaration -Werror=maybe-uninitialized
#CFLAGS += -DAUTOSTART_ENABLE


CLEAN += %.elf $(CONTIKI_PROJECT).elf

### Compilation rules

# Don't treat %.elf %.bin as an imtermediate file!
.PRECIOUS: %.elf %.bin

#TCPIP_SRC = \
#tcpip/tcpip.c \

#OBJECTDIR=objs
#CONTIKI_SRC = $(TCPIP_SRC)
#CONTIKI_OBJS=${addprefix $(OBJECTDIR)/,$(CONTIKI_SRC:.c=.o) $(CONTIKI_SRC:.c=.o)}

OBJECTDIR=objs

CONTIKI_SRC = \
dhcpc.c \
dhcp_process.c \
dhcps.c \
resolv.c \
simple-udp.c \
socket_driver.c \
tcpip.c \
uip_arp.c \
uip.c \
uiplib.c \
uip-udp-packet.c \
ntp_client.c

#ssid_recv.c ### To-DO ###

CONTIKI_OBJS=${addprefix $(OBJECTDIR)/,$(CONTIKI_SRC:.c=.o) $(CONTIKI_SRC:.c=.o)}


all:
	@mkdir -p objs
	make contikilib.a 

contikilib.a: $(CONTIKI_OBJS)
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


	
	
	
	
	
	