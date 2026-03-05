// kernel/entry.c
#include "desktop.h"
#include "window.h"
#include "vga.h"
#include "keyboard.h"
#include "mouse.h"
#include "fs.h"
#include "idt.h"
#include "isr.h"
#include "pic.h"
#include "ata.h"

static void draw_splash() {
    fill_rect_fast(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0x01); // DD blue
    draw_string(92, 96, "DharOS", 0x0F);
    draw_string(100, 112, "Made by DD", 0x0F);
}

static void init_interrupts() {
    pic_remap();
    idt_set_gate(0x21, isr_keyboard, 0x8E); // IRQ1
    idt_set_gate(0x2C, isr_mouse,    0x8E); // IRQ12
    idt_load();
    pic_unmask_irq(1);
    pic_unmask_irq(12);
}

void kernel_main(void) {
    // Graphics
    set_graphics_mode();
    set_palette();
    clear_framebuffer(0x01);

    // Disk + filesystem (real persistence via ATA)
    ata_init();
    load_fs();

    // Interrupts + input devices
    init_interrupts();
    keyboard_init();      // initialize key state, if implemented
    init_mouse();         // enable PS/2 mouse + 4-byte packets

    // Splash
    draw_splash();
    for (volatile int i = 0; i < 2000000; i++) {}

    // Desktop boot
    init_desktop();

    // Main loop
    while (1) {
        update_windows();     // drag/close/redraw
        // Per-app updates (e.g., update_files()) can be called here
        draw_mouse_cursor();  // draw cursor last for proper z-order
    }
}