#include "Trigger.h"
#include "Timer.h"

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
{}

class SerialFrontend : public Frontend
{
public:
    SerialFront(Control &ctrl) : control(ctrl) {}
    long val = 0;

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
        case 'X':
          Serial.println("OK");
          control.camera.trigger();
          break;
        case 'a':
          Serial.println("OK");
          control.arm_dryrun();
          break;
        case 'A':
          Serial.println("OK");
          control.arm();
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
          control.camera.on_time = val;
          val = 0;
          Serial.println("OK");
          break;
        case 'F':
          control.camera.off_time = val;
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
        case 'P':
          Serial.print("{'exposure_delay': ");
          Serial.print(control.camera.exposure_delay, DEC);
          Serial.print(", 'exposure_duration': ");
          Serial.print(control.camera.exposure_duration, DEC);
          Serial.print(", 'flash_delay': ");
          Serial.print(control.camera.flash_delay, DEC);
          Serial.print(", 'flash_duration': ");
          Serial.print(control.camera.flash_duration, DEC);
          Serial.println("}");
          break;
    }
  }

public:
  Control &control;
};