#define PIN_FLASH_FIRE      10 // port a
#define PIN_CAMERA_SHUTTER  9 // port b
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

#define DEFAULT_EXPOSURE_DELAY 18289
#define DEFAULT_FLASH_DELAY (DEFAULT_EXPOSURE_DELAY - 5190)
#define DEFAULT_FLASH_DURATION (3125 * 2)
#define DEFAULT_EXPOSURE_DURATION (3125 * 2)
#define MAX_EXPOSURE_DURATION 0xFFFF

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

class Camera
{
public:
  Camera(uint8_t shutter=PIN_CAMERA_SHUTTER, uint8_t focus=PIN_CAMERA_FOCUS, uint8_t flash=PIN_FLASH_FIRE, uint16_t exp_delay=DEFAULT_EXPOSURE_DELAY, uint16_t exp_duration=DEFAULT_EXPOSURE_DURATION, uint16_t fls_delay=DEFAULT_FLASH_DELAY, uint16_t fls_duration=DEFAULT_FLASH_DURATION) :
    pin_shutter(shutter), 
    pin_focus(focus),
    pin_flash(flash),
    exposure_delay(exp_delay),
    exposure_duration(exp_duration),
    flash_delay(fls_delay),
    flash_duration(fls_duration)
  {}

  void setup()
  {
      pinMode(pin_shutter, OUTPUT);
      digitalWrite(pin_shutter, LOW);
      pinMode(pin_focus, OUTPUT);
      digitalWrite(pin_focus, LOW);
      pinMode(pin_flash, OUTPUT);
      digitalWrite(pin_flash, LOW);
      //TCCR0A = TCCR0B = 0;
      TCCR1A = TCCR1B = 0;
      TCNT1 = 0;
  }

  void arm()
  {
      //TCCR1A = TCCR1B = 0;
      //TCNT1 = 0;
      digitalWrite(pin_focus, HIGH);
  }

  void disarm()
  {
      TCCR1A = TCCR1B = 0;
      TCNT1 = 0;
      digitalWrite(pin_focus, LOW);
  }

  void expose_force(size_t delay_ms=60) __attribute__((always_inline))
  {
      digitalWrite(pin_shutter, HIGH);
      _delay_ms(delay);
      digitalWrite(pin_shutter, LOW);
  }

  void flash_force(size_t delay_ms=60) __attribute__((always_inline))
  {
      digitalWrite(pin_flash, HIGH);
      _delay_ms(delay);
      digitalWrite(pin_flash, LOW);
  }

  void expose(void) __attribute__((always_inline))
  {
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

  uint16_t  exposure_delay;
  uint16_t  exposure_duration;
  uint16_t  flash_delay;
  uint16_t  flash_duration;
  uint8_t   pin_shutter;
  uint8_t   pin_focus;
  uint8_t   pin_flash;
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
    dryrun = false;
    cli();
    camera.setup();
    laser.setup();
    ldr.setup();
    sei();
  }

  void arm_dryrun (void)
  {
    if (armed) return;
    cli();
    laser.on();
    ldr.arm();
    sei();
    _delay_ms(500);
    armed = dryrun = true;
  }


  void arm (void)
  {
    if (armed) return;
    cli();
    laser.on();
    camera.arm();
    ldr.arm();
    sei();
    _delay_ms(500);
    armed = true;
    dryrun = false;
  }

  void disarm (void)
  {
    if (!armed) return;
    cli();
    ldr.disarm();
    camera.disarm();
    laser.off();
    armed = false;
    dryrun = false;
    sei();
  }

  bool volatile armed;
  bool volatile dryrun;
  LDR ldr;
  Laser laser;
  Camera camera;
};
