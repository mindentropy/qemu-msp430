#include "qemu/osdep.h"
#include "cpu.h"
#include "exec/exec-all.h"
#include "qemu/host-utils.h"
#include "exec/log.h"
#include "exec/helper-proto.h"

bool msp430_cpu_tlb_fill(CPUState *cpu, vaddr address,
	int size, MMUAccessType access_type,
	int mmu_idx, bool probe, uintptr_t retaddr)
{
	return true;
}

bool msp430_cpu_exec_interrupt(CPUState *cpu,
							int interrupt_request)
{
	
	return true;
}

hwaddr msp430_cpu_get_phys_page_debug(CPUState *cpu, vaddr addr)
{
	return addr; /* 1:1 mapping, no MMU and hence no virtual memory address */
}

void helper_debug(CPUMSP430State *env)
{
	CPUState *cpu_state = env_cpu(env);
	cpu_state->exception_index = EXCP_DEBUG;
	cpu_loop_exit(cpu_state);
}
