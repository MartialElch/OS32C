;boot.asm
[ORG 0x7C00]
    xor ax, ax
    mov ds, ax
    mov ss, ax
    mov sp, 0x9C00

    mov si, msg_welcome       ; print welcome message
    call bios_print

    mov si, msg_protected     ; switch processor to protected mode
    call bios_print

    cli                       ; no interrupt
    push ds                   ; save real mode
    lidt [idtr]               ; load idt register
    lgdt [gdtr]               ; load gdt register

    mov eax, cr0              ; set PE bit of CR0
    or  al, 0x01
    mov cr0, eax
    jmp codesel:go_pm

    cli                       ; stop processor
    hlt
;--------------------
bits 32

%define COLOR       0x0A 

go_pm:
    mov ax, datasel           ; set data selector
    mov ds, ax
    mov es, ax
    mov ax, videosel          ; set videoselector
    mov gs, ax

    mov ah, COLOR
    mov esi, msg_welcomeprot  ; print welcome message
    mov bx, 240               ; on line 4
    shl bx, 1
printString:
    lodsb
    cmp al, 0
    je printStringDone
    mov word [gs:bx], ax
    add bx, 2
    jmp printString
printStringDone:

    mov bx, 320               ; set cursor position
                              ; to start of 5th line
    mov dx, 0x03D4
    mov al, 0x0F
    out dx, al
    mov ax, bx
    mov dx, 0x03D5
    out dx, al

    mov dx, 0x03D4
    mov al, 0x0E
    out dx, al
    mov ax, bx
    shr ax, 8
    mov dx, 0x03D5
    out dx, al

    shl bx, 1                 ; set cursor color
    mov al, ' '
    mov ah, COLOR
    mov word [gs:bx], ax

    cli
    hlt

;--------------------
bits 16

idtr:
    dw gdt + 0x18 - 1       ; IDT limit=0
    dd gdt + 0x10           ; IDT base=0
gdtr:
    dw gdt_end - gdt - 1    ; length of gdt
    dd gdt                  ; base address of gdt

gdt:
nullsel equ $-gdt           ; 0x00 Null Descriptor
    dd 0x00000000
    dd 0x00000000
codesel equ $-gdt           ; 0x08 Code Descriptor
    dw 0xffff               ; limit 4Gb
    dw 0x0000               ; base 0000:0000
    db 0x00
    db 0x9a
    db 0xcf
    db 0x00
datasel equ $-gdt           ; 0x10
    dw 0xffff
    dw 0x0000
    db 0x00
    db 0x92
    db 0xcf
    db 0x00
videosel equ $-gdt          ; 0x18 Video Descriptor
    dw 39999                ; limit 80*25*2-1
    dw 0x8000               ; base 0xb8000
    db 0x0b
    db 0x92                 ; present, ring 0, data, expand-up, writeable
    db 0x00
    db 0x00
gdt_end:

msg_welcome     db 'booting me up ...', 13, 10, 0
msg_protected   db 'switch to protected mode', 13, 10, 0
msg_welcomeprot db 'welcome to protected mode', 0

;--------------------
bios_print:
    lodsb                   ; read next byte
    cmp al, 0               ; zero=end of str
    je bios_print_return
    mov ah, 0x0E            ; set color
    int 0x10                ; write char
    jmp bios_print          ; next
bios_print_return:
    ret

;--------------------
    times 510-($-$$) db 0
    db 0x55
    db 0xAA
