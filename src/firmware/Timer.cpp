#include "EventTimer.h"

TimerDevice channel_a(TimerDevice::ChannelA);
TimerDevice channel_b(TimerDevice::ChannelB);
EventTimer event_timer(channel_a, channel_b);

ISR(TIMER1_COMPA_vect)
{
    if (TCCR1A & bit(COM1A0))
    {
      TCCR1A &= ~bit(COM1A0);
      OCR1A = channel_a.off_time;
    }
}

ISR(TIMER1_COMPB_vect)
{
    if (TCCR1A & bit(COM1B0))
    {
      TCCR1A &= ~bit(COM1B0);
      OCR1B = channel_b.off_time;
    }
}

