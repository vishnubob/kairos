#ifndef _KAIROS_CONTROL_H
#define _KAIROS_CONTROL_H

#include "Device.h"
#include "Trigger.h"

#define PIN_FLASH_FIRE      10 // port a
#define PIN_CAMERA_SHUTTER  9 // port b
#define PIN_CAMERA_FOCUS    4
#define PIN_LASER_POWER     3

class Control
{
public:
  void Control(ComparatorTrigger& _ldr, EventTimer& _timer)
    ldr(_ldr),
    timer(_timer),
    laser(PIN_LASER_POWER),
    focus(PIN_CAMERA_FOCUS)
  {}

  void arm (bool dryrun=false)
  {
      cli();
      if (!ldr.is_armed())
      {
        focus.on();
        laser.on();
        callback_t callback = dryrun ? NULL : (&(timer.start));
        ldr.set_callback(callback);
        _delay_ms(500);
        ldr.arm();
      }
      sei();
  }

  void disarm (void)
  {
      cli();
      if (ldr.is_armed()) 
      {
        ldr.disarm();
        laser.off();
        focus.off();
        timer.reset();
      }
      sei();
  }

  void refresh()
  {
    if(ldr.is_armed() && ldr.is_triggered())
    {
      if (laser.is_on()) laser.off();
      Serial.println("TRG");
      // total hack
      _delay_ms(1000);
      disarm();
    }
  }
};

public:
  ComparatorTrigger &ldr;
  EventTimer &timer;
  TimerDevice &shutter;
  TimerDevice &flash;
  PinDevice laser;
  PinDevice focus;
};

#endif // _KAIROS_CONTROL_H
