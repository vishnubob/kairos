#include "Trigger.h"

ComparatorTrigger comparator_trigger;

ISR (ANALOG_COMP_vect) { comparator_trigger.trigger(); }

