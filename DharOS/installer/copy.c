#include "copy.h"
#include "../fs.h"
#include "../vga.h"
#include "syscalls.h"
#include "progress.h"

// Embedded asset declarations
extern unsigned char _binary_kernel_bin_start[];
extern unsigned char _binary_kernel_bin_end[];

extern unsigned char _binary_assets_grub_cfg_start[];
extern unsigned char _binary_assets_grub_cfg_end[];

extern unsigned char _binary_assets_ddsuite_ddpkg_start[];
extern unsigned char _binary_assets_ddsuite_ddpkg_end[];

void copy_kernel(int partition) {
    show_progress("Copying kernel...");
    unsigned int size = _binary_kernel_bin_end - _binary_kernel_bin_start;
    syscall_write_file(partition, "kernel.bin", _binary_kernel_bin_start, size);
}

void copy_grub_config(int partition) {
    show_progress("Copying GRUB config...");
    unsigned int size = _binary_assets_grub_cfg_end - _binary_assets_grub_cfg_start;
    syscall_write_file(partition, "grub.cfg", _binary_assets_grub_cfg_start, size);
}

void copy_ddsuite(int partition) {
    show_progress("Copying DD Suite...");
    unsigned int size = _binary_assets_ddsuite_ddpkg_end - _binary_assets_ddsuite_ddpkg_start;
    syscall_write_file(partition, "ddsuite.ddpkg", _binary_assets_ddsuite_ddpkg_start, size);
}

void copy_file(const char* src, const char* dest) {
    unsigned int size;
    char* data = read_file(src, &size);
    fill_screen(0x01); // Clear screen with blue
    if (size > 0) {
        write_file(dest, data, size);
        draw_string(20, 100, "Copied file.", 0x0F);
    } else {
        draw_string(20, 100, "Copy failed.", 0x0F);
    }
}

void copy_all_files() {
    // Example: copy "ddtexts.txt" to "ddtexts.bak"
    copy_file("ddtexts.txt", "ddtexts.bak");
}