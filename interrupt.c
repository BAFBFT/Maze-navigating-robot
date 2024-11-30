#include <xc.h>
#include "interrupt.h"

/************************************
 * Function to turn on interrupts and set if priority is used
 * Note you also need to enable peripheral interrupts in the INTCON register to use CM1IE.
************************************/
void Interrupts_init(void)
{
	// turn on global interrupts, peripheral interrupts and the interrupt source 
	// It's a good idea to turn on global interrupts last, once all other interrupt configuration is done.
        
    PIE2bits.C2IE = 1;
    PIE2bits.C3IE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    //char is_dark;
}

/************************************
 * High priority interrupt service routine
 * Make sure all enabled interrupts are checked and flags cleared
************************************/


void __interrupt(high_priority) HighISR()
{
	//add your ISR code here i.e. check the flag, do something (i.e. toggle an LED), clear the flag...
    if (PIR2bits.C3IF) {                //triggers on the rising edge of comparator (when the LDR output goes from low to high)

}
}