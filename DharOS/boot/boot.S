.section .multiboot
.align 4
.long 0x1BADB002
.long 0x00
.long -(0x1BADB002 + 0x00)

.section .text
.global _start
.type _start, @function
.extern kernel_main

_start:
    cli
    mov $stack_top, %esp
    call kernel_main

hang:
    hlt
    jmp hang

.section .data           # ✅ Use initialized memory
.align 16
stack:
.skip 4096               # 4KB stack
stack_top: