#include "qemu/osdep.h"
#include "qemu/units.h"
#include "qapi/error.h"
#include "cpu.h"
#include "net/net.h"
#include "hw/boards.h"
#include "hw/loader.h"
#include "exec/address-spaces.h"
#include "elf.h"
#include "hw/msp430/msp430.h" /* For future expansions */
#include "qemu/error-report.h"
#include "qemu/qemu-print.h"
#include "exec/memory.h"


/*static void msp430_load_firmware(CPUMSP430State *env, const char *filename)
{
	uint64_t entry;
	long fw_size;

	fw_size = load_elf(filename,
					NULL, NULL,
					NULL, &entry,
					NULL, NULL,
					NULL, 0,
					EM_MSP430, 1, 0);

	if(fw_size <= 0) {
		error_report("no kernel file : '%s'", filename);
		exit(1);
	} else {
		qemu_fprintf(stdout, "Entry point: %ld\n", entry);
	}

	env->regs[MSP430_PC_REG] = entry;
}*/


static void msp430_launchpad_init(MachineState *machine)
{
	/*MSP430CPU *cpu;
	CPUMSP430State *env;*/
	MemoryRegion *sysmem, *sram, *flash_ram;
	/*DeviceState *flashrom;*/

	cpu_create(machine->cpu_type);
	/*cpu = MSP430_CPU(cpu_create(machine->cpu_type));*/
	/* env = &cpu->env; */

	sysmem = get_system_memory();

	sram = g_new(MemoryRegion, 1);
	memory_region_init_ram(
						sram,
						NULL,
						"msp430.sram",
						MSP430_SRAM_SIZE,
						&error_fatal
					);
	memory_region_add_subregion(
						sysmem,
						MSP430_SRAM_BASE,
						sram
					);

	flash_ram = g_new(MemoryRegion, 2);
	memory_region_init_ram(
						flash_ram,
						NULL,
						"msp430.flash_ram",
						MSP430_FLASH_SIZE,
						&error_fatal
					);

	memory_region_add_subregion(
						sysmem,
						MSP430_FLASH_BASE,
						flash_ram
					);

	/*
	 * TODO: Test with loading the code to RAM. Create MemoryRegion spanning
	 * the entire memory space of RAM and Flash ROM and then load the firmware
	 */
	/* Create Flash and Flash Controller */
	/*flashrom = qdev_new("msp430G2xxx_flashrom"); */ /* Type created in hw/block */
/*	sysbus_realize_and_unref(SYS_BUS_DEVICE(flashrom), &error_fatal);
	sysbus_mmio_map(SYS_BUS_DEVICE(flashrom), 0, MSP430_FLASH_BASE);*/

	/*if(machine->firmware) {
		msp430_load_firmware(env, machine->firmware);
	}*/
}

static void msp430_machine_init(MachineClass *mc)
{
	mc->desc = "MSP430 Launchpad board";
	mc->init = msp430_launchpad_init;
	mc->default_cpu_type = MSP430_CPU_TYPE_NAME("g2553");
}

DEFINE_MACHINE("msp430-launchpad", msp430_machine_init)
