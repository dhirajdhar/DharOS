#include "../apps/snake.h"
#include "../window.h"
#include "../vga.h"
#include "../keyboard.h"

#define WIDTH 40
#define HEIGHT 20
#define CELL_SIZE 8
#define MAX_SNAKE 100

typedef struct {
    int x, y;
} Point;

static Point snake[MAX_SNAKE];
static int length = 5;
static int dir = 0; // 0=right, 1=down, 2=left, 3=up
static Point apple;
static int score = 0;

// 🔧 DD-style RNG (no stdlib)
static unsigned int dd_seed = 123456;
int dd_rand() {
    dd_seed = dd_seed * 1103515245 + 12345;
    return (dd_seed >> 16) & 0x7FFF;
}

void draw_board() {
    fill_screen(0x01); // Blue background
    draw_string(20, 10, "Snake Game — Use arrow keys. F1 to quit.", 0x0F);

    // Draw apple
    int ax = apple.x * CELL_SIZE;
    int ay = apple.y * CELL_SIZE + 30;
    for (int y = 0; y < CELL_SIZE; y++) {
        for (int x = 0; x < CELL_SIZE; x++) {
            put_pixel(ax + x, ay + y, 0x04); // Red apple
        }
    }

    // Draw snake
    for (int i = 0; i < length; i++) {
        int sx = snake[i].x * CELL_SIZE;
        int sy = snake[i].y * CELL_SIZE + 30;
        unsigned char color = (i == 0) ? 0x0F : 0x0A; // Head white, body green
        for (int y = 0; y < CELL_SIZE; y++) {
            for (int x = 0; x < CELL_SIZE; x++) {
                put_pixel(sx + x, sy + y, color);
            }
        }
    }
}

void move_snake() {
    for (int i = length - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }
    if (dir == 0) snake[0].x++;
    if (dir == 1) snake[0].y++;
    if (dir == 2) snake[0].x--;
    if (dir == 3) snake[0].y--;
}

int check_collision() {
    if (snake[0].x < 0 || snake[0].x >= WIDTH || snake[0].y < 0 || snake[0].y >= HEIGHT) return 1;
    for (int i = 1; i < length; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) return 1;
    }
    return 0;
}

void spawn_apple() {
    apple.x = dd_rand() % WIDTH;
    apple.y = dd_rand() % HEIGHT;
}

void launch_snake() {
    set_graphics_mode();
    open_window("Snake");

    length = 5;
    dir = 0;
    score = 0;
    for (int i = 0; i < length; i++) {
        snake[i].x = 10 - i;
        snake[i].y = 10;
    }
    spawn_apple();

    while (1) {
        unsigned char sc = read_scancode();
        if (sc == 0x3B) break; // F1 to quit
        if (sc == 0x48) dir = 3; // up
        if (sc == 0x50) dir = 1; // down
        if (sc == 0x4B) dir = 2; // left
        if (sc == 0x4D) dir = 0; // right

        move_snake();
        if (check_collision()) {
            draw_string(20, 180, "Game Over!", 0x0F);
            break;
        }

        if (snake[0].x == apple.x && snake[0].y == apple.y) {
            length++;
            score++;
            spawn_apple();
        }

        draw_board();
    }
}