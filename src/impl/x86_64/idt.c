#include "idt.h"
#include <stdint.h>

#define IDT_ENTRIES 256

extern void isr_common_stub(void);
extern void irq0_stub(void);
extern void irq1_stub(void);
extern void irq2_stub(void);
extern void irq3_stub(void);
extern void irq4_stub(void);
extern void irq5_stub(void);
extern void irq6_stub(void);
extern void irq7_stub(void);
extern void irq8_stub(void);
extern void irq9_stub(void);
extern void irq10_stub(void);
extern void irq11_stub(void);
extern void irq12_stub(void);
extern void irq13_stub(void);
extern void irq14_stub(void);
extern void irq15_stub(void);

extern void lidt(void* ptr);

static struct IdtEntry idt[IDT_ENTRIES];
static struct IdtPointer idt_ptr;

static inline void set_idt_gate(int n, uint64_t handler, uint8_t flags) {
	uint16_t sel = 0x08; // kernel code segment
	idt[n].offset_low = (uint16_t)(handler & 0xFFFF);
	idt[n].selector = sel;
	idt[n].ist = 0;
	idt[n].type_attr = flags; // present | DPL | type
	idt[n].offset_mid = (uint16_t)((handler >> 16) & 0xFFFF);
	idt[n].offset_high = (uint32_t)((handler >> 32) & 0xFFFFFFFF);
	idt[n].zero = 0;
}

void idt_init(void) {
	idt_ptr.limit = sizeof(idt) - 1;
	idt_ptr.base = (uint64_t)&idt[0];

	for (int i = 0; i < IDT_ENTRIES; i++) {
		set_idt_gate(i, (uint64_t)isr_common_stub, 0x8E);
	}

	// Hardware IRQs after PIC remap will be at 32..47
	set_idt_gate(32 + 0, (uint64_t)irq0_stub, 0x8E);
	set_idt_gate(32 + 1, (uint64_t)irq1_stub, 0x8E);
	set_idt_gate(32 + 2, (uint64_t)irq2_stub, 0x8E);
	set_idt_gate(32 + 3, (uint64_t)irq3_stub, 0x8E);
	set_idt_gate(32 + 4, (uint64_t)irq4_stub, 0x8E);
	set_idt_gate(32 + 5, (uint64_t)irq5_stub, 0x8E);
	set_idt_gate(32 + 6, (uint64_t)irq6_stub, 0x8E);
	set_idt_gate(32 + 7, (uint64_t)irq7_stub, 0x8E);
	set_idt_gate(32 + 8, (uint64_t)irq8_stub, 0x8E);
	set_idt_gate(32 + 9, (uint64_t)irq9_stub, 0x8E);
	set_idt_gate(32 + 10, (uint64_t)irq10_stub, 0x8E);
	set_idt_gate(32 + 11, (uint64_t)irq11_stub, 0x8E);
	set_idt_gate(32 + 12, (uint64_t)irq12_stub, 0x8E);
	set_idt_gate(32 + 13, (uint64_t)irq13_stub, 0x8E);
	set_idt_gate(32 + 14, (uint64_t)irq14_stub, 0x8E);
	set_idt_gate(32 + 15, (uint64_t)irq15_stub, 0x8E);

	// load idt
	__asm__ volatile ("lidt %0" : : "m"(idt_ptr));
}
