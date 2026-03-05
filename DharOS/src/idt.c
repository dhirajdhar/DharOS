#include "idt.h"

#define IDT_SIZE 256
static idt_entry_t idt[IDT_SIZE];

void idt_set_gate(int n, void (*handler)(), uint8_t type_attr) {
    uintptr_t addr = (uintptr_t)handler;
    idt[n].offset_low  = addr & 0xFFFF;
    idt[n].selector    = 0x08; // kernel code segment
    idt[n].ist         = 0;
    idt[n].type_attr   = type_attr;
    idt[n].offset_mid  = (addr >> 16) & 0xFFFF;
    idt[n].offset_high = (addr >> 32) & 0xFFFFFFFF;
    idt[n].zero        = 0;
}

extern void idt_flush(uintptr_t idt_ptr); // assembly stub

void idt_load() {
    struct {
        uint16_t limit;
        uintptr_t base;
    } __attribute__((packed)) idt_ptr = {
        .limit = sizeof(idt) - 1,
        .base  = (uintptr_t)&idt
    };
    idt_flush((uintptr_t)&idt_ptr);
}