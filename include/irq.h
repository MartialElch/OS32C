#define GDT_OFFSET_KERNEL_CODE 0x08
#define IDT_MAX_DESCRIPTORS    256

enum Interrupts
{
	IRQ_BASE     = 32,
	IRQ_KEYBOARD = 1,
	IRQ_FLOPPY   = 6
};

typedef struct {
    uint16_t    isr_low;      /* The lower 16 bits of the ISR's address */
    uint16_t    kernel_cs;    /* The GDT segment selector that the CPU will load into CS before calling the ISR */
    uint8_t	    ist;          /* The IST in the TSS that the CPU will load into RSP; set to zero for now */
    uint8_t     attributes;   /* Type and attributes; see the IDT page */
    uint16_t    isr_high;     /* The higher 16 bits of the lower 32 bits of the ISR's address */
} __attribute__((packed)) idt_entry_t;

typedef struct {
    uint16_t	limit;
    uint32_t	base;
} __attribute__((packed)) idtr_t;

extern void* isr_stub_table[];

void initIRQ(void);
void registerIRQ(uint8_t n, void* isr);
void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags);