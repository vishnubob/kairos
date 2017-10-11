#include "Trigger.h"
#include "Timer.h"
#include "Control.h"
#include "Frontend.h"

#define DEFAULT_EXPOSURE_DELAY 18289
#define DEFAULT_FLASH_DELAY (DEFAULT_EXPOSURE_DELAY - 5190)
#define DEFAULT_FLASH_DURATION (3125 * 2)
#define DEFAULT_EXPOSURE_DURATION (3125 * 2)
#define MAX_EXPOSURE_DURATION 0xFFFF

Control control(comparator_trigger, event_timer);
SerialFrontend frontend(control);

void setup ()
{
    Serial.begin(115200);
    Serial.println("Kairos v1.0.0");
}

void loop ()
{
  frontend.refresh();
  control.refresh();
}
