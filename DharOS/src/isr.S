global isr_keyboard
global isr_mouse
extern keyboard_isr_handler
extern mouse_isr_handler

isr_keyboard:
    pusha
    call keyboard_isr_handler
    popa
    mov al, 0x20
    out 0x20, al
    iret

isr_mouse:
    pusha
    call mouse_isr_handler
    popa
    mov al, 0x20
    out 0xA0, al
    mov al, 0x20
    out 0x20, al
    iret