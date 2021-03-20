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
#include "exec/memory.h"

static void msp430_load_kernel(CPUMSP430State *env, const char *filename)
{
	uint64_t entry;
	long kernel_size;

	kernel_size = load_elf(filename,
					NULL, NULL,
					NULL, &entry,
					NULL, NULL,
					NULL, 0,
					EM_MSP430, 1, 0);

	if(kernel_size <= 0) {
		error_report("no kernel file : '%s'", filename);
		exit(1);
	}
	env->regs[MSP430_PC_REG] = entry;
}

static void msp430_launchpad_init(MachineState *machine)
{
	struct MSP430CPU *cpu;
	CPUMSP430State *env;
	MemoryRegion *sysmem, *sram;
	DeviceState *flashrom;

	cpu = MSP430_CPU(cpu_create(machine->cpu_type));
	env = &cpu->env;

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

	/* Create Flash and Flash Controller */
	flashrom = qdev_new("msp430G2xxx_flashrom"); /* Type created in hw/block */
	sysbus_realize_and_unref(SYS_BUS_DEVICE(flashrom), &error_fatal);
	sysbus_mmio_map(SYS_BUS_DEVICE(flashrom), 0, MSP430_FLASH_BASE);

	if(machine->kernel_filename) {
		msp430_load_kernel(env, machine->kernel_filename);
	}
}

static void msp430_machine_init(MachineClass *mc)
{
	mc->desc = "MSP430 Launchpad board";
	mc->init = msp430_launchpad_init;
	mc->default_cpu_type = MSP430_CPU_TYPE_NAME("g2452");
}

DEFINE_MACHINE("msp430-launchpad", msp430_machine_init)
