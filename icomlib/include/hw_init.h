#ifndef _HW_CTRL_API_H_
#define _HW_CTRL_API_H_

#include <hw_regs_api.h>
#include <ssv_types.h>

#define DEFAULT_CHANNEL		9

#define SSV6XXX_SET_HW_TABLE(tbl_)                                 \
({                                                                 \
    int ret = 0;                                                   \
    U32 i=0;                                                       \
    for(; i<sizeof(tbl_)/sizeof(ssv6xxx_dev_table); i++) {         \
        ret = SMAC_REG_WRITE(tbl_[i].address, tbl_[i].data);       \
        if (ret) break;                                            \
    }                                                              \
    ret;                                                           \
})

int ssv6xxx_hw_rf_phy_mac_reset();
int ssv6xxx_hw_radio_init();
int ssv6xxx_set_channel(U16 ch) ATTRIBUTE_SECTION_SRAM;
void ssv6xxx_rf_enable() ATTRIBUTE_SECTION_SRAM;
void ssv6xxx_rf_disable() ATTRIBUTE_SECTION_SRAM;
void ssv_delay(U32 clk) ATTRIBUTE_SECTION_SRAM;

#endif /* _HW_CTRL_API_H_ */
