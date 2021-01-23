#include "qemu/osdep.h"
#include "qapi/error.h"
#include "cpu.h"
#include "qemu/module.h"
#include "hw/qdev-properties.h"
#include "migration/vmstate.h"
#include "exec/exec-all.h"
#include "fpu/softfloat-helpers.h"

static void msp430_cpu_initfn(Object *obj)
{

}

static void msp430_cpu_realizefn(
							DeviceState *dev,
							Error **errp
						)
{

}


static void msp430_cpu_reset(DeviceState *dev)
{

}

static ObjectClass * msp430_cpu_class_by_name(const char *cpu_model)
{
	return object_class_by_name(TYPE_MSP430_CPU);
}

static bool msp430_cpu_has_work(CPUState *cpu)
{
	return true;
}

static void msp430_cpu_set_pc(CPUState *cpu, vaddr value)
{

}

static void msp430_disas_set_info(CPUState *cpu, disassemble_info *info)
{

}

static const VMStateDescription vmstate_msp430_cpu = {
	.name = "cpu",
	.unmigratable = 1,
};


static Property msp430_properties[] = {
	
	DEFINE_PROP_END_OF_LIST(),
};

static void msp430_cpu_do_interrupt(CPUState *cpu)
{

}

static void msp430_cpu_class_init(ObjectClass *oc, void *data)
{
	MSP430CPUClass *mcc = MSP430_CPU_CLASS(oc);
	CPUClass *cc = CPU_CLASS(oc);
	DeviceClass *dc = DEVICE_CLASS(oc);

	device_class_set_parent_realize(dc,
					msp430_cpu_realizefn,
					&mcc->parent_realize);

	device_class_set_parent_reset(dc,
					msp430_cpu_reset, 
					&mcc->parent_reset);
	cc->class_by_name = msp430_cpu_class_by_name;
	cc->has_work = msp430_cpu_has_work;
	cc->do_interrupt = msp430_cpu_do_interrupt;
	cc->cpu_exec_interrupt = msp430_cpu_exec_interrupt;
	cc->dump_state = msp430_cpu_dump_state;
	cc->set_pc = msp430_cpu_set_pc;
	cc->gdb_read_register = msp430_cpu_gdb_read_register;
	cc->gdb_write_register = msp430_cpu_gdb_write_register;
	cc->tlb_fill = msp430_cpu_tlb_fill;

	/* Is the below really needed? No VMEM support in the microcontroller!!!
#ifdef CONFIG_USER_ONLY
	cc->do_transaction_failed = msp430_cpu_do_transaction_failed;
	cc->get_phys_page_debug = msp430_cpu_get_phys_page_debug;  
 #endif */
	dc->vmsd = &vmstate_msp430_cpu;
	device_class_set_props(dc, msp430_properties);

	cc->disas_set_info = msp430_disas_set_info;
	cc->tcg_initialize = msp430_tcg_init;
}

static const TypeInfo msp430_cpu_type_info = {
	.name = TYPE_MSP430_CPU,
	.parent = TYPE_CPU,
	.instance_size = sizeof(MSP430CPU),
	.instance_init = msp430_cpu_initfn,
	.class_size = sizeof(MSP430CPUClass),
	.class_init = msp430_cpu_class_init,
};


static void msp430_cpu_register_types(void)
{
	type_register_static(&msp430_cpu_type_info);
}

type_init(msp430_cpu_register_types);
