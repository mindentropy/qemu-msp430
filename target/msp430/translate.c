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

#include "exec/gen-icount.h"

/*
 *  Referencing the following archs for writing translation code:
 *  1. AVR
 *  2. Microblaze
 *  3. Tricore
 *
 */

enum {
	DISAS_EXIT = DISAS_TARGET_0, /* We want return to the cpu main loop */
	DISAS_LOOKUP = DISAS_TARGET_1, /* We have a variable condition exit */
	DISAS_CHAIN = DISAS_TARGET_2,  /* We have a single condition exit. */
};

static const char *regnames[MSP430_NUM_REGISTERS] = {
	"r0", "r1", "r2", "r3", "r4", "r5",
	"r6", "r7", "r8", "r9", "r10", "r11",
	"r12", "r13", "r14", "r15",
};

typedef struct DisasContext DisasContext;


struct DisasContext { /* Holds current CPU state Information */
	DisasContextBase base;
	target_ulong next_pc; //TODO:Is this redundant??
	CPUState *cpu_state;
	CPUMSP430State *msp430_cpu_state;
	uint32_t opcode; /* TODO: Can be removed? */
	TCGCond skip_cond;
};


#define TCGV_REGS(x) 	(tcg_cpu_regs[x])
#define TCGV_CPU_PC 	(tcg_cpu_regs[MSP430_PC_REG])
#define TCGV_CPU_SP 	(tcg_cpu_regs[MSP430_SP_REG])
#define TCGV_CPU_SR 	(tcg_cpu_regs[MSP430_SR_REG])
#define TCGV_CPU_CG2 	(tcg_cpu_regs[MSP430_CG2_REG])

static TCGv tcg_cpu_regs[MSP430_NUM_REGISTERS];

__attribute__((unused)) static uint16_t next_code(DisasContext *ctx)
{
	return 0;
}

static void msp430_disas_ctx_dump_state(DisasContextBase *dcbase)
{
	DisasContext *ctx = container_of(dcbase, DisasContext, base);
	qemu_fprintf(stderr, "CTX Dump State\n");
	qemu_fprintf(stderr, "==============\n");
	qemu_fprintf(stderr,"pc_next %u\n", ctx->base.pc_first);
	qemu_fprintf(stderr,"pc_next %u\n", ctx->base.pc_next);
	qemu_fprintf(stderr,"is_jmp %u\n", ctx->base.is_jmp);
	qemu_fprintf(stderr,"max_insns %u\n", ctx->base.max_insns);
	qemu_fprintf(stderr,"num_insns %u\n", ctx->base.num_insns);
	qemu_fprintf(stderr,"TB flags: %u\n", ctx->base.tb->flags);
	qemu_fprintf(stderr,"TB cflags: %u\n", ctx->base.tb->cflags);
	qemu_fprintf(stderr,"TB CS base: %u\n", ctx->base.tb->cs_base);
	qemu_fprintf(stderr,"TB PC: %u\n", ctx->base.tb->pc);

	qemu_fprintf(stderr, "==============\n");
}

static void  msp430_tr_init_disas_context(DisasContextBase *dcbase, CPUState *cs)
{
	qemu_fprintf(stderr, "===%s:%d===\n",__PRETTY_FUNCTION__, __LINE__);

	DisasContext *ctx __attribute__((unused)) = container_of(dcbase, DisasContext, base);
	CPUMSP430State *env __attribute__((unused)) = cs->env_ptr;

	ctx->cpu_state = cs;
	ctx->msp430_cpu_state = env;
	ctx->skip_cond = TCG_COND_NEVER;

	ctx->base.max_insns = 1;
}

static void msp430_tr_tb_start(DisasContextBase *dcbase, CPUState *cpu)
{
	/* Will be empty */
	qemu_fprintf(stderr, "===%s:%d===\n",__PRETTY_FUNCTION__, __LINE__);
}

static void msp430_tr_insn_start(DisasContextBase *dcbase, CPUState *cpu)
{
	qemu_fprintf(stderr, "===%s:%d===\n",__PRETTY_FUNCTION__, __LINE__);
	tcg_gen_insn_start(dcbase->pc_next);
}

