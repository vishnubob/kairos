#include "EventTimer.h"

EventTimer kairos_timer;

ISR(TIMER1_COMPB_vect)
{
    if (TCCR1A & bit(COM1B0))
    {
      TCCR1A &= ~bit(COM1B0);
      ch_b_timestamp = TCNT1;
      OCR1B = kairos_timer.channel_b.off_time;
    }
}

ISR(TIMER1_COMPA_vect)
{
    if (TCCR1A & bit(COM1A0))
    {
      TCCR1A &= ~bit(COM1A0);
      OCR1A = kairos_timer.channel_a.off_time;
    }
}


