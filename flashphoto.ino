volatile boolean triggered = false;

ISR (ANALOG_COMP_vect)
{
   // clk / 1
   // CTC mode, OCR1A configured as top
   //TCCR1B = bit(CS10) | bit(WGM12);
   //ACSR = 0;
   triggered = true;
}

void arm()
{
    ADCSRB = 0;
    ACSR =  bit(ACI) | bit(ACIE) | bit(ACIS1);
    //TCCR1B = 0;
    // set OC1A to high on CTC match
    //TCCR1A = bit(COM1A1) | bit(COM1A0); 
    //TCNT1 = 0;
    //OCR1A = 200;
}

void setup ()
{
    Serial.begin (115200);
    Serial.println ("Started.");
}

void loop ()
{
    if (triggered)
    {
        unsigned long time = millis();
        Serial.print(time);
        Serial.println(" - Triggered!"); 
        triggered = false;
    }
}
