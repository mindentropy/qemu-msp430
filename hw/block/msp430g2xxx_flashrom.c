#include "qemu/osdep.h"
#include "hw/sysbus.h"
#include "hw/qdev-properties.h"
#include "exec/memory.h"
#include "exec/address-spaces.h"
#include "qemu/log.h"
#include "migration/vmstate.h"

/*
 * [NOTE] Deriving code from OneNand, CFI Pflash and MSP430 Qemu port
 */

#define TYPE_MSP430G2xxx_FLASHROM "msp430G2xxx_flashrom"
#define MSP430G2xxx_FLASHROM(obj) OBJECT_CHECK(MSP430FlashRomState,\
							(obj),\
							TYPE_MSP430G2xxx_FLASHROM)

#define MSP430G2XXX_FLASHROM_CONFIG_BASE 	(0x0128)
#define MSP430G2XXX_FLASHROM_CONFIG_SIZE 	(4)

#define FCTL1 	(0)
#define FCTL2 	(1)
#define FCTL3 	(2)
#define FCTL4 	(3)

typedef enum {
	NO_ERASE 			= 0x0,
	ERASE_INDIV_SEG 	= 0x1,
	ERASE_ALL_MEM_SEG 	= 0x2,
	ERASE_FLASH 		= 0x3
} flash_erase_cycle;

typedef enum {
	ACLK 		= 0x0,
	MCLK 		= 0x1,
	SMCLK_1 	= 0x2,
	SMCLK_2		= 0x3
} flash_ctrl_clk_src_select;

static uint64_t flashrom_cfg_read(void *opaque, hwaddr addr, unsigned size);
static void flashrom_cfg_write(void *opaque, hwaddr addr, uint64_t data, unsigned size);
static uint64_t flash_read(void *opaque, hwaddr addr, unsigned size);
static void flash_write(void *opaque, hwaddr addr, uint64_t data, unsigned size);
static void info_flash_write(void *opaque, hwaddr addr, uint64_t data, unsigned size);

static const MemoryRegionOps mmio_ops = {
	.read = flashrom_cfg_read,
	.write = flashrom_cfg_write,
	.endianness = DEVICE_NATIVE_ENDIAN,
};

static const MemoryRegionOps flash_ops = {
	.read = flash_read,
	.write = flash_write,
	.endianness = DEVICE_NATIVE_ENDIAN,
};

static const MemoryRegionOps info_ops = {
	.read = flash_read,
	.write = info_flash_write,
	.endianness = DEVICE_NATIVE_ENDIAN,
};

struct MSPMemory {
	MemoryRegion region;
	void *ptr;
	const char *name;
	uint32_t base;
	uint32_t size;
	uint32_t segment_size;
};

struct MSP430FlashRomState {
	/* <private> */
	SysBusDevice parent_obj;

	/* <public> */
	struct MSPMemory flash;
	struct MSPMemory info;

	hwaddr config_base;
	int32_t config_size;
	MemoryRegion iomem;

	/* uint16_t -> 4 configs of 2 bytes each */
	uint16_t regs[MSP430G2XXX_FLASHROM_CONFIG_SIZE];
};

typedef struct MSP430FlashRomState MSP430FlashRomState;

static uint64_t flashrom_cfg_read(void *opaque, hwaddr addr, unsigned size)
{
	return 0;
}

static void flashrom_cfg_write(void *opaque, hwaddr addr, uint64_t data, unsigned size)
{
	return;
}

static uint64_t flash_read(void *opaque, hwaddr addr, unsigned size)
{
	return 0;
}

static void info_flash_write(void *opaque, hwaddr addr, uint64_t data, unsigned size)
{

}

static void flash_write(void *opaque, hwaddr addr, uint64_t data, unsigned size)
{

}

static void msp430g2xxx_flashrom_reset(DeviceState *dev)
{
	MSP430FlashRomState *rs = MSP430G2xxx_FLASHROM(dev);

	rs->regs[FCTL1] = 0x9600;
	rs->regs[FCTL2] = 0x9642;
	rs->regs[FCTL3] = 0x9658;
	rs->regs[FCTL4] = 0x0000;
}

