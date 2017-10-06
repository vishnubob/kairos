ISR(TIMER1_COMPA_vect) 
{
    /* disable this interrupt */
    //bitclr(TIMSK1, OCIE1A);
    timer_callback();
}

ISR (ANALOG_COMP_vect)
{
  analog_callback();
}


