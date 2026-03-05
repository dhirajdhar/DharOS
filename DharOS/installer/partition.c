#include "partition.h"
#include "vga.h"

void create_partition_table() {
    fill_screen(0x01); // Blue background
    draw_string(20, 100, "🧭 Creating partition table...", 0x0F);

    // Stub: simulate writing MBR
    unsigned char mbr[512];
    for (int i = 0; i < 512; i++) mbr[i] = 0;
    mbr[510] = 0x55;
    mbr[511] = 0xAA;

    // Simulate writing to disk (replace with real syscall later)
    draw_string(20, 120, "✅ MBR signature written (0xAA55)", 0x0F);
}