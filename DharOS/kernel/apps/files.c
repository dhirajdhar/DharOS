#include "../fs.h"
#include "../vga.h"
#include "../window.h"
#include "../mouse.h"
#include "file.h"
#include <string.h>

extern File files[];

static Window* files_win;

void draw_file_icon(int x, int y, const char* name) {
    fill_rect(x, y, 32, 32, 0x07); // grey file icon
    draw_string(x + 2, y + 12, name, 0x0F);
}

void launch_files() {
    files_win = open_window("Files");

    int x = files_win->x + 20;
    int y = files_win->y + 30;
    int count = 0;

    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].used) {
            draw_file_icon(x, y, files[i].name);
            x += 40;
            count++;
            if (count % 6 == 0) { x = files_win->x + 20; y += 50; }
        }
    }
    if (count == 0) {
        draw_string(files_win->x + 20, files_win->y + 40, "No files found.", 0x0F);
    }
}

void update_files() {
    if (!files_win || files_win->closed) return;

    int mx = get_mouse_x();
    int my = get_mouse_y();

    if (get_mouse_left()) {
        // Check if clicked inside Files window
        if (mx >= files_win->x && mx <= files_win->x + files_win->w &&
            my >= files_win->y && my <= files_win->y + files_win->h) {

            // Determine which file icon was clicked
            int relx = mx - (files_win->x + 20);
            int rely = my - (files_win->y + 30);
            int col = relx / 40;
            int row = rely / 50;
            int index = row * 6 + col;

            if (index >= 0 && index < MAX_FILES && files[index].used) {
                // Open file
                unsigned int size;
                char* data = read_file(files[index].name, &size);
                if (data) {
                    Window* fw = open_window(files[index].name);
                    draw_string(fw->x + 10, fw->y + 30, data, 0x0F);
                }
            }
        }
    }
}
