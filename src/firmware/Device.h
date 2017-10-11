class Device
{
public:
  virtual void on (void) = 0;
  virtual void off (void) = 0;
  virtual bool is_on (void) = 0;
  virtual void toggle (void) = 0;
};


class PinDevice : public Device
{
public:
  PinDevice(uint8_t _pin) : pin(_pin) 
  {
    // XXX: hardwired to output
    pinMode(pin_power, OUTPUT);
    digitalWrite(pin_power, LOW);
  }

  virtual void on (void) { digitalWrite(pin, HIGH); }
  virtual void off (void) { digitalWrite(pin, LOW); }
  virtual bool is_on (void) { return (digitalRead(pin) == HIGH); }
  virtual void toggle (void) { if (is_on()) off(); else on(); }

public:
  uint8_t pin;
};


