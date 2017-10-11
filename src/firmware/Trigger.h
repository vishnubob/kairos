#ifndef _KAIROS_TRIGGER_H
#define _KAIROS_TRIGGER_H

class Trigger
{
public:
  // Enable / Disable (pure virtual)
  virtual void enable() = 0;
  virtual void disable() = 0;
  virtual void is_enabled() = 0;

  // Arm / Disarm
  void arm() { cli(); _armed = true; _triggered = false; sei(); }
  void disarm() { cli(); _armed = false; _triggered = false; sei(); }
  __attribute__((always_inline)) 
    bool is_armed() { cli(); bool ret = _armed; sei(); return ret; }

  // Trigger
  __attribute__((always_inline))
      void trigger()
      {
        cli();
        if (_armed && !_triggered)
        {
          _triggered = true;
          if (_callback) _callback();
        }
        sei();
      }
  __attribute__((always_inline)) 
    bool is_triggered() { cli(); bool ret = _triggered; sei(); return ret; }
  __attribute__((always_inline)) 
    void reset_trigger() { cli(); _triggered = false; sei(); }

  // Trigger Callback
  void set_callback(callback_t callback=NULL) { _callback = callback; }
  callback_t get_callback() { return _callback; }

public:
  typedef void (*callback_t)();

private:
  volatile bool _armed;
  volatile bool _triggered;
  volatile callback_t callback;
};

class ComparatorTrigger : public Trigger
{
public:
  Comparator()
  {
      pinMode(6, INPUT);
      pinMode(7, INPUT);
      enable();
  }

  virtual void enable() { cli(); ACSR = Flags_ACSR; sei(); }
  virtual void disable() { cli(); ACSR = bit(ACD); sei(); }
  virtual is_enabled() { cli(); bool ret = ACSR & bit(ACD); sei(); }

public:
  // XXX: hardwired for rising edge
  const uint8_t Flags_ACSR = bit(ACIS0) | bit(ACIS1) | bit(ACIE);

};

extern ComparatorTrigger comparator_trigger;

#define // _KAIROS_TRIGGER_H
