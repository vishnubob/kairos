/******************************************************************************
 ** Serial
 ******************************************************************************/

void ReportStatus(void)
{
    Serial.println("");
    Serial.println("Laser:");
    Serial.print("  sensor: ");
    Serial.println(control.ldr_value, DEC);
    Serial.print("  threshold: ");
    Serial.println(control.ldr_threshold, DEC);
    Serial.print("  exposure delay: ");
    Serial.println(control.exposure_delay, DEC);
    Serial.println(OCR1A, DEC);
}

void prompt(void)
{
  static long v = 0;

  if (Serial.available()) {
    char ch = Serial.read();

    switch(ch) {
      case '0'...'9':
        v = v * 10 + ch - '0';
        break;
      case '-':
        v *= -1;
        break;
      case 'z':
        v = 0;
        break;
      case 'x':
        control.expose();
        break;
      case 'a':
        /* arm camera */
        control.arm();
        break;
      case 'f':
        /* disarm camera */
        control.disarm();
        break;
      case 'l':
        control.laser.toggle();
        break;
      case 't':
        control.ldr_threshold = v;
        v = 0;
        break;
      case 'd':
        /* delay in ticks */
        control.exposure_delay = v;
        v = 0;
        break;
      case 'p':
        /* Report on status of system */
        ReportStatus();
        break;
    }
    Serial.println("");
    Serial.print("Value:");
    Serial.println(v);
    Serial.print("> ");
  }
}

void loop() { prompt(); }

/******************************************************************************
 ** Setup
 ******************************************************************************/

void setup()
{
  Serial.begin(9600);
  Serial.print("[");
  control.setup();

  // setup timer1 - 16
  // resonsible for timing the camera after an event
  TCCR1A = 0;
  TCCR1B = 0;
  // 1:256
  bitset(TCCR1B, CS12);
  // select CTC mode
  bitset(TCCR1A, WGM12);
  // enable compare interrupt
  Serial.print("timer-");

  // Configure the ADC
  ADMUX = (1 << 6) | (LASER_SENSOR_PIN & 0x0f);
  // start a converson
  bitset(ADCSRA, ADSC);
  // enable the interrupt to store the conversion result
  bitset(ADCSRA, ADIE);
  Serial.print("ADC");
  
  // enable global interrupts
  sei();

  Serial.println("]");
  Serial.flush();
}



