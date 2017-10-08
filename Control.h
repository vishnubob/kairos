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

#define DEFAULT_EXPOSURE_DELAY 10000
#define DEFAULT_EXPOSURE_DURATION 25000
#define MAX_EXPOSURE_DURATION 60000

class Camera
{
public:
  Camera(uint8_t shutter=PIN_CAMERA_SHUTTER, uint8_t focus=PIN_CAMERA_FOCUS, uint16_t exp_delay=DEFAULT_EXPOSURE_DELAY, uint16_t exp_duration=DEFAULT_EXPOSURE_DURATION) :
    pin_shutter(shutter), 
    pin_focus(focus),
    exposure_delay(exp_delay),
    exposure_duration(exp_duration)
  {}

  void setup()
  {
      pinMode(pin_shutter, OUTPUT);
      digitalWrite(pin_shutter, LOW);
      pinMode(pin_focus, OUTPUT);
      digitalWrite(pin_focus, LOW);
      TCCR0A = TCCR0B = 0;
      TCCR1A = TCCR1B = 0;
      TCNT1 = 1;
  }

  void arm()
  {
      TCCR1A = TCCR1B = 0;
      TCNT1 = 1;
  }

  void disarm()
  {
      TCCR1A = TCCR1B = 0;
      TCNT1 = 1;
  }

  void expose(void) __attribute__((always_inline))
  {
      digitalWrite(pin_shutter, HIGH);
      _delay_ms(75);
      digitalWrite(pin_shutter, LOW);
  }

  uint16_t  exposure_delay;
  uint16_t  exposure_duration;
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
      ACSR = bit(ACIS0) | bit(ACIS1) | bit(ACIE);
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
    armed = false;
    cli();
    camera.setup();
    laser.setup();
    ldr.setup();
    sei();
  }

  void arm (void)
  {
    cli();
    if (armed) return;
    laser.on();
    camera.arm();
    ldr.arm();
    sei();
    _delay_ms(500);
    armed = true;
  }

  void disarm (void)
  {
    cli();
    if (!armed) return;
    ldr.disarm();
    camera.disarm();
    laser.off();
    armed = false;
    sei();
  }

  bool volatile armed;
  LDR ldr;
  Laser laser;
  Camera camera;
};
