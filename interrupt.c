#include <xc.h>
#include "interrupt.h"
#include "i2c.h"

/************************************
 * Function to turn on interrupts and set if priority is used
 * Note you also need to enable peripheral interrupts in the INTCON register to use CM1IE.
************************************/
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

void clear_interrupt_init(void)
//function to set the boundary levels for the interrupt on the color board
{
    I2C_Write_Register(0x00, 0x03);     //turns on the board and enables RGBC (bit 0 and bit 1 respectively)
    I2C_Write_Register(0x01, 0x00);     //ATIME = 0. This means that integration time is the default value of 700ms
    I2C_Write_Register(0x0F, 0b00);     //AGAIN = 0. This means that the gain is 1. again 1 ==> 4, again 2 ==> 16, again 3 ==> 60
    I2C_Write_Register(0x04, 0x20);     //set AILTL to 0x20
    I2C_Write_Register(0x05, 0x00);     //set AILTH to 0x00. AILT now = 0x0020
    I2C_Write_Register(0x06, 0xFF);     //set AIHTL to 0xFF
    I2C_Write_Register(0x07, 0xFF);     //set AIHTH to 0xFF. AIHT now = 0xFFFF (we dont care about the upper value)
    I2C_Write_Register(0x00, 0x13);     //turns the enable interrupt bit AIEN
}

void TCS3471_ISR() {
    unsigned char is_interrupt = I2C_Read_Register(0x13);   //reads the status register of the board
    if (is_interrupt & 0x10) {
        //code to stop the timer
        //code to drive the buggy forward for 1 second
        LATDbits.LATD7=1;
    } else {
        LATDbits.LATD7=0;
    }
}
/************************************
 * High priority interrupt service routine
 * Make sure all enabled interrupts are checked and flags cleared
************************************/


void __interrupt(high_priority) HighISR()
{
	//add your ISR code here i.e. check the flag, do something (i.e. toggle an LED), clear the flag...
    if (PIR0bits.INT1IF) {                //triggers on the rising edge of comparator (when the LDR output goes from low to high)
        TCS3471_ISR();
        PIR0bits.INT1IF = 0;
    }
}