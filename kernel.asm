;--------------------
;kernel.asm
bits 32
[ORG 0x10000]

struc idt_entry
	.m_baseLow		resw 1
	.m_selector		resw 1				; code segment selector
	.m_reserved		resb 1				; zero
	.m_flags		resb 1				; type and attributes
	.m_baseHi		resw 1
endstruc

struc idt_ptr
	.m_size			resw 1
	.m_base			resd 1
endstruc

	mov ax, datasel			; set data selector
	mov ds, ax
	mov es, ax
	mov ax, videosel		; set videoselector
	mov gs, ax

	call printreg

	mov esi, msg_load
	call write_string

	call set_idt

	mov eax, 0x21
	mov ebx, keyboard_handler
	mov ecx, TYPE_IRQ
	call irq_register

	mov eax, 0x06			; don't know why, but we need to handle
	mov ebx, irq_handler		; this one
	mov ecx, TYPE_IRQ
	call irq_register

	sti				; enable interrupts

hang:
	hlt
	jmp hang

;--------------------
keyboard_handler:
	pushad

	mov ax, 0x18
	mov gs, ax

	in  al, 0x60				; get key data
	mov bl, al
	xor ecx, ecx
	mov cl, al

	in  al, 0x61				; keyboard control
	mov ah, al
	or  al, 0x80				; disable bit 7
	out 0x61, al				; send it back
	xchg ah, al				; get original
	out 0x61, al				; send that back

	mov al, 0x20				; end of interrupt
	out 0x20, al

	and bl, 0x80				; key released
	jnz keyboard_handler_exit		; don't repeat

	mov ch, 0x0A
	mov word [gs:0], cx

keyboard_handler_exit:
	popad
	iretd

;--------------------
irq_handler:
	mov ax, 0x30
	mov gs, ax
	mov word [gs:0], 0x0A41
	iret

;--------------------
write_char:
write_char_done:
	ret

write_string:
	pushad
	call line_get
	mov di, ax
	mov ah, 0x0A
write_string_next:
	mov al, byte[esi]
	cmp al, 0
	je write_string_done
	mov word [gs:di], ax
	add di, 2
	inc esi
	jmp write_string_next
write_string_done:
	popad
	ret

;--------------------
printreg:
	push ax
	push bx
	push ax
	call line_get
	mov di, ax
	pop bx
	mov ah, 0x0A
	mov si, 0
printreg_next:
	rol bx, 4
	mov al, bl
	and al, 0x0F
	cmp al, 10
	jl  printreg_nibblelow
	add al, 0x07
printreg_nibblelow:
	add al, 0x30
	mov word [gs:di], ax

	add di, 2
	inc si
	cmp si, 4
	jne printreg_next

	pop bx
	pop ax
	ret

;--------------------
line_get:				; get gs pos in ax
	push bx
	mov ax, [line]			; get current line
	mov bx, 80			; multiply by terminal width
	mul bx
	shl ax, 1			; multiply by 2
line_advance:
	push ax	
	mov  ax, [line]			; get current line
	inc  ax				; incrment line
	mov  [line], ax			; store back
	pop  ax

	pop  bx
	ret

;--------------------
set_idt:
	cli												; disable interrupts
	mov word [idtr+idt_ptr.m_size], idt_size - 1	; set idt size
	mov dword [idtr+idt_ptr.m_base], idt		; set idt base pointer
	cli
	lidt [idtr]										; store idt structure
	ret

;--------------------
irq_register:
	pusha

	mov esi, msg_regirq
	call write_string

	mov   edx, 8
	mul   dx
	add   eax, idt
	mov   word [eax+idt_entry.m_baseLow], bx
	shr   ebx, 16
	mov   word [eax+idt_entry.m_baseHi], bx
	mov   byte [eax+idt_entry.m_flags], cl
	mov   byte [eax+idt_entry.m_reserved], 0
	mov   word [eax+idt_entry.m_selector], 0x08
	popa
	ret

;--------------------
msg_load    db 'start kernel', 0
msg_regirq  db 'register irq', 0
msg_irq     db 'irq received', 0
msg_irqdone db 'irq done', 0

line       dw 2

outstr16   db '0000', 0
port60     dw 0
reg16      dw 0

TYPE_IRQ	equ		0x8e

;--------------------
gdt:
nullsel equ $-gdt			; 0x00 Null Descriptor
	dd 0x00000000
	dd 0x00000000
codesel equ $-gdt			; 0x08 Code Descriptor
	dw 0xffff			; limit 4Gb
	dw 0x0000			; base 0000:0000
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
	dw 39999			; limit 80*25*2-1
	dw 0x8000			; base 0xb8000
	db 0x0b
	db 0x92				; present, ring 0, data, expand-up, writeable
	db 0x00
	db 0x00
gdt_end:

;--------------------
idt:
%rep 256
	istruc idt_entry
		at idt_entry.m_baseLow,  dw 0
		at idt_entry.m_selector, dw 0x8   
		at idt_entry.m_reserved, db 0
		at idt_entry.m_flags,    db 0x8e
		at idt_entry.m_baseHi,   dw 0
	iend
%endrep
idt_end:

idt_size db idt_end - idt

idtr:
	istruc idt_ptr
		at idt_ptr.m_size, dw 0
		at idt_ptr.m_base, dd 0
	iend

;--------------------
