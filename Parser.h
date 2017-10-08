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
          exposure_delay = v;
          v = 0;
          break;
    }
  }
}
