
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
#include "home.h"



void main(void) {
    Interrupts_init();

    // Declare motor structs
    DC_motor motorL, motorR;
    motorStruct(&motorL, &motorR);

    // Other initializations    
    initBuggyLights();
    initButtons();   
    color_click_init();
    initUSART4();

    // Declare HSV struct
    HSV color;

    // Declare timeStack and commandStack structs
    Stack timeStack, commandStack;
    initialiseStack(&timeStack, &commandStack);

    char go = 0;

    while (1) {
        
//        color = ReadHSV();
//        __delay_ms(500);
//        sendUnsignedIntSerial4(ClassifyColor(color));
//        sendUnsignedIntSerial4(color.H);
//        sendUnsignedIntSerial4(color.S);
        
        if (!PORTFbits.RF2) {  // Detect button press
            go = 1; 
        }        

        if (go) {
            setGoLED();
            fullSpeedAhead(&motorL, &motorR);

            if (color_read_Clear() < 30) {  // Detect white or other colors
                wallAlign(&motorL, &motorR); 
                color = ReadHSV();
                char command = ClassifyColor(color);
                
                // Execute the command
                CommandBuggy(&motorL, &motorR, command);
                
                if (command == 5) {  // White detected
                    if (!isEmpty(&commandStack)) {
                        //turn on all LEDs
                        setCalibrationLED();
                        // Move forward for 2 seconds
                        fullSpeedAhead(&motorL, &motorR);
                        __delay_ms(2000);

                        // Execute the last command in reverse
                        char lastCommand = pop(&commandStack);
                        CommandBuggy(&motorL, &motorR, lastCommand);
                        stop(&motorL, &motorR);
                    } else {
                        // Stop if no commands left to retrace
                        go = 0;
                        turnOffLEDs();
                        break;
                    }
                } else {
                    // Push the flipped command onto the stack
                    push(&commandStack, flipCommand(command));

                }
            }
        }
    }
}
