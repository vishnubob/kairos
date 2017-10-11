#ifndef __KAIROS_TIMER
#define __KAIROS_TIMER

#include "Device.h"

class TimerDevice : public PinDevice
{
public:
  enum tm_channel_t { ChA, ChannelA; ChB, ChannelB };
  uint8_t const _channel_pin_map[] = {10, 9};

  TimerDevice(tm_channel_t tm_channel=ChA, uint16_t tm_ontime=0, uint16_t tm_offtime=0) : 
      channel(tm_channel), 
      on_time(tm_ontime), 
      off_time(tm_offtime)
      PinDevice(_channel_pin_map[channel])
  {}

public:
  tm_offset_t on_time;
  tm_offset_t off_time;
  tm_channel_t channel;
  tm_channel_t channel_pin;
};

class EventTimer
{
public:
  EventTimer(TimerDevice &ch_a, TimerDevice &ch_b):
    channel_a(ch_a),
    channel_b(ch_b)
  {}

  void trigger(void)
  {
      cli();
      ch_a_timestamp = 0;
      ch_b_timestamp = 0;
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
  TimerDevice &channel_a;
  TimerDevice &channel_b;
  uint16_t channel_a_timestamp = 0;
  uint16_t channel_b_timestamp = 0;

};

extern EventTimer kairos_timer;
#endif // __KAIROS_TIMER
