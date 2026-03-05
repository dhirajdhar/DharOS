#include "keyboard.h"
#include "io.h"          // add this
#include <stdbool.h>
#include <stdint.h>

extern volatile uint64_t system_ticks; // maintained by PIT

#define REPEAT_DELAY 1000   // ms before repeat starts
#define REPEAT_RATE    50   // ms between repeats once started

typedef struct {
    bool pressed;
    uint64_t press_time;
    uint64_t last_repeat;
} KeyState;

static KeyState key_states[128];

static const char scancode_map[128] = {
    0, 27,'1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,'\\','z','x','c','v','b','n','m',',','.','/',0,
    '*',0,' ',0, // expand further if needed
};

unsigned char read_scancode() {
    return inb(0x60);
}

char read_key() {
    unsigned char sc = read_scancode();
    uint64_t now = system_ticks; // use PIT ticks

    if (sc & 0x80) {
        unsigned char code = sc & 0x7F;
        key_states[code].pressed = false;
        return 0;
    }

    if (sc < 128) {
        KeyState *ks = &key_states[sc];
        char ch = scancode_map[sc];

        if (!ks->pressed) {
            ks->pressed = true;
            ks->press_time = now;
            ks->last_repeat = 0;
            return ch; // immediate
        } else {
            if (now - ks->press_time >= REPEAT_DELAY) {
                if (now - ks->last_repeat >= REPEAT_RATE) {
                    ks->last_repeat = now;
                    return ch;
                }
            }
        }
    }
    return 0;
}

void keyboard_init() {
    // Clear key_states array, or set up IRQ1 handler if you want
    for (int i = 0; i < 128; i++) {
        key_states[i].pressed = false;
        key_states[i].press_time = 0;
        key_states[i].last_repeat = 0;
    }
}