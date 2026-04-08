#include "ui.h"
#include "screens.h"

void ui_init(void) {
    // This single function now builds the splash screen, 
    // the tileview interface, and automatically loads the splash screen to the display.
    create_screens();
}