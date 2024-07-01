;boot.asm
[ORG 0x7C00]
	xor ax, ax
	mov ds, ax
	mov ss, ax
	mov sp, 0x9C00

	mov si, msg_start	; print start message
	call sprint

hang:
	cli					; stop processor
	hlt
	jmp hang
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

;------------------------------------
msg_start	db 'start booting', 13, 10, 0

	times 510-($-$$) db 0
	db 0x55
	db 0xAA
;------------------------------------
