#ifndef MSP430_CPU_H
#define MSP430_CPU_H

#include "cpu-qom.h"
#include "exec/cpu-defs.h"

#define MSP430_CPU_TYPE_SUFFIX "-" TYPE_MSP430_CPU
#define MSP430_CPU_TYPE_NAME(model) model MSP430_CPU_TYPE_SUFFIX
#define CPU_RESOLVING_TYPE TYPE_MSP430_CPU

typedef struct CPUMSP430State CPUMSP430State;

typedef struct CPUMSP430State CPUMSP430State;

struct CPUMSP430State {
	uint16_t PC_r0;
	uint16_t SP_r1;
	uint16_t r2_sr_cg1;
	uint16_t r3_cg2;
	uint16_t r4;
	uint16_t r5;
	uint16_t r6;
	uint16_t r7;
	uint16_t r8;
	uint16_t r9;
	uint16_t r10;
	uint16_t r11;
	uint16_t r12;
	uint16_t r13;
	uint16_t r14;
	uint16_t r15;
};

/*
	A MSP430 CPU
*/
typedef struct MSP430CPU {
	/* <private> */
	CPUState parent_obj;

	/* <public> */
	CPUNegativeOffsetState neg;
	CPUMSP430State env;

} MSP430CPU;

typedef struct CPUMSP430State CPUArchState;
typedef struct MSP430CPU ArchCPU;
static inline int cpu_mmu_index(CPUMSP430State *env, bool ifetch)
{
	return 0;
}

#include "exec/cpu-all.h"

static inline void cpu_get_tb_cpu_state(CPUMSP430State *env, target_ulong *pc,
								target_ulong *cs_base, uint32_t *flags)
{
	*pc = env->PC_r0;
	*cs_base = 0;
	*flags = 0;
}

void cpu_state_reset(CPUMSP430State *s);
void msp430_cpu_dump_state(CPUState *cs, FILE *f, int flags);
int msp430_cpu_gdb_read_register(CPUState *cs, GByteArray *mem_buff, int n);
int msp430_cpu_gdb_write_register(CPUState *cs, uint8_t *mem_buff, int n);
void msp430_tcg_init(void);

bool msp430_cpu_tlb_fill(CPUState *cpu, vaddr address,
	int size, MMUAccessType access_type,
	int mmu_idx, bool probe, uintptr_t retaddr);

bool msp430_cpu_exec_interrupt(CPUState *cpu,
	int interrupt_request);


#endif