static void msp430_tr_translate_insn(DisasContextBase *dcbase, CPUState *cpu)
{
	uint32_t opcode;
	qemu_fprintf(stderr, "===%s:%d===\n",__PRETTY_FUNCTION__, __LINE__);

	DisasContext *ctx = container_of(dcbase, DisasContext, base);
	CPUMSP430State *env = cpu->env_ptr;

	opcode = cpu_lduw_code(env, ctx->base.pc_next);
	qemu_fprintf(stderr, "Opcode: %u\n", opcode);

	msp430_disas_ctx_dump_state(dcbase);
	ctx->opcode = opcode;
	ctx->base.pc_next += 2; //16 bit instruction, hence increase it by 2 bytes
	ctx->next_pc += 2; //TODO: Redundant next_pc ???

	if(ctx->base.is_jmp == DISAS_NEXT) {
		target_ulong page_start;

		page_start = ctx->base.pc_first & TARGET_PAGE_MASK;

		if((ctx->base.pc_next - page_start) >= (TARGET_PAGE_SIZE - 4)) {
			ctx->base.is_jmp = DISAS_TOO_MANY;
		}
	}
}

static void gen_goto_tb(DisasContext *ctx, int n, target_ulong dest)
{
	if(translator_use_goto_tb(&ctx->base, dest)) {
	/* is goto_tb allowed between the current TB and destination PC. See translator.h */
		tcg_gen_goto_tb(n);
		tcg_gen_movi_i32(TCGV_CPU_PC, dest);
		tcg_gen_exit_tb(ctx->base.tb, n);
	} else {
		tcg_gen_movi_i32(TCGV_CPU_PC, dest);

		if(ctx->base.singlestep_enabled) {
			gen_helper_debug(cpu_env); /*TODO: Need more info on this helper function */
		} else {
			tcg_gen_lookup_and_goto_ptr();
		}
	}

	ctx->base.is_jmp = DISAS_NORETURN;
}

static void msp430_tr_tb_stop(DisasContextBase *dcbase, CPUState *cpu)
{
	DisasContext *ctx = container_of(dcbase, DisasContext, base);

	qemu_fprintf(stderr, "is_jmp: %d\n", ctx->base.is_jmp);

	switch(ctx->base.is_jmp) {
		case DISAS_NORETURN:
			break;
		case DISAS_NEXT:
		case DISAS_TOO_MANY:
			qemu_fprintf(stderr, "DISAS_TOO_MANY ===%s:%d===\n", __PRETTY_FUNCTION__, __LINE__);
			gen_goto_tb(ctx, 0, ctx->base.pc_next);
			return;
		case DISAS_CHAIN:
			qemu_fprintf(stderr, "DISAS_CHAIN ===%s:%d===\n",__PRETTY_FUNCTION__, __LINE__);

			{
				gen_goto_tb(ctx, 1, ctx->base.pc_next);
				break;
			}
			tcg_gen_movi_tl(TCGV_CPU_PC, ctx->next_pc);
			/* fall through */
		case DISAS_LOOKUP:
			qemu_fprintf(stderr, "DISAS_LOOKUP ===%s:%d===\n",__PRETTY_FUNCTION__, __LINE__);
			if(!ctx->base.singlestep_enabled) {
				tcg_gen_lookup_and_goto_ptr();
				break;
			}
			/* fall through */
		case DISAS_EXIT:
			if(ctx->base.singlestep_enabled) {
				gen_helper_debug(cpu_env); /* TODO: How does this work? */
			} else {
				tcg_gen_exit_tb(NULL, 0);
			}
			break;
		default:
			g_assert_not_reached();
	}
	qemu_fprintf(stderr, "===%s:%d===\n",__PRETTY_FUNCTION__, __LINE__);
}

static void msp430_tr_disas_log(const DisasContextBase *dcbase, CPUState *cpu)
{
	qemu_fprintf(stderr, "===%s:%d===\n",__PRETTY_FUNCTION__, __LINE__);
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
	translator_loop(&msp430_tr_ops, &dc.base, cs, tb, max_insns);
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
