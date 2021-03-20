#include "qemu/osdep.h"
#include "hw/sysbus.h"
#include "exec/memory.h"
#include "exec/address-spaces.h"
#include "qemu/log.h"

#define TYPE_MSP430G2xxx_FLASHROM "msp430G2xxx_flashrom"
#define MSP430G2xxx_FLASHROM(obj) OBJECT_CHECK(MSP430FlashRomState, (obj), TYPE_MSP430G2xxx_FLASHROM)

#define MSP430G2XXX_FLASHROM_CONFIG_BASE 	(0x0128)
#define MSP430G2XXX_FLASHROM_CONFIG_SIZE 	(4)

#define FCTL1 (0)
#define FCTL2 (1)
#define FCTL3 (2)
#define FCTL4 (3)

typedef enum {
	NO_ERASE 			= 0x0,
	ERASE_INDIV_SEG 	= 0x1,
	ERASE_ALL_MEM_SEG 	= 0x2,
	ERASE_FLASH 		= 0x3
} flash_erase_cycle;

/*typedef enum {

};*/

struct MSP430FlashRomState {
	SysBusDevice parent_obj;

	MemoryRegion iomem;
	hwaddr config_base;
	int32_t config_size;
	uint16_t regs[MSP430G2XXX_FLASHROM_CONFIG_SIZE];
};

typedef struct MSP430FlashRomState MSP430FlashRomState;

static void msp430g2xxx_flashrom_reset(DeviceState *dev)
{
	MSP430FlashRomState *rs = MSP430G2xxx_FLASHROM(dev);

	rs->regs[FCTL1] = 0x9600;
	rs->regs[FCTL2] = 0x9642;
	rs->regs[FCTL3] = 0x9658;
	rs->regs[FCTL4] = 0x0000;

}

static void msp430g2xxx_flashrom_realize(DeviceState *dev, Error **errp)
{
	MSP430FlashRomState *rs = MSP430G2xxx_FLASHROM(dev);
}

static void msp430G2xxx_flashrom_class_init(ObjectClass *klass, void *data)
{
	DeviceClass *dc = DEVICE_CLASS(klass);

	dc->reset = msp430g2xxx_flashrom_reset;
	dc->realize = msp430g2xxx_flashrom_realize;
}

static const TypeInfo msp430_flashrom_info = {
	.name = TYPE_MSP430G2xxx_FLASHROM,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(MSP430FlashRomState),
	.class_init = msp430G2xxx_flashrom_class_init
};


static void msp430G2xxx_flashrom_register_types(void)
{
	type_register_static(&msp430_flashrom_info);
}

type_init(msp430G2xxx_flashrom_register_types)
