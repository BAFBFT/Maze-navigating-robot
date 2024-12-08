
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
//    Interrupts_init();
    //clear_interrupt_init();
    
// Declare motor structs
    DC_motor motorL, motorR;
    motorStruct(&motorL, &motorR);
//Other initialisations    
    initBuggyLights();
    initButtons();   
    color_click_init();
    initUSART4();

    char go = 0;
    while(1){
//        RGBC color_1 = MeasureRGBC();
//        HSV color = ReadHSV();
//        __delay_ms(200);
//        
//       sendUnsignedIntSerial4(ClassifyColor(color));
//        sendUnsignedIntSerial4(color_1.G);
//        sendUnsignedIntSerial4(color_1.B);
//        sendUnsignedIntSerial4(color_1.C);
//        __delay_ms(500);
//        sendUnsignedIntSerial4(color_read_Clear());
        
        if (!PORTFbits.RF2){ //detect button press
            go = 1; }        
        if (go) {
            LATDbits.LATD7=1;
            fullSpeedAhead(&motorL, &motorR);
        
            if ((color_read_Clear() < 30)){
                wallAlign(&motorL, &motorR);
                stop(&motorL, &motorR);  
                HSV color = ReadHSV();
                
                
                CommandBuggy(&motorL, &motorR, ClassifyColor(color));

            }
        }              
    }
}