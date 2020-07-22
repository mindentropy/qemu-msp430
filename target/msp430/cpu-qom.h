#ifndef QEMU_MSP430_CPU_QOM_H_
#define QEMU_MSP430_CPU_QOM_H_

#include "hw/core/cpu.h"

#define TYPE_MSP430_CPU "msp430-cpu"

typedef struct MSP430CPUClass {
	/*< private >*/
	CPUClass parent_class;
	/*< public >*/

	DeviceRealize parent_realize;
	DeviceReset parent_reset;
};

#define MSP430_CPU_CLASS(klass) \
	OBJECT_CLASS_CHECK(MSP430CPUClass, klass)

typedef struct MSP430CPU MSP430CPU;

#endif
