#include "Trigger.h"
#include "Timer.h"
#include "Control.h"

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

class Frontend
{
};

//class SerialFrontend : public Frontend
class SerialFrontend
{
public:
    SerialFrontend(Control &ctrl) : control(ctrl) {}
    long val = 0;

    void refresh()
    {
        if(Serial.available())
        {
          char ch = Serial.read();
          feed(ch);
        }
    }
  
    void feed(char ch)
    {
      switch(ch) {
        case '0'...'9':
          val = val * 10 + ch - '0';
          break;
        case '-':
          val *= -1;
          break;
        case 'Z':
          val = 0;
          break;
        case 'f':
          control.focus.strobe();
          Serial.println("OK");
          break;
        case 'x':
          control.shutter.strobe();
          Serial.println("OK");
          break;
        case 'X':
          control.timer.start();
          Serial.println("OK");
          break;
        case 'a':
          control.arm(true);
          Serial.println("OK");
          break;
        case 'A':
          control.arm();
          Serial.println("OK");
          break;
        case 'D':
          control.disarm();
          Serial.println("OK");
          break;
        case 'L':
          control.laser.toggle();
          Serial.println("OK");
          break;
        case 'S':
          control.shutter.on_time = val;
          val = 0;
          Serial.println("OK");
          break;
        case 'F':
          control.shutter.off_time = val;
          val = 0;
          Serial.println("OK");
          break;
        case 'Q':
          control.flash.on_time = val;
          val = 0;
          Serial.println("OK");
          break;
        case 'U':
          control.flash.off_time = val;
          val = 0;
          Serial.println("OK");
          break;
        case 'K':
          control.mirror_lockup = !control.mirror_lockup;
          Serial.println("OK");
          break;
        case 'P':
          Serial.print("{'flash': {'on_time': ");
          Serial.print(control.flash.on_time, DEC);
          Serial.print(", 'off_time': ");
          Serial.print(control.flash.off_time, DEC);
          Serial.print("}, 'shutter': {'on_time': ");
          Serial.print(control.shutter.on_time, DEC);
          Serial.print(", 'off_time': ");
          Serial.print(control.shutter.off_time, DEC);
          Serial.println("}}");
          break;
    }
  }

public:
  Control &control;
};
