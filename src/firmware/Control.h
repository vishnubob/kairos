#ifndef _KAIROS_CONTROL_H
#define _KAIROS_CONTROL_H

#include "Arduino.h"
#include "Device.h"
#include "Trigger.h"
#include "Timer.h"

#define PIN_CAMERA_SHUTTER  9   // port a
#define PIN_FLASH_FIRE      10  // port b
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
    shutter(channel_a),
    flash(channel_b),
    mirror_lockup(false)
  {}

  void arm (bool dryrun=false)
  {
      if (!ldr.is_armed())
      {
        Trigger::callback_t callback = dryrun ? NULL : static_cast<Trigger::callback_t>(timer.start);
        cli();
        {
          timer.reset();
          ldr.enable();
          laser.on();
          ldr.set_callback(callback);
          if (!dryrun)
          {
            focus.on();
            if (mirror_lockup)
            {
              shutter.on();
              _delay_ms(100);
              shutter.off();
            } else 
          }
          _delay_ms(200);
          ldr.arm();
        }
        sei();
      }
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
      Serial.println("TRG");
      if (laser.is_on()) { laser.off(); }
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
  bool mirror_lockup;
};

#endif // _KAIROS_CONTROL_H
