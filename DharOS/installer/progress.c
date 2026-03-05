#include "progress.h"
#include "../include/vga.h"

void show_progress(const char* msg) {
    fill_screen(0x01); // Blue background
    draw_string(20, 100, msg, 0x0F); // White text at center-ish
}