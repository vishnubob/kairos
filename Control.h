#define PIN_CAMERA_SHUTTER  10
#define PIN_CAMERA_FOCUS    4
#define PIN_LASER_POWER     3

// Helper macros for frobbing bits
#define bitset(var,bitno) ((var) |= (1 << (bitno)))
#define bitclr(var,bitno) ((var) &= ~(1 << (bitno)))
#define bittst(var,bitno) (var& (1 << (bitno)))

typedef void (*_isr_t)();

// 
// :: Camera ::
//

class Camera
{
public:
  Camera(uint8_t shutter=PIN_CAMERA_SHUTTER, uint8_t focus=PIN_CAMERA_FOCUS)
    : pin_shutter(shutter), pin_focus(focus) {}

  void setup()
  {
    pinMode(pin_shutter, OUTPUT);
    digitalWrite(pin_shutter, LOW);
    pinMode(pin_focus, OUTPUT);
    digitalWrite(pin_focus, LOW);
    TIMSK1 = 0;
    TCCR1B = 0;
    TCCR1A = 0;
  }

  void unlatch()
  {
    digitalWrite(pin_shutter, LOW);
  }

  void arm()
  {
    OCR1B = 10000;
    OCR1A = 60000;
  }

  void expose(void) __attribute__((always_inline))
  {
      digitalWrite(pin_shutter, HIGH);
      _delay_ms(75);
      digitalWrite(pin_shutter, LOW);
  }

  uint8_t   pin_shutter;
  uint8_t   pin_focus;
};

//
// :: Laser ::
//
class Laser
{
public:
  Laser(uint8_t power=PIN_LASER_POWER)
    : pin_power(power) {}

  void setup()
  {
    pinMode(pin_power, OUTPUT);
    digitalWrite(pin_power, LOW);
  }

  void on (void) { digitalWrite(pin_power, HIGH); }
  void off (void) { digitalWrite(pin_power, LOW); }
  bool is_on (void) { return (digitalRead(pin_power) == HIGH); }
  void toggle (void) { if (digitalRead(pin_power)) off(); else on(); }

  uint8_t   pin_power;
};

//
// :: LDR ::

class LDR
{
public:
  void setup()
  {
      pinMode(6, INPUT);
      pinMode(7, INPUT);
      ADCSRB = 0;
      ACSR = bit(ACD);
  }

  void arm()
  {
      // we want to select for the rising edge
      ACSR = bit(ACIS0) | bit(ACIS1);
      //cli();
      //ACSR = bit(ACIE) | bit(ACIS0) | bit(ACIS1);
      //sei();
  }

  void disarm()
  {
      ACSR = bit(ACD);
  }
};


//
// :: Control ::
//
class Control
{
public:
  void setup()
  {
    camera.setup();
    laser.setup();
    ldr.setup();
  }

  void arm (void)
  {
    if (armed) return;
    laser.on();
    camera.arm();
    ldr.arm();
    ACSR |= bit(ACIE);
    _delay_ms(500);
    armed = true;
  }

  void disarm (void)
  {
    if (!armed) return;
    ldr.disarm();
    laser.off();
    armed = false;
  }

  bool volatile armed;
  LDR ldr;
  Laser laser;
  Camera camera;
  uint16_t exposure_delay;
};
