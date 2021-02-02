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

static void msp430_load_kernel(CPUMSP430State *env, const char *filename)
{
	uint64_t entry;
	long kernel_size;

	kernel_size = load_elf(filename,
				NULL,
				NULL,
				NULL,
				&entry,
				NULL,
				NULL,
				NULL,
				0,
				EM_MSP430,
				1,
				0);

	if(kernel_size <= 0) {
		error_report("no kernel file : '%s'", filename);
		exit(1);
	}
	env->PC_r0 = entry;
}

static void msp430_launchpad_init(MachineState *machine)
{
	struct MSP430CPU *cpu;
	CPUMSP430State *env;

	cpu = MSP430_CPU(cpu_create(machine->cpu_type));
	env = &cpu->env;

	/*MemoryRegion *sysmem = get_system_memory();*/

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
