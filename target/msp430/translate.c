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

static const char *regnames[MSP430_NUM_REGISTERS] = {
	"r0", "r1", "r2", "r3", "r4", "r5",
	"r6", "r7", "r8", "r9", "r10", "r11",
	"r12", "r13", "r14", "r15",
};

typedef struct DisasContext DisasContext;

struct DisasContext {
	DisasContextBase base;
};


#define TCGV_REGS(x) 	(tcg_cpu_regs[x])
#define TCGV_CPU_PC 	(tcg_cpu_regs[MSP430_PC_REG])
#define TCGV_CPU_SP 	(tcg_cpu_regs[MSP430_SP_REG])
#define TCGV_CPU_SR 	(tcg_cpu_regs[MSP430_SR_REG])
#define TCGV_CPU_CG2 	(tcg_cpu_regs[MSP430_CG2_REG])

static TCGv tcg_cpu_regs[MSP430_NUM_REGISTERS];

static void  msp430_tr_init_disas_context(DisasContextBase *dcb, CPUState *cpu)
{
	qemu_fprintf(stderr, "===%s:%d===\n",__PRETTY_FUNCTION__, __LINE__);

	DisasContext *dc __attribute__((unused)) = container_of(dcb, DisasContext, base);
	CPUMSP430State *env __attribute__((unused)) = cpu->env_ptr;
}

static void msp430_tr_tb_start(DisasContextBase *db, CPUState *cpu)
{

}

static void msp430_tr_insn_start(DisasContextBase *db, CPUState *cpu)
{

}

static void msp430_tr_translate_insn(DisasContextBase *db, CPUState *cpu)
{

}

static void msp430_tr_tb_stop(DisasContextBase *db, CPUState *cpu)
{

}

static void msp430_tr_disas_log(const DisasContextBase *db, CPUState *cpu)
{

}

static const TranslatorOps msp430_tr_ops __attribute__((unused)) = {
	.init_disas_context = msp430_tr_init_disas_context,
	.tb_start = msp430_tr_tb_start,
	.insn_start = msp430_tr_insn_start,
	.translate_insn = msp430_tr_translate_insn,
	.tb_stop = msp430_tr_tb_stop,
	.disas_log = msp430_tr_disas_log,
};

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
	int i = 0;

	for(i = 0; i<MSP430_NUM_REGISTERS; i++)
	{
		tcg_cpu_regs[i] = tcg_global_mem_new_i32(
							cpu_env,
							offsetof(CPUMSP430State, regs[i]),
							regnames[i]
						);
	}
}

void gen_intermediate_code(
			CPUState *cs,
			TranslationBlock *tb,
			int max_insns)
{
	qemu_fprintf(stderr, "===%s:%d===\n", __PRETTY_FUNCTION__, __LINE__);
	DisasContext dc = { };
	translator_loop(&msp430_tr_ops, &dc.base, cs,tb, max_insns);
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
