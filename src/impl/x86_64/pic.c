#include "io.h"
#include <stdint.h>
#include <stddef.h>

#define PIC1            0x20
#define PIC2            0xA0
#define PIC1_COMMAND    PIC1
#define PIC1_DATA       (PIC1+1)
#define PIC2_COMMAND    PIC2
#define PIC2_DATA       (PIC2+1)

#define ICW1_INIT       0x10
#define ICW1_ICW4       0x01
#define ICW4_8086       0x01

// keyboard ring buffer (exported)
volatile uint8_t key_buffer[16] = {0};
volatile size_t buffer_head = 0;
volatile size_t buffer_tail = 0;

static void pic_remap(uint8_t offset1, uint8_t offset2) {
	uint8_t a1 = inb(PIC1_DATA);
	uint8_t a2 = inb(PIC2_DATA);

	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();
	outb(PIC1_DATA, offset1);
	io_wait();
	outb(PIC2_DATA, offset2);
	io_wait();
	outb(PIC1_DATA, 4);
	io_wait();
	outb(PIC2_DATA, 2);
	io_wait();
	outb(PIC1_DATA, ICW4_8086);
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();
	outb(PIC1_DATA, a1);
	outb(PIC2_DATA, a2);
}

void pic_init(void) {
	pic_remap(32, 40);
	// unmask IRQ1
	uint8_t mask1 = inb(PIC1_DATA);
	mask1 &= ~(1 << 1);
	outb(PIC1_DATA, mask1);
}

void irq1_handler(void) {
	uint8_t scancode = inb(0x60);
	size_t next_head = (buffer_head + 1) % 16;
	if (next_head != buffer_tail) {
		key_buffer[buffer_head] = scancode;
		buffer_head = next_head;
	}
}
