include ../Makefile.mk


# -nostdlib -falign-functions=4 -falign-jumps -falign-loops -falign-labels
DBGFLAGS   = -g
OPTFLAGS   = -Os -fomit-frame-pointer
INCLUDE    = -I./ -I./include -I./include/net -I./include/bsp -I./include/lib -I./include/net/ieee80211_bss -I./include/sys -I./include/net/mac -I./include/dev -I./include/matrixssl -I../include

CPPFLAGS  += $(INCLUDE) $(OPTFLAGS) $(DBGFLAGS)
ARCH_FLAGS = -march=armv3m -mno-thumb-interwork 
CFLAGS    += -Wall -Wno-trigraphs -fno-builtin $(CPPFLAGS) $(ARCH_FLAGS) -fdata-sections -ffunction-sections -DMODULE_ID=$(MODULE_ID) -DFUNC_OTA_ON=$(FUNC_OTA_ON) -DBOOT_SECTOR_UPDATE=$(BOOT_SECTOR_UPDATE) -DDISABLE_ICOMM_DISCOVER=$(DISABLE_ICOMM_DISCOVER) -Werror=implicit-function-declaration -Werror=maybe-uninitialized
#CFLAGS += -DAUTOSTART_ENABLE


CLEAN += %.elf $(CONTIKI_PROJECT).elf

### Compilation rules

# Don't treat %.elf %.bin as an imtermediate file!
.PRECIOUS: %.elf %.bin

OS_SRC = \
os/autostart.c \
os/etimer.c \
os/printf.c \
os/process.c \
os/shell-ping.c \
os/strformat.c \
os/random.c \
os/timer.c

PLATFORM_SRC = \
platform/airkiss.c \
platform/atcmd_icomm.c \
platform/boot-hiflying.c \
platform/bsp_info.c \
platform/cfs-coffee.c \
platform/clock.c \
platform/console.c \
platform/contiki-main.c \
platform/debug-uart.c \
platform/drv_fence.c \
platform/drv_flash.c \
platform/drv_gpio.c \
platform/gpio_api.c \
platform/drv_pmu.c \
platform/drv_pwm.c \
platform/pwm_api.c \
platform/drv_timer.c \
platform/drv_uart.c \
platform/serial_api.c \
platform/drv_wdog.c \
platform/i2c.c \
platform/fence.c \
platform/flash_api.c \
platform/hw_init.c \
platform/hw_regs_api.c \
platform/smartComm.c \
platform/smartConf.c \
platform/ssv_lib.c \
platform/ssv_lib_slow.c \
platform/swpwm.c \
platform/systemconf.c \
platform/userSmart.c \
platform/wdog_api.c \
platform/time.c \
platform/xmem.c

WIFI_SRC = \
wifi/bsp.c \
wifi/crypthw.c \
wifi/dbg-atcmd-api.c \
wifi/drv_mac.c \
wifi/drv_mbox.c \
wifi/drv_phy.c \
wifi/drv_pktbuf.c \
wifi/drv_pktbuf_slow.c \
wifi/EAPOL.c \
wifi/hash_alg.c \
wifi/hmac_md5.c \
wifi/hmac_sha1.c \
wifi/ieee80211_mac.c \
wifi/ieee80211_mgmt.c \
wifi/ieee80211_net.c \
wifi/ieee80211_rdc.c \
wifi/irq.c \
wifi/netstack.c \
wifi/powersave.c \
wifi/rate_control.c \
wifi/sha1.c \
wifi/soft_ap.c \
wifi/ssv6200_rx.c \
wifi/ssv6200_tx.c \
wifi/ssvradio.c \
wifi/wpa.c

ENCRYPT_SW_SRC = \
encrypt_sw/AES_decrypt.c \
encrypt_sw/arc4.c \
encrypt_sw/md5.c \
encrypt_sw/wep.c \

MATRIXSSL_SRC:=\
matrixssl/osdep.c \
matrixssl/corelib.c \
matrixssl/prng.c \
matrixssl/yarrow.c \
matrixssl/aes.c \
matrixssl/aesGCM.c \
matrixssl/des3.c \
matrixssl/seed.c \
matrixssl/arc4.c \
matrixssl/rc2.c \
matrixssl/idea.c \
matrixssl/sha1ssl.c \
matrixssl/sha256.c \
matrixssl/sha224.c \
matrixssl/sha384.c \
matrixssl/sha512.c \
matrixssl/md4.c \
matrixssl/md2.c \
matrixssl/hmac.c \
matrixssl/aesni.c \
matrixssl/pstm.c \
matrixssl/pstmmonreduce.c \
matrixssl/pstmsqrcomba.c \
matrixssl/pstmmulcomba.c \
matrixssl/pubkey.c \
matrixssl/rsa.c \
matrixssl/ecc.c \
matrixssl/dh.c \
matrixssl/pkcs.c \
matrixssl/asn1.c \
matrixssl/base64.c \
matrixssl/x509.c \
matrixssl/prf.c \
matrixssl/sslv3.c \
matrixssl/tls.c \
matrixssl/dtls.c \
matrixssl/psk.c \
matrixssl/cipherSuite.c \
matrixssl/hsHash.c \
matrixssl/matrixssl.c \
matrixssl/matrixsslApi.c \
matrixssl/sslDecode.c \
matrixssl/sslEncode.c

EXPORT_API = \
export_api/sys_status_api.c \
export_api/ssl_api.c

OBJECTDIR=objs
CONTIKI_SRC = $(ENCRYPT_SW_SRC) $(OS_SRC) $(PLATFORM_SRC) $(WIFI_SRC) $(EXPORT_API) $(MATRIXSSL_SRC)
CONTIKI_OBJS=${addprefix $(OBJECTDIR)/,$(CONTIKI_SRC:.c=.o) $(CONTIKI_SRC:.c=.o)}

all:
	@mkdir -p objs
	@mkdir -p objs/encrypt_sw
	@mkdir -p objs/os 
	@mkdir -p objs/platform
	@mkdir -p objs/wifi
	@mkdir -p objs/matrixssl
	@mkdir -p objs/export_api
	make icomlib.a

icomlib.a: $(CONTIKI_OBJS)
	echo "$(AR) $(AROPTS) $@ $^"
	$(AR) $(AROPTS) $@ $^

print:
	echo "CONTIKI_SRC=$(CONTIKI_SRC)"
	echo "CONTIKI_OBJS=$(CONTIKI_OBJS)"

#%.o: %.c
#	@#echo "-----------------make z0 target:$@ $(makefn3)-----------------"
#	@#echo "$(CC) $(CFLAGS) $< -c"
#	@#echo "$< $@"
#	#$(CC) $(CFLAGS) $< -c -o $(OBJECTDIR)/$@
#	$(CC) $(CFLAGS) $< -c -o objs/$@

$(OBJECTDIR)/%.o: %.c | $(OBJECTDIR)
	@echo "-----------------make z1 target:$@ $(makefn3)-----------------"
	$(CC) $(CFLAGS) -c $< -o $@

clean:

