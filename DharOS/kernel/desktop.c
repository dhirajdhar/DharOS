#include "desktop.h"
#include "vga.h"
#include "keyboard.h"

#include "../apps/files.h"
#include "../apps/ddtexts.h"
#include "../apps/calculator.h"
#include "../apps/snake.h"
#include "../apps/trash.h"

// --- Icon drawing helpers ---
static void draw_icon_files(int x, int y) {
    fill_rect(x, y, 32, 32, 0x07);          // grey folder
    fill_rect(x, y, 32, 8, 0x0E);           // yellow tab
    draw_string(x, y + 34, "Files", 0x0F);
}

static void draw_icon_ddtexts(int x, int y) {
    fill_rect(x, y, 32, 32, 0x01);          // blue square
    // white vertical lines
    for (int i = 4; i < 28; i += 6) {
        draw_vline(x + i, y + 4, 24, 0x0F);
    }
    draw_string(x + 4, y + 10, "DD", 0x0F); // DD letters
    draw_string(x, y + 34, "DD Texts", 0x0F);
}

static void draw_icon_calculator(int x, int y) {
    fill_rect(x, y, 32, 32, 0x02);          // green body
    // buttons grid
    for (int row = 6; row < 28; row += 8) {
        for (int col = 6; col < 28; col += 8) {
            fill_rect(x + col, y + row, 4, 4, 0x0F);
        }
    }
    draw_string(x - 2, y + 34, "Calc", 0x0F);
}

static void draw_icon_snake(int x, int y) {
    fill_rect(x, y, 32, 32, 0x04);          // red background
    // snake body segments
    fill_rect(x + 4, y + 4, 6, 6, 0x0F);
    fill_rect(x + 12, y + 4, 6, 6, 0x0F);
    fill_rect(x + 20, y + 4, 6, 6, 0x0F);
    fill_rect(x + 20, y + 12, 6, 6, 0x0F);
    draw_string(x - 2, y + 34, "Snake", 0x0F);
}

static void draw_icon_trash(int x, int y) {
    fill_rect(x, y, 32, 32, 0x08);          // dark grey bin
    fill_rect(x, y, 32, 6, 0x0F);           // white lid
    draw_string(x - 2, y + 34, "Trash", 0x0F);
}

// --- Desktop initialization ---
void init_desktop() {
    set_graphics_mode();         // Switch to graphics mode
    set_palette();
    fill_screen(0x01);           // Blue background

    // Title bar
    fill_rect(0, 0, SCREEN_WIDTH, 20, 0x0F);
    draw_string(8, 6, "DharOS Desktop", 0x00);

    // Draw icons in a grid
    draw_icon_files(40, 60);
    draw_icon_ddtexts(100, 60);
    draw_icon_calculator(160, 60);
    draw_icon_snake(220, 60);
    draw_icon_trash(280, 60);

    // Instruction line
    draw_string(20, SCREEN_HEIGHT - 20,
                "Press F/D/C/S/T to launch apps, ESC to exit.", 0x0F);
}

// --- Desktop loop ---
void desktop_loop() {
    while (1) {
        char key = read_key();
        if (key == 0) continue;

        if (key == 'f' || key == 'F') {
            launch_files();
        } else if (key == 'd' || key == 'D') {
            launch_ddtexts();
        } else if (key == 'c' || key == 'C') {
            launch_calculator();
        } else if (key == 's' || key == 'S') {
            launch_snake();
        } else if (key == 't' || key == 'T') {
            launch_trash();
        } else if (read_scancode() == 0x01) { // ESC
            fill_screen(0x01);
            draw_string(20, 180, "Exiting DharOS...", 0x0F);
            break;
        }
    }
}
