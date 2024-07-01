;boot.asm
[ORG 0x7C00]
	xor ax, ax
	mov ds, ax
	mov ss, ax
	mov sp, 0x9C00

	mov si, msg_start	; print start message
	call sprint

	mov si, msg_prot	; tell user we are going to protected mode
	call sprint

	cli					; no interrupt
	push ds				; save real mode
	lidt [idtr]			; load idt register
	lgdt [gdtr]			; load gdt register

	mov bl, 0xa2

	mov eax, cr0		; set PE bit of CR0
	or  al, 0x01
	mov cr0, eax
	jmp codesel:go_pm

hang:
	cli					; stop processor
	hlt
	jmp hang

;--------------------
bits 32
go_pm:
	mov ax, datasel		; set data selector
	mov ds, ax
	mov es, ax
	mov ax, videosel	; set videoselector
	mov gs, ax

	mov esi, msg_welcome
	call write_string

hang_prot:
	cli
	hlt
	jmp hang_prot

;--------------------
write_char:
write_char_done:
	ret

write_string:
	pushad
	mov ax, 0
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

bits 16

;------------------------------------
sprint:
	mov ah, 0x0E		; set color
	lodsb				; read next byte
	cmp al, 0			; end if 0
	jz sprint_exit
	int 0x10			; write char
	jmp sprint			; next
sprint_exit:
	ret

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
msg_start	db 'start', 13, 10, 0
msg_prot	db 'go protected mode', 13, 10, 0
msg_welcome	db 'welcome to protected mode', 0

	times 510-($-$$) db 0
	db 0x55
	db 0xAA
;------------------------------------
