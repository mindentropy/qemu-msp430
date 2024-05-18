#include "qemu/osdep.h"
#include "qemu/error-report.h"
#include "qemu/log.h"
#include "qemu/qemu-print.h"
#include "cpu.h"
#include "hw/core/tcg-cpu-ops.h"
#include "exec/exec-all.h"
#include "exec/log.h"
#include "exec/cpu_ldst.h"
#include "exec/address-spaces.h"
#include "exec/helper-proto.h"

bool msp430_cpu_tlb_fill(CPUState *cpu_state, vaddr address,
	int size, MMUAccessType access_type,
	int mmu_idx, bool probe, uintptr_t retaddr)
{
/*	MSP430CPU *cpu = MSP430_CPU(cpu_state);
	CPUMSP430State *env = &cpu->env;
	hwaddr physical;*/
	access_type &= 1;
	int page_size = TARGET_PAGE_SIZE;

/*	int prot; */

/*	ret = get_physical_address(env, &physical, &prot,
				address, access_type, mmu_idx);*/

	qemu_fprintf(stdout,"%s addr: 0x%lx\n", __func__, address & 0xFFFFFFFF);

/*	if(probe) {
		page_size = 1;
	} else {
		cpu_loop_exit_restore(cpu_state, retaddr);
	}*/

	tlb_set_page(cpu_state, address & TARGET_PAGE_MASK,
					address & TARGET_PAGE_MASK,
					PAGE_READ|PAGE_WRITE|PAGE_EXEC,
					mmu_idx,
					page_size);
	return true;
}

bool msp430_cpu_exec_interrupt(CPUState *cpu_state,
							int interrupt_request)
{
	
	return true;
}

hwaddr msp430_cpu_get_phys_page_debug(CPUState *cpu_state, vaddr addr)
{
	return addr; /* 1:1 mapping, no MMU and hence no virtual memory address */
}

void helper_debug(CPUMSP430State *env)
{
	CPUState *cpu_state = env_cpu(env);
	cpu_state->exception_index = EXCP_DEBUG;
	cpu_loop_exit(cpu_state);
}
