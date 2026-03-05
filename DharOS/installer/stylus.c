#include "stylus.h"
#include "vga.h"
#include "keyboard.h"

void wait_for_stylus() {
    fill_screen(0x01); // Blue background
    draw_string(20, 100, "Tap stylus or press any key to begin install...", 0x0F);

    while (1) {
        if (read_scancode()) break;
    }
}