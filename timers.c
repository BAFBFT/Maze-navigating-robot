#include <xc.h>
#include "timers.h"

/************************************
 * Function to set up timer 0
************************************/
void Timer0_init(void)
{
    T0CON1bits.T0CS = 0b010;     // Fosc/4 = 16 MHz
    T0CON1bits.T0ASYNC = 1;      // Asynchronous mode for Fosc/4
    T0CON1bits.T0CKPS = 0b1000;  // Prescaler 1:256
    T0CON0bits.T016BIT = 1;
    // Preload Timer0 to overflow after 0.25 seconds
    TMR0H = 0x0B;                // High byte of 0xC2F7
    TMR0L = 0xDC;                // Low byte of 0xC2F7
    T0CON0bits.T0EN = 1;         // Start the timer
}

/************************************
 * Function to return the full 16bit timer value
 * Note TMR0L and TMR0H must be read in the correct order, or TMR0H will not contain the correct value
************************************/
unsigned int get16bitTMR0val(void)
{   
    unsigned int lowByte, highByte;  
    lowByte = TMR0L;
    highByte = TMR0H;
    
    return highByte;
}

