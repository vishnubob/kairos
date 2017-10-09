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
          control.camera.expose();
          break;
        case 'A':
          control.arm();
          break;
        case 'D':
          control.disarm();
          break;
        case 'L':
          control.laser.toggle();
          break;
        case 'S':
          // sleep time
          control.camera.exposure_delay = val;
          val = 0;
          break;
        case 'F':
          // fire time
          control.camera.exposure_duration = val;
          val = 0;
          break;
        case 'P':
          Serial.print("exp: [");
          Serial.print(control.camera.exposure_delay / 250000.0, 4);
          Serial.print(", ");
          Serial.print(control.camera.exposure_duration / 250000.0, 4);
          Serial.println("]");
          break;
    }
  }

Control &control;
};
