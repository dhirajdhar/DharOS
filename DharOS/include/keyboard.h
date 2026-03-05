#ifndef KEYBOARD_H
#define KEYBOARD_H

// Reads raw scancode from port 0x60
unsigned char read_scancode();

// Decodes scancode into ASCII character (returns 0 if unknown or non-printable)
char read_key();

void keyboard_init();

#endif