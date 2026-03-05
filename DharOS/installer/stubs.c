#include "../vga.h"
#include "../fs.h"
#include "../keyboard.h"
#include "../io.h" // for inb()

unsigned char read_scancode() {
    return inb(0x60); // Real hardware input
}

// ✅ REMOVE these duplicate definitions:
#if 0
char* read_file(const char* path, unsigned int* out_size) {
    return fs_read(path, out_size);
}

void write_file(const char* path, void* data, unsigned int size) {
    fs_write(path, data, size);
}
#endif