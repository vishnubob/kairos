
Comparator kairos_comparator;

ISR (ANALOG_COMP_vect)
{
    sei();
    if (kairos_comparator.is_armed())
    {
        kairos_comparator.trigger();
    }
    cli();
}

