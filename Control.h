#define PIN_CAMERA_SHUTTER  9
#define PIN_CAMERA_FOCUS    10
#define PIN_LASER_POWER     11
#define PIN_LDR_VOLTAGE     A0

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
  void Camera(uint8_t shutter=PIN_CAMERA_SHUTTER, uint8_t focus=PIN_CAMERA_FOCUS)
    : pin_shutter(shutter), pin_focus(focus) {}

  void setup()
  {
    pinMode(pin_shutter, OUTPUT);
    digitalWrite(pin_shutter, LOW);
    pinMode(pin_focus, OUTPUT);
    digitalWrite(pin_focus, LOW);
  }

  void expose(void) __attribute__((always_inline))
  {
      digitalWrite(pin_shutter, LOW);
      _delay_us(1000);
      digitalWrite(pin_shutter, HIGH);
  }

  uint8_t   pin_shutter;
  uint8_t   pin_focus;
}

//
// :: Laser ::
//
class Laser
{
  Laser(uint8_t power=LASER_PIN_POWER)
    : pin_power(power) {}

  void setup()
  {
    pinMode(pin_power, OUTPUT);
    digitalWrite(pin_power, LOW);
  }

  void on (void) { digitalWrite(_pin_power, HIGH); }
  void off (void) { digitalWrite(_pin_power, LOW); }
  bool is_on (void) { return (digitalRead(_pin_power) == HIGH)}
  void toggle (void) { if digitalRead(_pin_power) off(); else on(); }
};

//
// :: LDR ::

class LDR
{
  LDR(sensor=PIN_LDR_SENSOR, reference=PIN_LDR_REFERENCE)
    : pin_sensor(sensor), pin_reference(reference)
  {}

  void setup()
  {
    // XXX: cli() / sei() ?
    // XXX: assert not setup
    // cache our previous state
    __prior_ADCSRB = ADCSRB;
    __prior_ACSE = ACSR;

    // (Disable) ACME: Analog Comparator Multiplexer Enable
    ADCSRB = 0;
    // (Clear) Analog Comparator Interrupt Flag
    bitset(ACSR, ACI);
    // Analog Comparator Interrupt Enable
    bitset(ACSR, ACIE)    
    // ACIS1, ACIS0: Analog Comparator Interrupt Mode Select (trigger on falling edge)
    bitset(ACSR, ACIS1);  
  }
};


//
// :: Control ::
//
class Control
{
  void arm (void)
  {
    _laser.on();
    // XXX: wait until LDR stabalizes?
    _delay_us(10000);
    _armed = true;
  }

  void disarm (void)
  {
    _laser.on();
    _delay_us(10000);
    _armed = true;
  }

  bool _armed;
  LDR ldr;
  Laser laser;
  Camera camera;
}

