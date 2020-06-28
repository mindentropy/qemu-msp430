#ifndef TMP123_REGS_H
#define TMP123_REGS_H

#include "hw/i2c/i2c.h"
#include "hw/misc/tmp123_regs.h"

typedef enum TMP123Reg {
	TMP123_REG_TEMPERATURE,
	TMP123_REG_CONFIG,
	TMP123_REG_T_LOW,
	TMP123_REG_T_HIGH
} TMP123Reg;

#endif
