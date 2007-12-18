#ifndef LIBOS_PERCPU_H
#define LIBOS_PERCPU_H

/*
 * Per-cpu data structures
 */

#ifndef _ASM
#include <stdint.h>
#include <libos/libos.h>
#include <libos/fsl-booke-tlb.h>
#endif

#define CPUSAVE_LEN 2
#define KSTACK_SIZE 2048

#define TLB1_SIZE 64

#ifndef _ASM
typedef uint8_t kstack_t[KSTACK_SIZE] __attribute__((aligned(16)));

typedef struct {
	client_cpu_t client;
	register_t normsave[CPUSAVE_LEN];
	register_t critsave[CPUSAVE_LEN];
	register_t machksave[CPUSAVE_LEN];
	register_t dbgsave[CPUSAVE_LEN];
	tlb_entry_t tlb1[TLB1_SIZE];
	int coreid;
	uint8_t *kstack; // Set to stack[KSTACK_SIZE - FRAMELEN];
	kstack_t debugstack, critstack, mcheckstack;
} cpu_t;

register cpu_t *cpu asm("r2");
#endif

#endif