static void msp430_register_memory(struct MSPMemory *memory,
									MSP430FlashRomState *rs,
									const MemoryRegionOps *ops,
									int n)
{
	memory_region_init_rom_device(
						&(memory->region),
						OBJECT(rs),
						ops,
						rs,
						memory->name,
						memory->size,
						NULL);

	vmstate_register_ram(&memory->region, DEVICE(rs));
	memory->ptr = memory_region_get_ram_ptr(&(memory->region));
	sysbus_init_mmio(SYS_BUS_DEVICE(rs), &(memory->region) );
	sysbus_mmio_map(SYS_BUS_DEVICE(rs), n, memory->base);
}

static void msp430g2xxx_flashrom_realize(DeviceState *dev, Error **errp)
{
	MSP430FlashRomState *rs = MSP430G2xxx_FLASHROM(dev);

	rs->config_size = (MSP430G2XXX_FLASHROM_CONFIG_SIZE * 2);
	rs->config_base = MSP430G2XXX_FLASHROM_CONFIG_BASE;
	memory_region_init_io(&rs->iomem,
						OBJECT(rs),
						&mmio_ops,
						rs,
						"flash-cfg",
						rs->config_size);

	sysbus_init_mmio(SYS_BUS_DEVICE(dev), &rs->iomem);

	rs->flash.size = 16384;
	rs->flash.base = 0xC000;
	rs->flash.name = "flash";
	rs->flash.segment_size = 16384;
	memory_region_init_rom_device(
						&(rs->flash.region),
						OBJECT(rs),
						&flash_ops,
						rs,
						rs->flash.name,
						rs->flash.size,
						NULL);

/*	vmstate_register_ram(&rs->flash.region, DEVICE(rs));
	rs->flash.ptr = memory_region_get_ram_ptr(&(rs->flash.region) )
	sysbus_init_mmio(SYS_BUS_DEVICE(rs), &(rs->flash.region) );
	sysbus_mmio_map(SYS_BUS_DEVICES(rs), 1, rs->flash.base);*/

	msp430_register_memory(&rs->flash, rs, &flash_ops, 1);

	rs->info.size = 256;
	rs->info.base = 0x1000;
	rs->info.name = "info";
	rs->flash.segment_size = 256;
	msp430_register_memory(&rs->info, rs, &info_ops, 2);
}

static Property msp430_flashrom_properties[] = {

	DEFINE_PROP_UINT32("base", MSP430FlashRomState, flash.base, 0 /*0xC000*/),
	DEFINE_PROP_UINT32("size", MSP430FlashRomState, flash.size, 0 /*16384*/),
	DEFINE_PROP_UINT32("segment_size", MSP430FlashRomState, flash.segment_size, 0 /*16384*/),

	DEFINE_PROP_UINT32("base", MSP430FlashRomState, info.base, 0 /*0x1000*/),
	DEFINE_PROP_UINT32("size", MSP430FlashRomState, info.size, 0 /*256*/),
	DEFINE_PROP_UINT32("segment_size", MSP430FlashRomState, info.segment_size, 0 /*256*/),

	DEFINE_PROP_END_OF_LIST(),
};

static int msp430_flash_post_load(void *opaque, int version_id)
{
	/* Use only if necessary */
	return 0;
}

static const VMStateDescription vmstate_msp430_flash = {
	.name = "msp430_flashrom",
	.version_id = 1,
	.minimum_version_id = 1,
	.post_load = msp430_flash_post_load,
	.fields = (VMStateField[]) { /* Array of structures below */
		VMSTATE_END_OF_LIST()
	}
};

static void msp430G2xxx_flashrom_class_init(ObjectClass *klass, void *data)
{
	DeviceClass *dc = DEVICE_CLASS(klass);

	dc->realize = msp430g2xxx_flashrom_realize;
	dc->reset = msp430g2xxx_flashrom_reset;
	dc->vmsd = &vmstate_msp430_flash;
	device_class_set_props(
							dc,
							msp430_flashrom_properties
						);
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
