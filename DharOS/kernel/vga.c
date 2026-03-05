#include "vga.h"
#include "font8x8_basic.h"
#include <stdint.h>

// If you have a mouse header that exposes mouse_x/mouse_y, include it.
// Or forward-declare getters:
int get_mouse_x(void);
int get_mouse_y(void);

#define VIDEO_MEMORY ((unsigned char*)0xA0000)

// --- Internal helpers for Mode X ---
static inline void set_map_mask(unsigned char mask) {
    outb(0x3C4, 0x02); // Sequencer Map Mask
    outb(0x3C5, mask);
}

static inline void select_plane(unsigned char plane) {
    static const unsigned char masks[4] = { 0x01, 0x02, 0x04, 0x08 };
    set_map_mask(masks[plane & 3]);
}

static inline unsigned char plane_of_x(int x) {
    return (unsigned char)(x & 3);
}

static inline int byte_offset_for_xy(int x, int y) {
    // Each plane holds every 4th pixel horizontally
    return y * (SCREEN_WIDTH / 4) + (x >> 2);
}

// --- Mode X setup: 320x240 (planar, unchained) ---
void set_graphics_mode() {
    // Reset AC flip-flop
    (void)inb(0x3DA);

    // Misc Output: 25.175 MHz, color, A000
    outb(0x3C2, 0x63);

    // Sequencer: enter unchained planar (Mode X)
    outb(0x3C4, 0x00); outb(0x3C5, 0x03); // sync reset
    outb(0x3C4, 0x01); outb(0x3C5, 0x01); // clocking
    outb(0x3C4, 0x02); outb(0x3C5, 0x0F); // map mask: planes 0..3 enabled
    outb(0x3C4, 0x03); outb(0x3C5, 0x00); // char map select
    outb(0x3C4, 0x04); outb(0x3C5, 0x06); // memory mode: unchained, odd/even off

    // Unlock CRTC
    outb(0x3D4, 0x11); outb(0x3D5, inb(0x3D5) & 0x7F);

    // Base CRTC (Mode X baseline), then patch height/stride
    unsigned char crtc[] = {
        0x5F, // 00 H total
        0x4F, // 01 H display end
        0x50, // 02 H blank start
        0x82, // 03 H blank end
        0x54, // 04 H retrace start
        0x80, // 05 H retrace end
        0xBF, // 06 V total (low baseline)
        0x1F, // 07 Overflow (patched later)
        0x00, // 08 Preset row scan
        0x40, // 09 Max scan line (no doublescan)
        0x00, // 0A Cursor start
        0x00, // 0B Cursor end
        0x00, // 0C Start address high
        0x00, // 0D Start address low
        0x00, // 0E Cursor loc high
        0x00, // 0F Cursor loc low
        0x9C, // 10 V retrace start
        0x0E, // 11 V retrace end
        0x8F, // 12 V display end (low baseline)
        0x28, // 13 Offset => 320 bytes per scanline
        0x00, // 14 Underline location
        0x96, // 15 V blank start
        0xB9, // 16 V blank end
        0xA3, // 17 Mode control (safe baseline for Mode X)
        0xFF  // 18 Line Compare
    };
    for (int i = 0; i < 25; i++) {
        outb(0x3D4, i); outb(0x3D5, crtc[i]);
    }

    // Extend visible height for 240 lines (VDE = 239 = 0xEF), adjust totals
    outb(0x3D4, 0x12); outb(0x3D5, 0xEF); // VDisplayEnd low
    outb(0x3D4, 0x06); outb(0x3D5, 0x0D); // VTotal low (~0x10D total)

    // Overflow bits: set VT9, VDE9, VBlank9, VT8, VDE8 as needed
    outb(0x3D4, 0x07); outb(0x3D5, 0x5F);

    // Ensure stride stays 320 bytes/scanline
    outb(0x3D4, 0x13); outb(0x3D5, 0x28);

    // Disable line compare completely (prevent mid-screen switch)
    outb(0x3D4, 0x18); outb(0x3D5, 0xFF);   // LC low = 0xFF (out of range)
    outb(0x3D4, 0x09); outb(0x3D5, 0x80);   // bit7=1 (LC9), bit6=0 (no doublescan)

    // Start address = 0
    outb(0x3D4, 0x0C); outb(0x3D5, 0x00);
    outb(0x3D4, 0x0D); outb(0x3D5, 0x00);

    // Graphics Controller: planar writes, write mode 0, map at A000, 256-color
    outb(0x3CE, 0x00); outb(0x3CF, 0x00); // Set/Reset
    outb(0x3CE, 0x01); outb(0x3CF, 0x00); // Enable Set/Reset
    outb(0x3CE, 0x02); outb(0x3CF, 0x00); // Color Compare
    outb(0x3CE, 0x03); outb(0x3CF, 0x00); // Data Rotate
    outb(0x3CE, 0x04); outb(0x3CF, 0x00); // Read Map Select
    outb(0x3CE, 0x05); outb(0x3CF, 0x00); // Graphics Mode: write mode 0
    outb(0x3CE, 0x06); outb(0x3CF, 0x05); // Misc: A000, graphics, 256-color, chain off
    outb(0x3CE, 0x07); outb(0x3CF, 0x0F); // Color don't care
    outb(0x3CE, 0x08); outb(0x3CF, 0xFF); // Bit mask

    // Attribute Controller: program with flip-flop resets
    (void)inb(0x3DA);
    for (int i = 0; i < 16; i++) {
        outb(0x3C0, i); outb(0x3C0, i);
        (void)inb(0x3DA);
    }
    outb(0x3C0, 0x10); outb(0x3C0, 0x41); // Mode control (graphics, 8-bit)
    (void)inb(0x3DA);
    outb(0x3C0, 0x11); outb(0x3C0, 0x00); // Overscan
    (void)inb(0x3DA);
    outb(0x3C0, 0x12); outb(0x3C0, 0x0F); // Color plane enable (all planes)
    (void)inb(0x3DA);
    outb(0x3C0, 0x13); outb(0x3C0, 0x00); // Horizontal panning
    (void)inb(0x3DA);
    outb(0x3C0, 0x14); outb(0x3C0, 0x00); // Color select

    // Re-enable Attribute Controller
    (void)inb(0x3DA);
    outb(0x3C0, 0x20);

    // Enable all planes for initial writes
    set_map_mask(0x0F);

    // Start from clean VRAM (black)
    for (int p = 0; p < 4; ++p) {
        select_plane((unsigned char)p);
        for (int y = 0; y < SCREEN_HEIGHT; ++y) {
            int base = y * (SCREEN_WIDTH / 4);
            for (int bx = 0; bx < (SCREEN_WIDTH / 4); ++bx) {
                VIDEO_MEMORY[base + bx] = 0x00;
            }
        }
    }
}

