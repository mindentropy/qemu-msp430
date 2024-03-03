#ifndef QEMU_MSP430_CPU_QOM_H_
#define QEMU_MSP430_CPU_QOM_H_

#include "hw/core/cpu.h"

#define TYPE_MSP430_CPU "msp430-cpu"

typedef struct MSP430CPUClass {
	/*< private >*/
	CPUClass parent_class;
	/*< public >*/

	DeviceRealize parent_realize;
	ResettablePhases parent_phases;
	DeviceReset parent_reset;
} MSP430CPUClass;

/*#define MSP430_CPU_CLASS(klass) \
	OBJECT_CLASS_CHECK(MSP430CPUClass, (klass), TYPE_MSP430_CPU)*/

/*#define MSP430_CPU(obj) \
	OBJECT_CHECK(MSP430CPU, (obj), TYPE_MSP430_CPU)*/

/*#define MSP430_CPU_GET_CLASS(obj) \
	OBJECT_GET_CLASS(MSP430CPUClass, (obj), TYPE_MSP430_CPU)*/

OBJECT_DECLARE_CPU_TYPE(MSP430CPU, MSP430CPUClass, MSP430_CPU)


#endif /* QEMU_MSP430_CPU_QOM_H_ */
