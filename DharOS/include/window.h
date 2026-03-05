#ifndef WINDOW_H
#define WINDOW_H

#define MAX_WINDOWS 8

typedef struct {
    char title[32];
    int x, y, w, h;
    int focused;
    int dragging;
} Window;

void open_window(const char* title);
void draw_window(Window* win);
void update_windows();         // Handles mouse movement and dragging
void start_drag(int win_id);   // Manual drag trigger (optional)
void stop_drag(int win_id);    // Manual drag release (optional)

#endif