// --- Planar pixel ops ---
void put_pixel(int x, int y, unsigned char color) {
    if (x < 0 || y < 0 || x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT) return;
    unsigned char plane = plane_of_x(x);
    int offset = byte_offset_for_xy(x, y);
    select_plane(plane);
    VIDEO_MEMORY[offset] = color;
}

void draw_hline(int x, int y, int w, unsigned char color) {
    if (y < 0 || y >= SCREEN_HEIGHT || w <= 0) return;
    int x0 = x < 0 ? 0 : x;
    int x1 = x + w; if (x1 > SCREEN_WIDTH) x1 = SCREEN_WIDTH;
    for (int px = x0; px < x1; ++px) put_pixel(px, y, color);
}

void draw_vline(int x, int y, int h, unsigned char color) {
    if (x < 0 || x >= SCREEN_WIDTH || h <= 0) return;
    int y0 = y < 0 ? 0 : y;
    int y1 = y + h; if (y1 > SCREEN_HEIGHT) y1 = SCREEN_HEIGHT;
    for (int py = y0; py < y1; ++py) put_pixel(x, py, color);
}

void draw_rect(int x, int y, int w, int h, unsigned char color) {
    if (w <= 0 || h <= 0) return;
    draw_hline(x, y, w, color);
    draw_hline(x, y + h - 1, w, color);
    draw_vline(x, y, h, color);
    draw_vline(x + w - 1, y, h, color);
}

