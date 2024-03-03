#gen = [
#  decodetree.process('insn.decode', extra_args: [ '--decode', 'decode_insn',
#                                                  '--insnwidth', '16' ])
#]

msp430_ss = ss.source_set()
msp430_system_ss= ss.source_set()

msp430_ss.add(files(
	'translate.c',
	'helper.c',
	'cpu.c',
	'gdbstub.c'))

target_arch += {'msp430': msp430_ss}
target_system_arch += {'msp430': msp430_system_ss}
