#include "copy.h"
#include "stylus.h"
#include "progress.h"

void installer_main(void) {
    show_progress("Installing DharOS...");
    wait_for_stylus();  // Stylus-triggered install
    copy_kernel(0);
    copy_grub_config(0);
    copy_ddsuite(0);
    show_progress("Installation complete. Rebooting...");
    reboot();
}