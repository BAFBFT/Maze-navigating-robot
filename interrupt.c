#include <xc.h>
#include "interrupt.h"
#include "i2c.h"
#include "serial.h"

void Interrupts_init(void)
{
	// turn on global interrupts, peripheral interrupts and the interrupt source 
	// It's a good idea to turn on global interrupts last, once all other interrupt configuration is done.
    // RB1 is external interrupt 1
    PIE0bits.INT1IE = 1;
    INTCONbits.IPEN = 0;
    INTCONbits.INT1EDG = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
}

void Timer0_interrupt_init(void)
{

    PIE0bits.TMR0IE = 1; // Enable timer0 interrupt
    INTCONbits.PEIE = 1;      // Enable peripheral interrupts
    INTCONbits.GIE = 1;       // Enable global interrupts
    TRISHbits.TRISH3 = 0;     // Set RH3 as output for LED
}

void __interrupt(high_priority) HighISR_timer()
{
        if (PIR0bits.TMR0IF) {               // Check if Comparator 1 triggered the interrupt
        PIR0bits.TMR0IF = 0;             // Clear the interrupt flag
        
        overflowCount++;
        
        // Reload the timer to maintain accurate timing
        TMR0H = 0xFC;                    // High byte of 0xFC17
        TMR0L = 0x17;                    // Low byte of 0xFC17
    }    
}
