#ifndef MSP430_CPU_H
#define MSP430_CPU_H

/*
	MSP430CPU state
*/

struct MSP430CPUState {
	/* <private> */
	CPUState parent_obj;

	/* <public> */

	struct {
	} cfg;
};

#endif
