#include <util/delay.h>

class Parser
{
public:
    Parser(Control &ctrl) : control(ctrl) {}
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
          control.camera.expose();
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
          control.camera.exposure_delay = val;
          val = 0;
          Serial.println("OK");
          break;
        case 'F':
          control.camera.exposure_duration = val;
          val = 0;
          Serial.println("OK");
          break;
        case 'Q':
          control.camera.flash_delay = val;
          val = 0;
          Serial.println("OK");
          break;
        case 'U':
          control.camera.flash_duration = val;
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

Control &control;
};
