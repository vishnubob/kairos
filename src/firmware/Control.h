#ifndef _KAIROS_CONTROL_H
#define _KAIROS_CONTROL_H

#include "Arduino.h"
#include "Device.h"
#include "Trigger.h"
#include "Timer.h"

#define PIN_FLASH_FIRE      10 // port a
#define PIN_CAMERA_SHUTTER  9 // port b
#define PIN_CAMERA_FOCUS    4
#define PIN_LASER_POWER     3

class Control
{
public:
  Control(ComparatorTrigger& _ldr, EventTimer& _timer) :
    ldr(_ldr),
    timer(_timer),
    laser(PIN_LASER_POWER),
    focus(PIN_CAMERA_FOCUS),
    shutter(channel_b),
    flash(channel_a)
  {}

  void arm (bool dryrun=false)
  {
      cli();
      if (!ldr.is_armed())
      {
        ldr.enable();
        focus.on();
        laser.on();
        Trigger::callback_t callback = dryrun ? NULL : static_cast<Trigger::callback_t>(timer.start);
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

public:
  ComparatorTrigger &ldr;
  EventTimer &timer;
  TimerDevice &shutter;
  TimerDevice &flash;
  PinDevice laser;
  PinDevice focus;
};

#endif // _KAIROS_CONTROL_H
