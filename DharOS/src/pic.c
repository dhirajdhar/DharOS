#include <stdint.h>
#include "io.h"

void pic_remap() {
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0xFF);
    outb(0xA1, 0xFF);
}

void pic_unmask_irq(uint8_t irq) {
    uint16_t port = (irq < 8) ? 0x21 : 0xA1;
    uint8_t mask = inb(port);
    mask &= ~(1 << (irq % 8));
    outb(port, mask);
}

void pic_eoi(uint8_t irq) {
    if (irq >= 8) outb(0xA0, 0x20);
    outb(0x20, 0x20);
}