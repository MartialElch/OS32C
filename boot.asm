;boot.asm
[ORG 0x7C00]
    xor ax, ax
    mov ds, ax
    mov ss, ax
    mov sp, 0x9C00

    mov si, msg_start       ; print start message
    call bios_print

    cli                     ; stop processor
    hlt

;------------------------------------
bios_print:
    lodsb                   ; read next byte
    cmp al, 0               ; zero=end of str
    je bios_print_return
    mov ah, 0x0E            ; set color
    int 0x10                ; write char
    jmp bios_print          ; next
bios_print_return:
    ret

;------------------------------------
msg_start  db 'start booting', 13, 10, 0

    times 510-($-$$) db 0
    db 0x55
    db 0xAA
