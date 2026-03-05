#include "window.h"
#include "vga.h"
#include "mouse.h"
#include "keyboard.h"
#include <string.h>

#define MAX_WINDOWS 8
#define CHAR_WIDTH 8
#define CHAR_HEIGHT 8

static Window windows[MAX_WINDOWS];
static int window_count = 0;
static int mouse_pressed = 0;

void open_window(const char* title) {
    if (window_count >= MAX_WINDOWS) return;
    Window* win = &windows[window_count++];
    strncpy(win->title, title, 32);
    win->x = 40 + window_count * 20;
    win->y = 40 + window_count * 20;
    win->w = 200;
    win->h = 120;
    win->focused = 1;
    win->dragging = 0;
    win->closed = 0;
    draw_window(win);
}

void draw_window(Window* win) {
    if (win->closed) return;

    // Window body
    fill_rect(win->x, win->y, win->w, win->h, 0x03); // Cyan body

    // Border
    draw_rect(win->x, win->y, win->w, win->h, 0x0F);

    // Title bar (top strip)
    fill_rect(win->x, win->y, win->w, 16, 0x01); // Blue title bar
    draw_string(win->x + 20, win->y + 4, win->title, 0x0F);

    // Logo area (small square at left of title bar)
    fill_rect(win->x + 2, win->y + 2, 12, 12, 0x0F);

    // Close button at bottom-left
    fill_rect(win->x, win->y + win->h - 16, 32, 16, 0x04); // Red button
    draw_string(win->x + 10, win->y + win->h - 12, "X", 0x0F);
}

void update_windows() {
    int mx = get_mouse_x();
    int my = get_mouse_y();
    int left_click = get_mouse_left();

    for (int i = 0; i < window_count; i++) {
        Window* win = &windows[i];
        if (win->closed) continue;

        // --- Close button detection ---
        if (left_click &&
            mx >= win->x && mx <= win->x + 32 &&
            my >= win->y + win->h - 16 && my <= win->y + win->h) {
            win->closed = 1;
            continue;
        }

        // --- Dragging logic ---
        if (left_click && !mouse_pressed &&
            mx >= win->x && mx <= win->x + win->w &&
            my >= win->y && my <= win->y + 16) {
            win->dragging = 1;
            mouse_pressed = 1;
        }

        if (!left_click && mouse_pressed) {
            win->dragging = 0;
            mouse_pressed = 0;
        }

        if (win->dragging) {
            win->x = mx - win->w / 2;
            win->y = my - 8;
            if (win->x < 0) win->x = 0;
            if (win->y < 20) win->y = 20; // keep below title bar
        }

        draw_window(win);
    }
}
