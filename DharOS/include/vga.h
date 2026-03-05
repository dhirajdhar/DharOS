#ifndef VGA_H
#define VGA_H

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240

void set_graphics_mode();
void set_palette();
void clear_framebuffer(unsigned char color);

void put_pixel(int x, int y, unsigned char color);
void draw_rect(int x, int y, int w, int h, unsigned char color);
void fill_rect(int x, int y, int w, int h, unsigned char color);
void fill_rect_fast(int x, int y, int w, int h, unsigned char color);

void draw_hline(int x, int y, int w, unsigned char color);
void draw_string(int x, int y, const char* s, unsigned char color);

void draw_mouse_cursor();

#endif