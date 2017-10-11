#ifndef _KAIROS_CONTROL_H
#define _KAIROS_CONTROL_H

#include "Device.h"
#include "Trigger.h"

class Control
{
public:
  void Control(ComparatorTrigger& _ldr, EventTimer& _timer)
    ldr(_ldr),
    timer(_timer),
    laser(PIN_LASER_POWER)
  {}

  void arm (bool dryrun=false)
  {
      if (ldr.is_armed()) return;
      laser.on();
      callback_t cb = dryrun ? NULL : (&(timer.trigger));
      ldr.set_callback(cb);
      ldr.enable();
      _delay_ms(500);
      ldr.arm();
  }

  void disarm (void)
  {
      if (!ldr.is_armed()) return;
      ldr.disable();
      laser.off();
      timer.reset();
  }

public:
  Comparator &ldr;
  EventTimer &timer;
  PinDevice laser;
  PinDevice focus;
};

#endif // _KAIROS_CONTROL_H
