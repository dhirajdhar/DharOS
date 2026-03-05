#ifndef MOUSE_H
#define MOUSE_H

#include <stdbool.h>   // <-- add this

int get_mouse_x();
int get_mouse_y();
void init_mouse();
void handle_mouse_interrupt(); // IRQ12 handler should call this
bool get_mouse_left();
bool get_mouse_right();
int get_mouse_scroll();

extern int mouse_x;
extern int mouse_y;

#endif