#ifndef __KAIROS_TIMER
#define __KAIROS_TIMER

#include "Arduino.h"
#include "Device.h"

static uint8_t _channel_pin_map[2] = {10, 9};

class TimerDevice : public PinDevice
{
public:
  enum tm_channel_t { ChannelA, ChannelB };

  TimerDevice(tm_channel_t ch=ChannelA, uint16_t on=0, uint16_t off=0) : 
      PinDevice(_channel_pin_map[ch]),
      channel(ch), 
      on_time(on), 
      off_time(off)
  {}

public:
  tm_channel_t channel;
  volatile uint16_t on_time;
  volatile uint16_t off_time;
};

extern TimerDevice channel_a;
extern TimerDevice channel_b;

class EventTimer
{
public:
  EventTimer() { reset(); }

  static void start(void)
  {
      cli();
      //ch_a_timestamp = 0;
      //ch_b_timestamp = 0;
      ICR1 = 0xFFFF;
      OCR1A = channel_a.on_time;
      OCR1B = channel_b.on_time;
      TCNT1 = 0;
      TCCR1A = bit(COM1A1) | bit(COM1A0) | bit(COM1B1) | bit(COM1B0);
      TCCR1B = bit(WGM12) | bit(WGM13) | bit(CS12);
      TIMSK1 = bit(OCIE1A) | bit(OCIE1B);
      sei();
  }

  void reset()
  {
      cli();
      TCCR1A = TCCR1B = 0;
      TCNT1 = 0;
      sei();
  }

public:
  //uint16_t channel_a_timestamp = 0;
  //uint16_t channel_b_timestamp = 0;
};

extern EventTimer event_timer;
#endif // __KAIROS_TIMER
