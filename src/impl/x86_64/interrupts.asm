BITS 64
GLOBAL isr_common_stub
GLOBAL irq0_stub
GLOBAL irq1_stub
GLOBAL irq2_stub
GLOBAL irq3_stub
GLOBAL irq4_stub
GLOBAL irq5_stub
GLOBAL irq6_stub
GLOBAL irq7_stub
GLOBAL irq8_stub
GLOBAL irq9_stub
GLOBAL irq10_stub
GLOBAL irq11_stub
GLOBAL irq12_stub
GLOBAL irq13_stub
GLOBAL irq14_stub
GLOBAL irq15_stub

extern irq1_handler ; C handler

section .text

isr_common_stub:
	iretq

; Default IRQ stub: just EOI if needed and return (for unhandled IRQs)
%macro IRQ_STUB_DEFAULT 1
%1:
	; Master EOI (some IRQs will come from slave too, but we only use IRQ1 for now)
	mov al, 0x20
	out 0x20, al
	iretq
%endmacro

IRQ_STUB_DEFAULT irq0_stub

; Keyboard IRQ1: call C handler, then send EOI
irq1_stub:
	push rbp
	mov rbp, rsp
	cld
	; call C handler (no args)
	call irq1_handler
	; Send EOI to PIC master
	mov al, 0x20
	out 0x20, al
	leave
	iretq

IRQ_STUB_DEFAULT irq2_stub
IRQ_STUB_DEFAULT irq3_stub
IRQ_STUB_DEFAULT irq4_stub
IRQ_STUB_DEFAULT irq5_stub
IRQ_STUB_DEFAULT irq6_stub
IRQ_STUB_DEFAULT irq7_stub
IRQ_STUB_DEFAULT irq8_stub
IRQ_STUB_DEFAULT irq9_stub
IRQ_STUB_DEFAULT irq10_stub
IRQ_STUB_DEFAULT irq11_stub
IRQ_STUB_DEFAULT irq12_stub
IRQ_STUB_DEFAULT irq13_stub
IRQ_STUB_DEFAULT irq14_stub
IRQ_STUB_DEFAULT irq15_stub
