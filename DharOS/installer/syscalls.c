#include "syscalls.h"
#include "../fs.h"
#include "../vga.h"

void syscall_write_file(int partition, const char* path, void* data, unsigned int size) {
    (void)partition;
    draw_string(20, 160, "💾 Writing file...", 0x0F);
    write_file(path, data, size);
    draw_string(20, 170, "✅ File written.", 0x0F);
}

static inline void outb(unsigned short port, unsigned char val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

void reboot() {
    draw_string(20, 180, "🔁 Rebooting system...", 0x0F);
    outb(0x64, 0xFE);  // Try reboot via keyboard controller

    // Fallback: infinite loop if reboot fails
    for (;;) {}
}