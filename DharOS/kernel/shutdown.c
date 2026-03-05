#include "shutdown.h"
#include "io.h"
void shutdown() {
    outb(0x64, 0xFE);  // Try to shut down via keyboard controller
}