#include "keyboard.h"
#include "mouse.h"
#include "pic.h"

void keyboard_isr_handler() {
    (void)read_scancode(); // consume scancode so buffer updates
    pic_eoi(1);
}

void mouse_isr_handler() {
    handle_mouse_interrupt();
    pic_eoi(12);
}