#ifndef _HW_REGS_API_H_
#define _HW_REGS_API_H_

#include <ssv_types.h>

#define ADDRESS_OFFSET		16
#define HW_ID_OFFSET        7

#define SMAC_REG_READ(_r, _v)    hw_reg_read(_r, _v)
#define SMAC_REG_WRITE(_r, _v)   hw_reg_write(_r, _v)

#define SMAC_REG_CONFIRM(_r, _v)                   \
({                                                 \
    U32 _regval_confirm;                           \
    SMAC_REG_READ(_r, &_regval_confirm);           \
    if ((_regval_confirm) != (_v)) {               \
        return -1;                                 \
    }                                              \
})
//        printf("[0x%08x]: 0x%08x!=0x%08x\n", (_r), (_v), (_regval_confirm));      

#define SMAC_REG_SET_BITS(_reg, _set, _clr)    hw_reg_set_bits(_reg, _set, _clr)


int hw_reg_write(U32 reg, U32 value);
int hw_reg_read(U32 reg, U32 *value);
int hw_reg_set_bits(U32 reg, U32 set, U32 clr);

#endif /* _HW_REGS_API_H_ */
