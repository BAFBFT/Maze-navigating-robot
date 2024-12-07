
 // CONFIG1L
#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)

// CONFIG3L
#pragma config WDTCPS = WDTCPS_31// WDT Period Select bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)


#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  


#include <xc.h>
#include "color.h"
#include "serial.h"
#include "dc_motor.h"
#include "interrupt.h"



void main(void) {
    initDCmotorsPWM(199);
    Interrupts_init();
    clear_interrupt_init();
    
	//don't forget TRIS for your output!
    TRISDbits.TRISD7 = 0; //for testing the clear interrupt
    // Define motors
    DC_motor motorL = {
        .power = 0,
        .direction = 1,
        .brakemode = 1,
        .posDutyHighByte = (unsigned char *)(&CCPR1H),
        .negDutyHighByte = (unsigned char *)(&CCPR2H),
        .PWMperiod = 199
    };

    DC_motor motorR = {
        .power = 0,
        .direction = 1,
        .brakemode = 1,
        .posDutyHighByte = (unsigned char *)(&CCPR3H),
        .negDutyHighByte = (unsigned char *)(&CCPR4H),
        .PWMperiod = 199
    };

    //turns headlights on
    TRISDbits.TRISD3 = 0;
    LATDbits.LATD3 = 0;
    
    //initialise left turn signals
    TRISFbits.TRISF0 = 0;
    LATFbits.LATF0 = 0;
    
    //initialise right turn signals
    TRISEbits.TRISE5 = 0;
    LATEbits.LATE5 = 0;
    
    //initialise brake turn signals
    TRISFbits.TRISF7 = 0;
    LATFbits.LATF7 = 0;
    
    //initialise buttons
    TRISFbits.TRISF2 = 1;
    ANSELFbits.ANSELF2 = 0;
    TRISFbits.TRISF3 = 1;
    ANSELFbits.ANSELF3 = 0;
    
    color_click_init();
    initUSART4();
    while(1){
        RGBC color = MeasureRGBC();

        //__delay_ms(100);
        sendUnsignedIntSerial4(color.R);
        //__delay_ms(100);
        sendUnsignedIntSerial4(color.G);
        //__delay_ms(100);
        sendUnsignedIntSerial4(color.B);
        sendUnsignedIntSerial4(color.C);
    }
}