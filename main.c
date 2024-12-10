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
#include "timers.h"


void main(void) {
    Interrupts_init();
    Timer0_interrupt_init();

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
    char calibrate = 0;
    extern volatile unsigned int overflowCount; // Global declaration

    while (1) {
        // Turn calibration routine
        if (!PORTFbits.RF3) {  // Detect button press
            calibrate = 1; 
        }     
        
        if (calibrate) {
            setCalibrationLED();
            calibrationRoutine(&motorL, &motorR);
            turnOffLEDs();
            calibrate = 0;
        }
        
        // Maze solving routine
        if (!PORTFbits.RF2) {  // Detect button press
            go = 1;
            Timer0_init();
            overflowCount = 0;
        }        

        if (go) {
            setGoLED();
            fullSpeedAhead(&motorL, &motorR);
            
            // read ambient light level and set threshold for obstacle 
            // might need to add condition depending on light level, it sometimes interrupts too soon
            // i.e. if the light condition is > this change the percentage needed for measurement routine
            unsigned int clearVal = color_read_Clear();
            unsigned int clearThreshold = (93 * clearVal) / 100;

            if (color_read_Clear() < clearThreshold) {  // Detect obstacle
                // stop motors
//                sendStringSerial4("Current overflowcount:");
//                sendUnsignedIntSerial4(overflowCount);
                stop(&motorL, &motorR);
                
                // Record the time spent moving forward (overflowCount * 0.25 seconds)
//                sendStringSerial4("Current overflowCount: ");
//                sendUnsignedIntSerial4(overflowCount);
                push(&timeStack, overflowCount);
                
                
                //align and read color
                turnOffLEDs();
                wallAlign(&motorL, &motorR); 
                color = ReadHSV();
                char command = ClassifyColor(color);

                
                // Execute the command
                CommandBuggy(&motorL, &motorR, command);
                
                // Reset overflow count for the next forward movement
                overflowCount = 0; 
                
                if (command == WHITE || command == LOST){  // White detected (or lost), go home
                    goHome(&motorL, &motorR, &timeStack, &commandStack);
                    
                    // Stop motors, arrived home
                    go = 0;
                    stop(&motorL, &motorR);
                    turnOffLEDs();
                    break;
                } else {
                    // Push the flipped command onto the stack
                    push(&commandStack, flipCommand(command)); 
                }
            }
        }
    }
}
