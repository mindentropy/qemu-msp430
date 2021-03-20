#include "qemu/osdep.h"
#include "cpu.h"
#include "disas/disas.h"
#include "exec/exec-all.h"
#include "tcg/tcg-op.h"
#include "exec/helper-proto.h"
#include "exec/cpu_ldst.h"
#include "exec/helper-gen.h"
#include "exec/translator.h"
#include "qemu/qemu-print.h"

#include "trace-tcg.h"
#include "exec/log.h"

/*
static const char *regnames[MSP430_NUM_REGISTERS] = {
	"r0", "r1", "r2", "r3", "r4", "r5",
	"r6", "r7", "r8", "r9", "r10", "r11",
	"r12", "r13", "r14", "r15",
};
*/

void msp430_cpu_dump_state(CPUState *cs, FILE *f, int flags)
{
	MSP430CPU *cpu = MSP430_CPU(cs);

	int i = 0;

	for(i = 0; i < MSP430_NUM_REGISTERS; i++) {
		qemu_fprintf(f, "r%d=0x%04x\n", i, cpu->env.regs[i]);
	}

	qemu_fprintf(f,"\n");
}

void msp430_tcg_init(void)
{

}

void gen_intermediate_code(
			CPUState *cpu,
			TranslationBlock *tb,
			int max_insns)
{

}

void restore_state_to_opc(
				CPUArchState *env,
				TranslationBlock *tb,
				target_ulong *data
				)
{

}

void cpu_state_reset(CPUMSP430State *env)
{
	/*
		TODO:Reset the registers to default state
	*/

}
