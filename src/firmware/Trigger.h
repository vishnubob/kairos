#ifndef _KAIROS_TRIGGER_H
#define _KAIROS_TRIGGER_H

class Trigger
{
public:
  virtual void enable() = 0;
  virtual void disable() = 0;

  void arm() { _armed = true; }
  void disarm() { _armed = false; }
  __attribute__((always_inline)) 
    bool is_armed() { return _armed; }

  void set_callback(callback_t callback=NULL) { _callback = callback; }
  callback_t get_callback() { return _callback; }
  __attribute__((always_inline))
    void trigger() { if (_callback) _callback(); }

public:
  typedef void (*callback_t)();

private:
  volatile bool _armed;
  volatile callback_t callback;
};

class ComparatorTrigger : public Trigger
{
public:
  Comparator()
  {
      pinMode(6, INPUT);
      pinMode(7, INPUT);
      disable();
  }

  void enable()
  {
      cli();
      // XXX: hardwired for rising edge
      ACSR = bit(ACIS0) | bit(ACIS1) | bit(ACIE);
      sei();
  }

  void disable()
  {
      cli();
      ACSR = bit(ACD);
      sei();
  }

protected:
  volatile bool _armed;
  sensor_callback_t callback;
};

extern Comparator kairos_comparator;

#define // _KAIROS_TRIGGER_H
