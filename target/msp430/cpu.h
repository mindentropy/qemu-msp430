#ifndef MSP430_CPU_H
#define MSP430_CPU_H

#include "cpu-qom.h"
#include "exec/cpu-defs.h"

#define MSP430_CPU_TYPE_SUFFIX "-" TYPE_MSP430_CPU
#define MSP430_CPU_TYPE_NAME(model) model MSP430_CPU_TYPE_SUFFIX
#define CPU_RESOLVING_TYPE TYPE_MSP430_CPU

#define MSP430_NUM_REGISTERS 	(16)

#define MSP430_PC_REG 			(0)
#define MSP430_SP_REG 			(1)
#define MSP430_SR_REG 			(2)
#define MSP430_CG2_REG 			(3)

/* G2553 sizes */
#define MSP430_FLASH_BASE 		(0xC000)
#define MSP430_FLASH_SIZE 		(0x4000)

#define MSP430_SRAM_BASE 		(0x200)
#define MSP430_SRAM_SIZE 		(0x100)

#define MMU_USER_IDX 0
#define MMU_CODE_DATA_IDX 0 /* MSP430 is Von Neumann and has both code and data in the same memory */

typedef struct CPUArchState {
	uint16_t regs[16];
} CPUMSP430State;

/*
	A MSP430 CPU
*/
struct ArchCPU {
	/* <private> */
	CPUState parent_obj;

	/* <public> */
	CPUMSP430State env;
	CPUNegativeOffsetState neg;
};

#include "exec/cpu-all.h"

static inline void cpu_get_tb_cpu_state(CPUMSP430State *env, vaddr *pc,
								uint64_t *cs_base, uint32_t *flags)
{
	*pc = env->regs[MSP430_PC_REG];
	*cs_base = 0;
	*flags = 1;
}

void cpu_state_reset(CPUMSP430State *s);
void msp430_cpu_dump_state(CPUState *cs, FILE *f, int flags);
int msp430_cpu_gdb_read_register(CPUState *cs, GByteArray *mem_buff, int n);
int msp430_cpu_gdb_write_register(CPUState *cs, uint8_t *mem_buff, int n);
void msp430_tcg_init(void);

bool msp430_cpu_tlb_fill(CPUState *cpu_state, vaddr address,
	int size, MMUAccessType access_type,
	int mmu_idx, bool probe, uintptr_t retaddr);

bool msp430_cpu_exec_interrupt(CPUState *cpu_state,
	int interrupt_request);

hwaddr msp430_cpu_get_phys_page_debug(CPUState *cpu_state, vaddr addr);


#endif