// --- Plane-batched fast fills (solid, no striping) ---
void fill_rect_fast(int x, int y, int w, int h, unsigned char color) {
    if (w <= 0 || h <= 0) return;

    int x0 = x < 0 ? 0 : x;
    int y0 = y < 0 ? 0 : y;
    int x1 = x + w; if (x1 > SCREEN_WIDTH) x1 = SCREEN_WIDTH;
    int y1 = y + h; if (y1 > SCREEN_HEIGHT) y1 = SCREEN_HEIGHT;
    if (x0 >= x1 || y0 >= y1) return;

    for (int py = y0; py < y1; ++py) {
        for (int plane = 0; plane < 4; ++plane) {
            int xs = x0;
            int m = xs & 3;
            if (m != plane) xs += (plane - m + 4) & 3;
            if (xs >= x1) continue;

            select_plane((unsigned char)plane);
            for (int px = xs; px < x1; px += 4) {
                int off = byte_offset_for_xy(px, py);
                VIDEO_MEMORY[off] = color;
            }
        }
    }
}

void fill_rect(int x, int y, int w, int h, unsigned char color) {
    fill_rect_fast(x, y, w, h, color);
}

void fill_screen(unsigned char color) {
    fill_rect_fast(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, color);
}

// --- Clear all planes to a color (use for full-screen clears) ---
void clear_framebuffer(unsigned char color) {
    for (int p = 0; p < 4; ++p) {
        select_plane((unsigned char)p);
        for (int y = 0; y < SCREEN_HEIGHT; ++y) {
            int base = y * (SCREEN_WIDTH / 4);
            for (int bx = 0; bx < (SCREEN_WIDTH / 4); ++bx) {
                VIDEO_MEMORY[base + bx] = color;
            }
        }
    }
}

// --- Simple 8x8 text (optional, for labels) ---
void draw_char(int x, int y, char c, unsigned char color) {
    if ((unsigned char)c > 127) return;
    for (int row = 0; row < 8; row++) {
        unsigned char bits = font8x8_basic[(int)c][row];
        for (int col = 0; col < 8; col++) {
            if (bits & (1 << col)) put_pixel(x + col, y + row, color);
        }
    }
}

void draw_string(int x, int y, const char* str, unsigned char color) {
    while (*str) {
        draw_char(x, y, *str++, color);
        x += 8;
    }
}

// --- Palette: DD blue + bright white + distinct test colors ---
void set_palette() {
    // DD blue at index 0x01 (true blue)
    (void)inb(0x3DA);
    outb(0x3C8, 0x01);
    outb(0x3C9, 0x00); // R
    outb(0x3C9, 0x00); // G
    outb(0x3C9, 0x3F); // B

    // Bright white at index 0x0F
    (void)inb(0x3DA);
    outb(0x3C8, 0x0F);
    outb(0x3C9, 0x3F); // R
    outb(0x3C9, 0x3F); // G
    outb(0x3C9, 0x3F); // B

    // Fill remaining 0..15 with test colors (skip 0x01 and 0x0F)
    for (int i = 0; i < 16; i++) {
        if (i == 0x01 || i == 0x0F) continue;
        (void)inb(0x3DA);
        outb(0x3C8, i);
        outb(0x3C9, (i * 4) % 64); // R
        outb(0x3C9, (i * 2) % 64); // G
        outb(0x3C9, (i * 3) % 64); // B
    }
}

/* ---------------- Cursor rendering ---------------- */

// 9x13 arrow cursor bitmap (1 = pixel on)
static const uint16_t cursor_bitmap[13] = {
    0b100000000,
    0b110000000,
    0b111000000,
    0b111100000,
    0b111110000,
    0b111111000,
    0b111111100,
    0b111111110,
    0b111111111,
    0b111110000,
    0b110110000,
    0b000110000,
    0b000011000
};

void draw_cursor(int x, int y) {
    unsigned char color = 0x0F; // white
    for (int row = 0; row < 13; ++row) {
        int py = y + row;
        if (py < 0 || py >= SCREEN_HEIGHT) continue;
        unsigned char bits_low = (unsigned char)(cursor_bitmap[row] & 0xFF);
        unsigned char bit9 = (cursor_bitmap[row] >> 8) & 0x01;
        for (int col = 0; col < 8; ++col) {
            if (bits_low & (1 << (7 - col))) {
                int px = x + col;
                if (px >= 0 && px < SCREEN_WIDTH) put_pixel(px, py, color);
            }
        }
        if (bit9) {
            int px = x + 8;
            if (px >= 0 && px < SCREEN_WIDTH) put_pixel(px, py, color);
        }
    }
}

// Helper to be called after your desktop draw to place the cursor
void draw_mouse_cursor() {
    int mx = get_mouse_x();
    int my = get_mouse_y();
    draw_cursor(mx, my);
}