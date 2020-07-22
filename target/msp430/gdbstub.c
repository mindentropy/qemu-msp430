#include "qemu/osdep.h"
#include "cpu.h"
#include "exec/gdbstub.h"

int msp430_cpu_gdb_read_register(CPUState *cs, GByteArray *mem_buff, int n)
{
	return 0;
}


int msp430_cpu_gdb_write_register(CPUState *cs, uint8_t *mem_buff, int n)
{
	return 0;
}
