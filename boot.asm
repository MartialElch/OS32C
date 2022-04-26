;boot.asm
[ORG 0x7C00]
	xor ax, ax
	mov ds, ax
	mov ss, ax
	mov sp, 0x9C00

	mov si, msg_load	; print message
	call sprint

	call read_floppy

	mov si, msg_prot	; print message
	call sprint

	cli					; no interrupt
	push ds				; save real mode
	lidt [idtr]			; load idt register
	lgdt [gdtr]			; load gdt register

	call reprogram_pic

	mov bl, 0xa2

	mov eax, cr0		; set PE bit of CR0
	or  al, 0x01
	mov cr0, eax
	jmp codesel:go_pm

;--------------------
bits 32
go_pm:
	mov ax, datasel			; set data selector
	mov ds, ax
	mov es, ax
	mov ax, videosel		; set videoselector
	mov gs, ax

	jmp 0x10000

bits 16

;------------------------------------
sprint:
	mov ah, 0x0E		; set color
	lodsb			; read next byte
	cmp al, 0		; end if 0
	jz sprint_exit
	int 0x10		; write char
	jmp sprint		; next
sprint_exit:
	ret

;--------------------
read_floppy:
	mov ah, 0x00		; reset floppy controller
	int 0x13		; execute command
	jc error

	mov ax, 0x1000		; set buffer address
	mov es, ax
	mov bx, 0x00

read_sector:
	mov ah, 0x02		; read data from floppy
	mov al, 0x20		; number of sectors to read
	mov ch, 0x00		; track  = 1
	mov cl, 0x02		; sector = 2
	mov dh, 0x00		; head   = 1
	int 0x13		; execute command
	jc error

read_done:
	ret

;--------------------
reprogram_pic:
	mov	al,0x11		; initialization sequence
	out	0x20,al		; send it to 8259A-1
	out	0xA0,al		; and to 8259A-2

	mov	al,0x20		; start of hardware int's (0x20)
	out	0x21,al

	mov	al,0x28		; start of hardware int's 2 (0x28)
	out	0xA1,al

	mov	al,0x04		; 8259-1 is master
	out	0x21,al

	mov	al,0x02		; 8259-2 is slave
	out	0xA1,al

	mov	al,0x01		; 8086 mode for both
	out	0x21,al
	out	0xA1,al

	mov	al,0xFF		; mask off all interrupts for now
	out	0xA1,al

	mov	al,0xB1		; mask all irq's but irq2 which
	out	0x21,al		; is cascaded
	ret

;--------------------
error:
	mov si, msg_err
	cli
	hlt

;--------------------
idtr:
	dw gdt + 0x18 - 1		; IDT limit=0
	dd gdt + 0x10			; IDT base=0
gdtr:
	dw gdt_end - gdt - 1	; length of gdt
	dd gdt					; base address of gdt

gdt:
nullsel equ $-gdt			; 0x00 Null Descriptor
	dd 0x00000000
	dd 0x00000000
codesel equ $-gdt			; 0x08 Code Descriptor
	dw 0xffff				; limit 4Gb
	dw 0x0000				; base 0000:0000
	db 0x00
	db 0x9a
	db 0xcf
	db 0x00
datasel equ $-gdt			; 0x10
	dw 0xffff
	dw 0x0000
	db 0x00
	db 0x92
	db 0xcf
	db 0x00
videosel equ $-gdt			; 0x18 Video Descriptor
	dw 39999				; limit 80*25*2-1
	dw 0x8000				; base 0xb8000
	db 0x0b
	db 0x92					; present, ring 0, data, expand-up, writeable
	db 0x00
	db 0x00
gdt_end:

;------------------------------------
msg_load	db 'loading kernel', 13, 10, 0
msg_prot	db 'go protected mode', 13, 10, 0
msg_err		db 'ERROR', 13, 10, 0

	times 510-($-$$) db 0
	db 0x55
	db 0xAA
;------------------------------------
