/* A hypothetical TMP123 temperature sensor */

#ifndef QEMU_TMP123_H
#define QEMU_TMP123_H

#include "hw/i2c/i2c.h"
#include "hw/misc/tmp123_regs.h"

#define TYPE_TMP123 "tmp123"
#define TMP123(obj) OBJECT_CHECK(TMP123State, obj, TYPE_TMP123)

typedef struct TMP123State {
	/* Private */
	I2CSlave i2c;

	/* Public */
	uint8_t len;
	uint8_t buf[2];
	qemu_irq pin;

	uint8_t pointer;
	uint8_t config;
	int16_t temperature;
	int16_t limit[2];
	int faults;
	uint8_t alarm;

} TMP123State;

#endif
