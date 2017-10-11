#include "Control.h"
#include "Frontend.h"

#define DEFAULT_EXPOSURE_DELAY 18289
#define DEFAULT_FLASH_DELAY (DEFAULT_EXPOSURE_DELAY - 5190)
#define DEFAULT_FLASH_DURATION (3125 * 2)
#define DEFAULT_EXPOSURE_DURATION (3125 * 2)
#define MAX_EXPOSURE_DURATION 0xFFFF

SerialFrontend frontend(control);

void setup ()
{
    Serial.begin(115200);
}

void loop ()
{
  frontend.refresh();
  control.refresh();
}
