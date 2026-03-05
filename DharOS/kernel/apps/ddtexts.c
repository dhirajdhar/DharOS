#include "../apps/ddtexts.h"
#include "../window.h"
#include "../vga.h"
#include "../fs.h"
#include "../keyboard.h"
#include "../mouse.h"
#include <string.h>

#define MAX_TEXT_LENGTH 1024
static char buffer[MAX_TEXT_LENGTH];
static int cursor = 0;

static char filename_input[MAX_FILENAME];
static int filename_cursor = 0;

static Window* ddtexts_win;

// --- UI helpers ---
static void draw_button(int x, int y, int w, int h, const char* label, unsigned char color) {
    fill_rect(x, y, w, h, color);
    draw_rect(x, y, w, h, 0x0F);
    draw_string(x + 6, y + 6, label, 0x0F);
}

static int mouse_in_region(int mx, int my, int x, int y, int w, int h) {
    return (mx >= x && mx <= x + w && my >= y && my <= y + h);
}

// --- Main editor ---
void launch_ddtexts() {
    ddtexts_win = open_window("DD Texts");

    cursor = 0;
    memset(buffer, 0, MAX_TEXT_LENGTH);
    filename_cursor = 0;
    memset(filename_input, 0, MAX_FILENAME);

    while (!ddtexts_win->closed) {
        int mx = get_mouse_x();
        int my = get_mouse_y();
        int left_click = get_mouse_left();

        // Draw UI
        fill_rect(ddtexts_win->x + 10, ddtexts_win->y + 30, ddtexts_win->w - 20, ddtexts_win->h - 40, 0x01);

        // Filename input box
        fill_rect(ddtexts_win->x + 10, ddtexts_win->y + 30, 140, 20, 0x07);
        draw_string(ddtexts_win->x + 14, ddtexts_win->y + 34, filename_input, 0x0F);

        // Buttons
        draw_button(ddtexts_win->x + 160, ddtexts_win->y + 30, 60, 20, "Save", 0x02);
        draw_button(ddtexts_win->x + 230, ddtexts_win->y + 30, 60, 20, "Open", 0x04);

        // Text editor area
        fill_rect(ddtexts_win->x + 10, ddtexts_win->y + 60, ddtexts_win->w - 20, ddtexts_win->h - 70, 0x03);
        draw_string(ddtexts_win->x + 14, ddtexts_win->y + 64, buffer, 0x0F);

        // --- Keyboard input ---
        char key = read_key();
        unsigned char sc = read_scancode();

        // Typing into filename input if cursor is there
        if (my >= ddtexts_win->y + 30 && my <= ddtexts_win->y + 50 && mx >= ddtexts_win->x + 10 && mx <= ddtexts_win->x + 150) {
            if (key == '\b' && filename_cursor > 0) {
                filename_cursor--;
                filename_input[filename_cursor] = '\0';
            } else if (key != 0 && key != '\n') {
                filename_input[filename_cursor++] = key;
                filename_input[filename_cursor] = '\0';
            }
        } else {
            // Typing into main editor
            if (key == '\b' && cursor > 0) {
                cursor--;
                buffer[cursor] = '\0';
            } else if (key == '\n') {
                buffer[cursor++] = '\n';
                buffer[cursor] = '\0';
            } else if (key != 0) {
                buffer[cursor++] = key;
                buffer[cursor] = '\0';
            }
        }

        // --- Mouse clicks on buttons ---
        if (left_click) {
            // Save button
            if (mouse_in_region(mx, my, ddtexts_win->x + 160, ddtexts_win->y + 30, 60, 20)) {
                if (filename_cursor > 0) {
                    char fullpath[MAX_FILENAME];
                    snprintf(fullpath, MAX_FILENAME, "%s.ddtxt", filename_input);
                    write_file(fullpath, buffer, cursor);
                    draw_string(ddtexts_win->x + 10, ddtexts_win->y + ddtexts_win->h - 20, "Saved!", 0x0F);
                }
            }
            // Open button
            if (mouse_in_region(mx, my, ddtexts_win->x + 230, ddtexts_win->y + 30, 60, 20)) {
                // Launch a Files-like window to choose
                Window* open_win = open_window("Open File");
                int ox = open_win->x + 20;
                int oy = open_win->y + 30;
                int count = 0;
                for (int i = 0; i < MAX_FILES; i++) {
                    if (files[i].used) {
                        draw_string(ox, oy, files[i].name, 0x0F);
                        oy += 12;
                        count++;
                    }
                }
                if (count == 0) {
                    draw_string(ox, oy, "No files found.", 0x0F);
                }
                // Simple click-to-open
                if (get_mouse_left()) {
                    for (int i = 0; i < MAX_FILES; i++) {
                        if (files[i].used &&
                            mouse_in_region(mx, my, ox, open_win->y + 30 + i*12, 120, 12)) {
                            unsigned int size;
                            char* data = read_file(files[i].name, &size);
                            if (data) {
                                memcpy(buffer, data, size);
                                cursor = size;
                            }
                        }
                    }
                }
            }
        }

        if (cursor >= MAX_TEXT_LENGTH) cursor = MAX_TEXT_LENGTH - 1;
    }
}