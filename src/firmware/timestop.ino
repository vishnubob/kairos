#include "Control.h"
#include "Parser.h"

Control control;
Parser parser(control);
volatile bool triggered = false;
volatile uint16_t exp_start = 0;
volatile uint16_t exp_stop = 0;

ISR (ANALOG_COMP_vect)
{
    if (!control.armed) return;
    ACSR = bit(ACD);
    // enable int on match with OCR1B
    triggered = true;
    if (control.dryrun) return;
    cli();
    ICR1 = MAX_EXPOSURE_DURATION;
    OCR1B = control.camera.exposure_delay;
    OCR1A = control.camera.flash_delay;
    TCNT1 = 0;
    TCCR1A = bit(COM1A1) | bit(COM1A0) | bit(COM1B1) | bit(COM1B0);
    TCCR1B = bit(WGM12) | bit(WGM13) | bit(CS12);
    TIMSK1 = bit(OCIE1A) | bit(OCIE1B);
    sei();
}

ISR(TIMER1_COMPB_vect)
{
    if (TCCR1A & bit(COM1B0))
    {
      TCCR1A &= ~bit(COM1B0);
      exp_start = TCNT1;
      OCR1B += control.camera.exposure_duration;
    } else
    {
      // we just finished our exposure, wrap it up
      exp_stop = TCNT1;
      TIMSK1 = 0;
      TCCR1B = 0;
      TCCR1A = 0;
    }
}

ISR(TIMER1_COMPA_vect)
{
    if (TCCR1A & bit(COM1A0))
    {
      TCCR1A &= ~bit(COM1A0);
      OCR1A += control.camera.flash_duration;
    }
}
void setup ()
{
    Serial.begin(115200);
    control.setup();
}

void print_timers(uint8_t tcnt)
{
    Serial.print("[TCNT1 0x");
    Serial.print(tcnt, DEC);
    Serial.print("] [OCR1A 0x");
    Serial.print(OCR1A, DEC);
    Serial.print("] [OCR1B 0x");
    Serial.print(OCR1B, DEC);
    Serial.print("] [TCCR1A 0x");
    Serial.print(TCCR1A, BIN);
    Serial.print("] [TCCR1B 0x");
    Serial.print(TCCR1B, BIN);
    Serial.print("]\r\n");
}


void loop ()
{
  if(triggered)
  {
    triggered = false;
    control.laser.off();
    if (control.dryrun)
    {
      control.disarm();
    }
    Serial.println("TRG");
    //print_timers(triggered);
  }

  if(exp_start)
  {
    exp_start = 0;
    //Serial.print("O: ");
    //print_timers(exp_start);
  }

  if(exp_stop)
  {
    control.disarm();
    exp_stop = 0;
    //Serial.print("o: ");
    //print_timers(exp_stop);
  }

  if(Serial.available())
  {
    char ch = Serial.read();
    parser.feed(ch);
    //Serial.println(ch);
  }
}
