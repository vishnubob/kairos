#include "Trigger.h"
#include "Timer.h"
#include "Control.h"
#include "Frontend.h"

#define MAX_EXPOSURE_DURATION 0xFFFF

Control control(comparator_trigger, event_timer);
SerialFrontend frontend(control);

void setup ()
{
    Serial.begin(115200);
    Serial.println("Kairos v1.0.0");
    // XXX: HACK
    pinMode(9, OUTPUT);
    pinMode(10, OUTPUT);
}

void loop ()
{
  frontend.refresh();
  control.refresh();
}
