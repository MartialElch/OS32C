#include <types.h>
#include <io.h>
#include <irq.h>

static idt_entry_t idt[IDT_MAX_DESCRIPTORS];
static idtr_t idtr;
static bool vectors[IDT_MAX_DESCRIPTORS];

void exception_handler() {
    __asm__ volatile ("cli; hlt"); /* Completely hangs the computer */
}

void initIRQ(void) {
    uint8_t vector;

    __asm__ volatile (
        "cli");

    idtr.base = (uint32_t) ((void*)idt);
    idtr.limit = (uint16_t)sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;

    for (vector = 0; vector < 32; vector++) {
        idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
        vectors[vector] = true;
    }

    __asm__ volatile (
        "lidt (%0)"
        : /* no output */
        : "m" (idtr)
        : /* no globber */);
    __asm__ volatile (
        "sti");

    return;
}

void registerIRQ(uint8_t n, void* isr) {
   __asm__ volatile (
       "cli");

    idt_set_descriptor(n, isr, 0x8E);

    __asm__ volatile (
        "sti");
}

void idt_set_descriptor(uint8_t n, void* isr, uint8_t flags) {
    uint8_t vector;

    vector = n + IRQ_BASE;

    idt_entry_t* descriptor = &idt[vector];

    descriptor->isr_low        = (uint32_t)isr & 0xFFFF;
    descriptor->isr_high       = ((uint32_t)isr >> 16) & 0xFFFF;
    descriptor->kernel_cs      = GDT_OFFSET_KERNEL_CODE;
    descriptor->attributes     = flags;
    descriptor->ist            = 0;
}
