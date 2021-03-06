#include <stdint.h>

#ifndef _IRQ_H
#define _IRQ_H

#define irq_enable()  __asm__("sti")
#define irq_disable()  __asm__("cli")
#define irq_return()  __asm__("iret")

struct IDTDescr {
	uint16_t offset_1;	// offset bits 0..15
	uint16_t selector;	// a code segment selector in GDT or LDT
	uint8_t  zero;		// unused, set to 0
	uint8_t  type_attr;	// type and attributes, see below
	uint16_t offset_2;	// offset bits 16..31
};

struct IDTDescr idt[256];

#define TYPE_IRQ  0x8E

#endif
