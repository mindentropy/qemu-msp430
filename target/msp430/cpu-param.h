#ifndef MSP430_CPU_PARAM_H
#define MSP430_CPU_PARAM_H

/* TODO: Need to revisit this */
#define TARGET_LONG_BITS 32
#define TARGET_PAGE_BITS_VARY
#define TARGET_PAGE_BITS_MIN 10
//#define TARGET_PAGE_BITS 10 /* 1k */
//#define TARGET_PAGE_BITS 17 /* 128k?? */

#define TARGET_PHYS_ADDR_SPACE_BITS 32
#define TARGET_VIRT_ADDR_SPACE_BITS 32
/* For NB_MMU_MODES see cpu.h */

#endif
