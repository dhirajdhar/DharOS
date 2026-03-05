#ifndef IDT_H
#define IDT_H
#include <stdint.h>

typedef struct {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  ist;        // zero if not using IST
    uint8_t  type_attr;  // 0x8E for interrupt gate
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;
} __attribute__((packed)) idt_entry_t;

void idt_set_gate(int n, void (*handler)(), uint8_t type_attr);
void idt_load();

#endif