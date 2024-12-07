
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



void main(void) {
    initDCmotorsPWM(199);
	//don't forget TRIS for your output!
    
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
    
    
    char go = 0;
    while(1){
//        RGBC color_1 = MeasureRGBC();
//        HSV color = ReadHSV();

//        sendStringSerial4(classify_color(MeasureRGBC()));
          // 

//        sendUnsignedIntSerial4(color_1.R);
//        sendUnsignedIntSerial4(color_1.G);
//        sendUnsignedIntSerial4(color_1.B);
//        sendUnsignedIntSerial4(color_1.C);
        
        
        if (!PORTFbits.RF2){ //detect button press
            go = 1; }        
        if (go) {

            fullSpeedAhead(&motorL, &motorR);
        
            if ((color_read_Clear() < 100)){
                char j = 0;
                while (j < 5) {
                    fullSpeedAhead(&motorL, &motorR);
                    __delay_ms(150);
                    j++;
                }
                stop(&motorL, &motorR);  
                HSV color = ReadHSV();
                
                
                
                if (ClassifyColor(color) == 1){
                    reverse(&motorL, &motorR);
                    turnRight(&motorL, &motorR);
                } else if (ClassifyColor(color) == 7) {
                    reverse(&motorL, &motorR);
                    turnLeft(&motorL, &motorR);        
                } else if (ClassifyColor(color) == 8){
                    reverse(&motorL, &motorR);
                    for (char i = 0; i < 2; i++){
                        turnLeft(&motorL, &motorR);
                        __delay_ms(800);
                    }
                }
            }
        }              
    }
}