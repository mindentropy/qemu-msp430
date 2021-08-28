msp430_ss = ss.source_set()

msp430_ss.add(when: 'CONFIG_MSP430_LAUNCHPAD', if_true: files('msp430_launchpad.c'))

hw_arch += {'msp430': msp430_ss}
