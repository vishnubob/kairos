#include "Control.h"
#include "Parser.h"

Control control;
Parser parser(control);
volatile bool triggered = false;
volatile bool comp_b = false;
volatile bool comp_a = false;

#define PRINT_16BREG(reg, name, page) { \
    unsigned short value = reg; \
    Serial.println(F("* " #reg "(H/L) - " name " (p" #page "):")); \
    Serial.print(F("  " #reg "[15:8]: ")); \
    Serial.println(value >> 8, BIN); \
    Serial.print(F("  " #reg "[7:0]:  ")); \
    Serial.println(value & 0xff, BIN); \
    Serial.print(F("  16-bit value: 0x")); \
    Serial.print(value, HEX); \
    Serial.print(F(" / ")); \
    Serial.print(value, DEC); \
    Serial.print(F(" / 0")); \
    Serial.print(value, OCT); \
    Serial.print(F(" / 0b")); \
    Serial.println(value, BIN); \
    Serial.println(); \
}

ISR (ANALOG_COMP_vect)
{
    if (!control.armed) return;
    TCNT1 = 0;
    ACSR = bit(ACD);
    triggered = true;
    TCCR1A = bit(COM1B1) | bit(COM1B0);
    // turn timer on, clk / 64
    TCCR1B = bit(WGM12) | bit(CS10) | bit(CS11);
    TIMSK1 = bit(OCIE1B);
    //control.disarm();
    // triggered
}

ISR(TIMER1_COMPB_vect)
{
    // we just exposed the camera, configure pin to turn off on next MATCH
    // configure next match at 100ms
    OCR1B = 10000 + 25000;
    TCCR1A = bit(COM1B1);
    TIMSK1 = bit(OCIE1A);
    comp_b = true;
}

ISR(TIMER1_COMPA_vect)
{
    // turn off timer
    TIMSK1 = 0;
    TCCR1B = 0;
    TCCR1A = 0;
    comp_a = true;
}

void setup ()
{
    Serial.begin(9600);
    TCCR0A = 0;
    TCCR1A = 0;
    control.setup();
}

void loop ()
{
  /*
  if(control.armed)
  {
    uint16_t tcnt = TCNT1;
    PRINT_16BREG(TCNT1, "Timer/Counter1", 135);
    PRINT_16BREG(OCR1A, "Output Compare Register 1 A", 136);
    PRINT_16BREG(OCR1B, "Output Compare Register 1 A", 137);
    _delay_ms(2000);
  }
  */
  if(triggered)
  {
    control.disarm();
    Serial.print('T');
    triggered = false;
  }
  if(comp_a)
  {
    Serial.print('A');
    comp_a = false;
  }
  if(comp_b)
  {
    Serial.print('B');
    comp_b = false;
  }
  if(Serial.available())
  {
    char ch = Serial.read();
    parser.feed(ch);
    //Serial.println(ch);
  }
}
