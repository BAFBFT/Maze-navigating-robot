#include <xc.h>
#include "timers.h"

// Timer 0 initialisation.
void Timer0_init(void)
{
    T0CON1bits.T0CS = 0b010;     // Fosc/4 = 16 MHz
    T0CON1bits.T0ASYNC = 1;      // Asynchronous mode for Fosc/4
    T0CON1bits.T0CKPS = 0b1000;  // Prescaler 1:256
    T0CON0bits.T016BIT = 1;
    
    // Preload Timer0 to overflow after 10ms  seconds
    TMR0H = 0xFC;                // High byte of 0xFC17
    TMR0L = 0x17;                // Low byte of 0xFC17
    T0CON0bits.T0EN = 1;         // Start the timer
}


