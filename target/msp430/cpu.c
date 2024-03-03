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
	//MSP430CPU *cpu = MSP430_CPU(obj);

    //cpu_set_cpustate_pointers(cpu);
}

static void g2553_initfn(Object *obj)
{
	/*
		MSP430CPU *cpu = MSP430_CPU(obj);
		cpu->env.features = 0;
	*/

	/*
	* TODO: Set CPU environment features here.
	*/
}

static void msp430_cpu_realizefn(
					DeviceState *dev,
					Error **errp
				)
{
	CPUState *cs = CPU(dev); // DeviceState <- CPUState
	MSP430CPUClass *mcc = MSP430_CPU_GET_CLASS(dev);
	Error *local_err = NULL;

	/* local_err does not get set here. */
	cpu_exec_realizefn(cs, &local_err);

	if(local_err != NULL) { /* Setting local_err to
		NULL is important as the previous function
		will not set */
		error_propagate(errp, local_err);
		return;
	}

	/* TODO: Check for different MSP430 CPU models and set the features */
	cpu_reset(cs);
	qemu_init_vcpu(cs);

	mcc->parent_realize(dev, errp);
	return;
}


static void msp430_cpu_reset(DeviceState *dev)
{
	CPUState *s = CPU(dev);
	MSP430CPU *cpu = MSP430_CPU(s); // DeviceState <- CpuState <- MSP430CPU

	MSP430CPUClass *mcc = MSP430_CPU_GET_CLASS(cpu);
	CPUMSP430State *env = &cpu->env;

	mcc->parent_reset(dev);

	cpu_state_reset(env);
}

static ObjectClass * msp430_cpu_class_by_name(const char *cpu_model)
{
	ObjectClass *oc;
	char *typename;

	typename = g_strdup_printf(MSP430_CPU_TYPE_NAME("%s"), cpu_model);
	oc = object_class_by_name(typename);
	g_free(typename);

	if(!oc || !object_class_dynamic_cast(oc, TYPE_MSP430_CPU) ||
		object_class_is_abstract(oc)) {
		return NULL;
	}
	return oc;
}

static bool msp430_cpu_has_work(CPUState *cpu)
{
	return true;
}

static void msp430_cpu_set_pc(CPUState *cs, vaddr value)
{
	MSP430CPU *cpu = MSP430_CPU(cs);

	cpu->env.regs[MSP430_PC_REG] = value & 0xFFFE;
}

static void msp430_disas_set_info(CPUState *cpu, disassemble_info *info)
{

}

static const VMStateDescription vmstate_msp430_cpu = {
	.name = "cpu",
	.unmigratable = 1,
	.version_id = 1,
	.minimum_version_id = 1,
};


static Property msp430_properties[] = {
	
	DEFINE_PROP_END_OF_LIST(),
};

static void msp430_cpu_do_interrupt(CPUState *cpu)
{

}

static void msp430_restore_state_to_opc(
				CPUState *cpu,
				const TranslationBlock *tb,
				const uint64_t *data
				)
{

}

static const gchar *msp430_gdb_arch_name(CPUState *cs)
{
	return g_strdup("msp430");
}

#include "hw/core/sysemu-cpu-ops.h"

static const struct SysemuCPUOps msp430_sysemu_ops = {
	.get_phys_page_debug = msp430_cpu_get_phys_page_debug,
};

#include "hw/core/tcg-cpu-ops.h"

struct TCGCPUOps msp430_tcg_ops = {
	.tlb_fill = msp430_cpu_tlb_fill,
	.initialize = msp430_tcg_init,
	.cpu_exec_interrupt = msp430_cpu_exec_interrupt,
	.do_interrupt = msp430_cpu_do_interrupt,
	.restore_state_to_opc = msp430_restore_state_to_opc,
};


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
	cc->dump_state = msp430_cpu_dump_state;
	cc->set_pc = msp430_cpu_set_pc;
	cc->gdb_read_register = msp430_cpu_gdb_read_register;
	cc->gdb_write_register = msp430_cpu_gdb_write_register;
	cc->gdb_num_core_regs = 1; /* TODO: Setup core registers */

	cc->gdb_arch_name = msp430_gdb_arch_name;
	cc->sysemu_ops = &msp430_sysemu_ops;
	cc->tcg_ops = &msp430_tcg_ops;

	/* Is the below really needed? No VMEM support in the microcontroller!!!
#ifdef CONFIG_USER_ONLY
	cc->do_transaction_failed = msp430_cpu_do_transaction_failed;
 #endif */
	dc->vmsd = &vmstate_msp430_cpu;
	device_class_set_props(dc, msp430_properties);

	cc->disas_set_info = msp430_disas_set_info;
/*	cc->tcg_initialize = msp430_tcg_init;*/
}

static const TypeInfo msp430_cpu_type_infos[] = {
	{
		.name = TYPE_MSP430_CPU,
		.parent = TYPE_CPU,
		.instance_size = sizeof(MSP430CPU),
		.instance_init = msp430_cpu_initfn,
		.class_size = sizeof(MSP430CPUClass),
		.class_init = msp430_cpu_class_init,
		.abstract = true,
	},
	{
		.parent = TYPE_MSP430_CPU,
		.instance_init = g2553_initfn,
		.name = MSP430_CPU_TYPE_NAME("g2553"),
	}
};

DEFINE_TYPES(msp430_cpu_type_infos)

/* Commented out below as we will register multiple MSP430 types  */
/*
static void msp430_cpu_register_types(void)
{
	type_register_static(&msp430_cpu_type_info);
}

type_init(msp430_cpu_register_types)
*/
