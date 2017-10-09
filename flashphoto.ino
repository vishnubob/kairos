#include "Control.h"
#include "Parser.h"

Control control;
Parser parser(control);
volatile uint16_t triggered = 0;
volatile uint16_t exp_start = 0;
volatile uint16_t exp_stop = 0;

ISR (ANALOG_COMP_vect)
{
    if (!control.armed) return;
    cli();
    ACSR = bit(ACD);
    // enable int on match with OCR1B
    triggered = TCNT1;
    OCR1A = MAX_EXPOSURE_DURATION;
    OCR1B = control.camera.exposure_delay;
    TCCR1A = bit(COM1B1) | bit(COM1B0);
    TCCR1B = bit(WGM12) | bit(CS10) | bit(CS11);
    TIMSK1 = bit(OCIE1B);
    sei();
}

ISR(TIMER1_COMPB_vect)
{
    cli();
    if (TCCR1A & bit(COM1B0))
    {
      // we just exposed the camera, configure pin to turn off on next MATCH
      // configure next match at 100ms
      TCCR1B = 0;
      TCCR1A = bit(COM1B1);
      exp_start = TCNT1;
      OCR1B = control.camera.exposure_duration;
      TCNT1 = 1;
      TCCR1B = bit(WGM12) | bit(CS10) | bit(CS11);
      //digitalWrite(PIN_FLASH_FIRE, HIGH);
    } else
    {
      // we just finished our exposure, wrap it up
      exp_stop = TCNT1;
      TIMSK1 = 0;
      TCCR1B = 0;
      TCCR1A = 0;
      //digitalWrite(PIN_FLASH_FIRE, LOW);
    }
    sei();
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
    control.laser.off();
    Serial.print("T: ");
    print_timers(triggered);
    triggered = 0;
  }
  if(exp_start)
  {
    Serial.print("O: ");
    print_timers(exp_start);
    exp_start = 0;
  }
  if(exp_stop)
  {
    Serial.print("o: ");
    control.disarm();
    print_timers(exp_stop);
    exp_stop = 0;
  }
  if(Serial.available())
  {
    char ch = Serial.read();
    parser.feed(ch);
    //Serial.println(ch);
  }
}
