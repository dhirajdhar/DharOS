.section .multiboot
.align 4
.long 0x1BADB002              # Multiboot magic
.long 0x00                    # Flags (no memory info, no video mode)
.long -(0x1BADB002 + 0x00)    # Checksum

.section .text
.global _start
.type _start, @function
.extern installer_main        # Declare external symbol

_start:
    cli                       # Disable interrupts
    call installer_main       # Direct call to C entry point

hang:
    hlt
    jmp hang