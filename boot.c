asm (
	".code16gcc\n"
	"call boot\n"
	"hang:\n"
	"jmp hang");

static void sys_loadkernel() {
	asm volatile (				/* reset floppy controller */
		"    mov  $0x00, %%ah\n"
		"    int  $0x13"
		: /* no output */
		: /* no input */
		: "%ax" );

	asm volatile (				/* read data from floppy */
		"    mov  $0x1000, %%ax\n"	/* set buffer address */
		"    mov  %%ax, %%es\n"
		"    mov  $0x00, %%bx\n"
		"    mov  $0x02, %%ah\n"	/* read data */
		"    mov  $0x20, %%al\n"	/* number of sectors to read */
		"    mov  $0x00, %%ch\n"	/* track  = 1 */
		"    mov  $0x02, %%cl\n"	/* sector = 2 */
		"    mov  $0x00, %%dh\n"	/* head = 1 */
		"    mov  $0x00, %%dl\n"	/* drive = 0 */
		"    int  $0x13"
		: /* no output */
		: /* no input */
		: "%ax", "%bx", "%cx", "%dx");
}

static void sys_startkernel() {
	asm volatile (
		"    mov  $0x1000, %%eax\n"
		"    mov  %%ax, %%ds\n"
		"    pushl %%eax\n"
		"    mov  $0x0000, %%eax\n"
		"    pushl %%eax\n"
		"    retf"
		: /* no output */
		: /* no input */
		: "%eax" );
}

int boot(void) {
	sys_loadkernel();
	sys_startkernel();
}
