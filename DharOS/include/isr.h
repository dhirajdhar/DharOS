#ifndef ISR_H
#define ISR_H

void isr_keyboard(); // IRQ1 -> vector 0x21
void isr_mouse();    // IRQ12 -> vector 0x2C

void keyboard_isr_handler();
void mouse_isr_handler();

#endif