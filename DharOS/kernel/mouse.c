#include "mouse.h"
#include "io.h"          // add this
#include <stdbool.h>

int mouse_x = 160; // Start at center of 320x200 screen
int mouse_y = 100;

static int mouse_cycle = 0;
static unsigned char mouse_bytes[4];
static int mouse_ready = 0;

// Button states
bool mouse_left = false;
bool mouse_right = false;
int mouse_scroll = 0;

int get_mouse_x() { return mouse_x; }
int get_mouse_y() { return mouse_y; }
bool get_mouse_left() { return mouse_left; }
bool get_mouse_right() { return mouse_right; }
int get_mouse_scroll() { return mouse_scroll; }

void mouse_wait(unsigned char type) {
    unsigned int timeout = 100000;
    if (type == 0) {
        while (timeout-- && !(inb(0x64) & 1));
    } else {
        while (timeout-- && (inb(0x64) & 2));
    }
}

void mouse_write(unsigned char data) {
    mouse_wait(1);
    outb(0x64, 0xD4);
    mouse_wait(1);
    outb(0x60, data);
}

unsigned char mouse_read() {
    mouse_wait(0);
    return inb(0x60);
}

void init_mouse() {
    outb(0x64, 0xA8);           // Enable auxiliary device
    mouse_write(0xF6);          // Set defaults
    mouse_read();               // Acknowledge
    mouse_write(0xF4);          // Enable packet streaming
    mouse_read();               // Acknowledge

    // Enable scroll wheel (IntelliMouse extension)
    mouse_write(0xF3);          // Set sample rate
    mouse_read();
    mouse_write(200);           // Magic sequence
    mouse_read();

    mouse_write(0xF3);
    mouse_read();
    mouse_write(100);
    mouse_read();

    mouse_write(0xF3);
    mouse_read();
    mouse_write(80);
    mouse_read();

    // After this, packets are 4 bytes (with scroll)
    mouse_ready = 1;
}

void handle_mouse_interrupt() {
    if (!mouse_ready) return;

    unsigned char status = inb(0x64);
    if (!(status & 1)) return;

    unsigned char data = inb(0x60);
    mouse_bytes[mouse_cycle++] = data;

    // For scroll‑enabled mice, packets are 4 bytes
    if (mouse_cycle == 4) {
        mouse_cycle = 0;

        unsigned char flags = mouse_bytes[0];
        char dx = (char)mouse_bytes[1];
        char dy = (char)mouse_bytes[2];
        char dz = (char)mouse_bytes[3]; // scroll wheel

        mouse_x += dx;
        mouse_y -= dy;

        if (mouse_x < 0) mouse_x = 0;
        if (mouse_x > 319) mouse_x = 319;
        if (mouse_y < 0) mouse_y = 0;
        if (mouse_y > 199) mouse_y = 199;

        // Buttons: bit0 = left, bit1 = right, bit2 = middle
        mouse_left  = flags & 0x01;
        mouse_right = flags & 0x02;

        // Scroll: positive = up, negative = down
        mouse_scroll = dz;
    }
}